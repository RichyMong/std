import asyncio
import socket
from . import base_client
from ..message import Header, Message

LOGGER = base_client.LOGGER

class Client(base_client.BaseClient):
    def __init__(self, loop = None):
        super().__init__(loop)

    def async_connect(self, server):
        f = self._loop.sock_connect(self._sock, server)
        f.add_done_callback(self._connection_made)

    def async_send_data(self, data):
        f = self._loop.sock_sendall(self._sock, data)
        f.add_done_callback(self._data_sent)

    def _connection_made(self, of):
        try:
            of.result()
        except Exception as e:
            LOGGER.error('failed to connect: {}'.format(e))
            self._close()
        else:
            self._change_state(self.CONNECTED)

    def _data_sent(self, of = None):
        LOGGER.debug('data sent successfully')
        f = self._loop.sock_recv(self._sock, 65536)
        f.add_done_callback(self._data_received)

    def _data_received(self, of):
        try:
            data = of.result()
        except Exception as e:
            if self._sock:
                base_client.LOGGER.error('{} data receive exception: {}'.format(
                    self._sock.getsockname(), e))
            self._close()
            return

        self._handle_data(data)

        if self.is_waiting_for_msg():
            f = self._loop.sock_recv(self._sock, 65536)
            f.add_done_callback(self._data_received)
