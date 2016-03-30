import asyncio
import socket
from . import base_client
from ouou.message import Header, Message

class Client(base_client.BaseClient):
    def __init__(self, loop):
        super().__init__(loop)
        self._recv_header = None

    def connect(self, host, port):
        super()._connect_check()

        f = self._loop.sock_connect(self._sock, (host, port))
        f.add_done_callback(self._connection_made)


    def send_data(self, data):
        f = self._loop.sock_sendall(self._sock, data)
        f.add_done_callback(self._data_sent)


    def _connection_made(self, of):
        try:
            r = of.result()
        except Exception as e:
            base_client.LOGGER.error('connection failed {}'.format(e))
            self.close(passive = True)
        else:
            self._state_callback(self, self.CONNECTED, True)
            self.state = self.CONNECTED


    def _data_sent(self, of = None):
        f = self._loop.sock_recv(self._sock, Header.type_size)
        f.add_done_callback(self._data_received)


    def _data_received(self, of):
        try:
            data = of.result()
        except Exception as e:
            base_client.LOGGER.error('{} data receive exception: {}'.format(
                 self._sock.getsockname(), e))
            self.close(passive = True)
            return

        if not len(data):
            self.close(passive = True)
        elif not self._recv_header:
            assert len(data) == Header.type_size

            self._recv_header = Header.frombytes(data, c2s = False)
            f = self._loop.sock_recv(self._sock, self._recv_header.msg_size)
            f.add_done_callback(self._data_received)
        else:
            assert len(data) == self._recv_header.msg_size

            p = Message.frombytes(data, self._recv_header, c2s = False)
            self._message_callback(self, p)
            self._recv_header = None
