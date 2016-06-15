import socket
import logging
from .. import message

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

    STATE = {
              CLOSED : 'CLOSED', CONNECTING : 'CONNECTING',
              CONNECTED : 'CONNECTED', CLOSING : 'CLOSING'
            }

    HEARTBEAT_PERIOD = 30
    HeartBeat = message.Request_5517

    current_client_id = 0

    def __init__(self, loop = None):
        self.cid = BaseClient.next_cid()
        self.state = BaseClient.CLOSED
        self.localaddr = None
        self._loop = loop
        self._sock = None
        self._message_callback = default_message_callback
        self._state_callback = default_state_callback
        self._waited_response = []
        self._waited_push = set()
        self._ka_handle = None
        self._buf = b''

    def connect(self, address):
        '''
        address should be a (host, port) tuple
        '''
        self._setup_connection()
        if self._loop:
            self.async_connect(address)
        else:
            self._sock.connect(address)

    def async_connect(self, host, port):
        raise NotImplementedError('')

    def send_data(self, data):
        if self._loop:
            self.async_send_data(data)
        else:
            self._sock.sendall(data)

    def recv_nbytes(self, n):
        data = b''
        while len(data) < n:
            data += self._sock.recv(n - len(data))
        return data

    def recv_message(self):
        if self._loop:
            raise RuntimeError('blocked in event loop')

        head = self.recv_nbytes(message.Header.type_size)
        if not len(head):
            self._close()
            return None
        header = message.Header.frombytes(head)
        body = self.recv_nbytes(header.body_size())
        return message.Message.frombytes(body, header)

    def send_and_receive(self, *msg):
        if len(msg) > 1:
            multiple = message.MultipleMessage()
            expected_cnt = 1
            for m in msg:
                multiple.add_message(m)
                expected_cnt += 1
            self.send_data(multiple.tobytes())
            resps = []
            while len(resps) < expected_cnt:
                resps.append(self.recv_message())
            return resps
        else:
            self._sock.sendall(msg[0].tobytes())
            return self.recv_message()

    def async_send_data(self, data):
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
            multiple = message.MultipleMessage()
            for m in msgs:
                LOGGER.debug('add sub message {} to multiple message'.format(m.msg_id))
                self._add_waited_message(m)
                multiple.add_message(m)
            self.send_data(multiple.tobytes())
        else:
            self._add_waited_message(msgs[0])
            self.send_data(msgs[0].tobytes())

    def handle_message(self, msg):
        msg_id = msg.msg_id
        waiting = msg_id in self._waited_response
        if not msg.is_push_msg() and waiting:
            try:
                self._waited_response.remove(msg.header.msg_id)
            except ValueError:
                LOGGER.error('{} received unexpected message {}'.format(self, msg_id))
                LOGGER.error('{} waiting for {}'.format(self, self._waited_response))

        if msg_id != self.HeartBeat.MSG_ID or waiting:
            self._message_callback(self, msg)

    def _handle_data(self, data):
        if not len(data):
            self._close()
        else:
            self._buf += data
            while True:
                p = message.Message.allfrombytes(self._buf)
                if p is None:
                    break
                self.handle_message(p)
                self._buf = self._buf[p.size():]

    def _setup_connection(self):
        if self.state == BaseClient.CONNECTED:
            raise RuntimeError('{} connected to {}'.format(
                       self, self._sock.getpeername())
                  )

        if self.state == BaseClient.CONNECTING:
            LOGGER.debug('{} connection in progress'.format(self))
            return

        self.state = BaseClient.CONNECTING
        self._sock = socket.socket()

    def set_message_callback(self, callback):
        self._message_callback = callback or default_message_callback

    def set_state_callback(self, callback):
        self._state_callback = callback or default_state_callback

    def close(self):
        if self.state in (BaseClient.CLOSING, BaseClient.CLOSED):
            LOGGER.debug('{}:close in {} state'.format(self,
                          self.STATE[self.state]))
            return

        self.state = BaseClient.CLOSING
        self._close()

    def send_heartbeat(self):
        msg = BaseClient.HeartBeat()
        msg.pid = 1
        self.send_data(msg.tobytes())

    def _change_state(self, new_state):
        if new_state == self.state:
            return

        LOGGER.debug('client<{}> state changed from [{}] to [{}]'.format(
            self.cid, BaseClient.STATE[self.state], BaseClient.STATE[new_state]
            ))

        self._state_callback(self, new_state)
        self.state = new_state
        if new_state == BaseClient.CONNECTED:
            self._keep_alive()
            self.localaddr = '{}:{}'.format(*self._sock.getsockname())

    def _keep_alive(self):
        if self.state != BaseClient.CONNECTED:
            return

        if self._loop:
            self._ka_handle = self._loop.call_later(self.HEARTBEAT_PERIOD, self._keep_alive)
        self.send_heartbeat()

    def _close(self):
        if self.state == BaseClient.CLOSED:
            return

        self._change_state(BaseClient.CLOSED)
        self._sock.close()
        self._sock = None
        self._waited_response = []
        self._waited_push = set()
        if self._ka_handle:
            self._ka_handle.cancel()
            self._ka_handle = None

    def __repr__(self):
        return 'Client<{}>'.format(self.cid)

    @staticmethod
    def next_cid():
        BaseClient.current_client_id += 1
        return BaseClient.current_client_id
