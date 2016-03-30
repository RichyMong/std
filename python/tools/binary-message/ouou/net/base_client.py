import asyncio
import socket
import errno
import logging
from datetime import datetime
import ouou

LOGGER = logging.getLogger("client")


def default_message_callback(c, data):
    LOGGER.debug('client[{}] receive data length: {}'.format(c.cid, len(data)))


def default_state_callback(c, state, passive = False):
    LOGGER.debug('client[{}] state changed to: {}'.format(c.cid, state))


class BaseClient(object):
    CLOSED = 1
    CONNECTING = 2
    CONNECTED = 3
    CLOSING = 4

    current_client_id = 0

    def __init__(self, loop):
        self.cid = BaseClient.next_cid()
        self.state = BaseClient.CLOSED
        self._loop = loop
        self._sock = None
        self._message_callback = default_message_callback
        self._state_callback = default_state_callback


    def _connect_check(self):
        if self.state == BaseClient.CONNECTED:
            raise RuntimeError('Client <{}> connected to {}'.format(
                       self.cid, self._sock.getpeername())
                  )
        self.state = BaseClient.CONNECTING
        self._sock = socket.socket()


    def set_message_callback(self, callback):
        self._message_callback = callback or default_message_callback


    def set_state_callback(self, callback):
        self._state_callback = callback or default_state_callback


    def close(self, passive = False):
        self._state_callback(self, BaseClient.CLOSED, passive)
        self.state = BaseClient.CLOSED
        self._sock.close()


    def __repr__(self):
        return 'Client <{}>:{}'.format(hex(id(self)), self.cid)


    @staticmethod
    def next_cid():
        BaseClient.current_client_id += 1
        return BaseClient.current_client_id


