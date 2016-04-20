import asyncio
import socket
from . import base_client
from ..message import Header, Message

LOGGER = base_client.LOGGER

class Client(base_client.BaseClient):
    def __init__(self, loop = None):
        super().__init__(loop)
        self._recv_header = None

    def async_connect(self, server):
        f = self._loop.sock_connect(self._sock, server)
        f.add_done_callback(self._connection_made)

    def async_send_data(self, data):
        f = self._loop.sock_sendall(self._sock, data)
        f.add_done_callback(self._data_sent)

    def _connection_made(self, of):
        try:
            r = of.result()
        except Exception as e:
            LOGGER.error('failed to connect: {}'.format(e))
            self._close()
        else:
            self._change_state(self.CONNECTED)

    def _data_sent(self, of = None):
        f = self._loop.sock_recv(self._sock, Header.type_size)
        f.add_done_callback(self._data_received)

    def _data_received(self, of):
        try:
            data = of.result()
        except Exception as e:
            base_client.LOGGER.error('{} data receive exception: {}'.format(
                 self._sock.getsockname(), e))
            self._close()
            return

        if not len(data):
            self._close()
        elif not self._recv_header:
            self._recv_header = Header.frombytes(data)
            f = self._loop.sock_recv(self._sock, self._recv_header.body_size())
            f.add_done_callback(self._data_received)
        else:
            p = Message.frombytes(data, self._recv_header)
            self.handle_message(p)
            self._recv_header = None

            if self.is_waiting_for_msg():
                f = self._loop.sock_recv(self._sock, Header.type_size)
                f.add_done_callback(self._data_received)
