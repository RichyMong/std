import asyncio
import socket
import errno
import logging
import ouou.message
from datetime import datetime
from ..util.basetypes import Char

LOGGER = logging.getLogger("client")

def default_message_callback(c, data):
    LOGGER.debug('client[{}] receive data length: {}'.format(c.cid, len(data)))


def default_state_callback(c, state):
    LOGGER.debug('client[{}] state changed to: {}'.format(c.cid, state))


class BaseClient(object):
    # State change: CLOSED -> CONNECTING -> CONNECTED
    # Afterwards, if the user calls close(),    -> CLOSING -> CLOSED
    # else -> CLOSED
    CLOSED = 1
    CONNECTING = 2
    CONNECTED = 3
    CLOSING = 4

    HEARTBEAT_PERIOD = 30
    HeartBeat = ouou.message.Request_5517

    current_client_id = 0

    def __init__(self, loop):
        self.cid = BaseClient.next_cid()
        self.state = BaseClient.CLOSED
        self._loop = loop
        self._sock = None
        self._message_callback = default_message_callback
        self._state_callback = default_state_callback
        self._waited_response = []
        self._waited_push = set()


    def send_data(self, data):
        raise NotImplementedError('do not use this class directly')


    def is_waiting_for_push(self):
        '''
        Could just return `_waited_push`. However we want to make sure
        a bool object is returned.
        '''
        return self._waited_push != set()


    def is_waiting_for_response(self):
        '''
        Could just return `_waited_response`. However we want to make sure
        a bool object is returned.
        '''
        return self._waited_response != []


    def is_waiting_for_msg(self):
        return any((self.is_waiting_for_push(), self.is_waiting_for_response()))


    def _add_waited_message(self, msg):
        self._waited_response.append(msg.header.msg_id)
        if msg.is_asked_for_push():
            self._waited_push.add(msg.header.msg_id)


    def send_message(self, *msgs):
        assert len(msgs)

        if len(msgs) > 1:
            multiple = ouou.message.MultipleMessage()
            for m in msgs:
                self._add_waited_message(m)
                multiple.add_message(m)
            self.send_data(multiple.tobytes())
        else:
            self._add_waited_message(msgs[0])
            self.send_data(msgs[0].tobytes())


    def handle_message(self, msg):
        msg_id = msg.header.msg_id
        waiting = msg_id in self._waited_response
        if not msg.is_push_msg() and msg_id != 5500:
            try:
                self._waited_response.remove(msg.header.msg_id)
            except ValueError:
                LOGGER.error('{} received unexpected message {}'.format(self, msg_id))
                LOGGER.error('{} waiting for {}'.format(self, self._waited_response))

        if msg_id != BaseClient.HeartBeat.MSG_ID or waiting:
            self._message_callback(self, msg)


    def _connect_check(self):
        if self.state == BaseClient.CONNECTED:
            raise RuntimeError('{} connected to {}'.format(
                       self, self._sock.getpeername())

                  )
        self.state = BaseClient.CONNECTING
        self._sock = socket.socket()


    def set_message_callback(self, callback):
        self._message_callback = callback or default_message_callback


    def set_state_callback(self, callback):
        self._state_callback = callback or default_state_callback


    def close(self):
        self.state = BaseClient.CLOSING
        self._close()


    def keep_alive(self):
        LOGGER.info('keep alive')
        msg = BaseClient.HeartBeat()
        msg.pid = 1
        self.send_message(msg)
        self._loop.call_later(self.HEARTBEAT_PERIOD, self.keep_alive)


    def change_state(self, new_state):
        self._state_callback(self, new_state)
        self.state = new_state
        if new_state == BaseClient.CONNECTED:
            self._loop.call_later(self.HEARTBEAT_PERIOD, self.keep_alive)


    def _close(self):
        self.change_state(BaseClient.CLOSED)
        self._sock.close()


    def __repr__(self):
        return 'Client<{}>'.format(self.cid)


    @staticmethod
    def next_cid():
        BaseClient.current_client_id += 1
        return BaseClient.current_client_id
