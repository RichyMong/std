import socket
import errno
from . import base_client
from ouou.message import Header, Message

class Client(base_client.BaseClient):
    def __init__(self, loop):
        super().__init__(loop)
        self._buf = b''

    def connect(self, host, port):
        super()._connect_check()
        self._sock.setblocking(False)
        addr = (host, port)
        ec = self._sock.connect_ex(addr)
        if ec == errno.EINPROGRESS:
            self._loop.add_writer(self._sock.fileno(), self._connection_made)
        else:
            base_client.LOGGER.error('connection to {} error {}'.format(addr, ec))


    def send_data(self, data):
        self._sock.sendall(data)


    def close(self, passive = False):
        if self.state == self.CONNECTED:
            self._loop.remove_reader(self._sock.fileno())
        super().close(passive)


    def _data_ready(self):
        try:
            data = self._sock.recv(65536)
        except socket.error as e:
            if e.args[0] not in (errno.EWOULDBLOCK, errno.EAGAIN):
                raise
            print('data receive exception', e, self._sock.getsockname())
            self.close(True)
        else:
            if not len(data):
                self.close(True)
            else:
                self._buf += data
                p = Message.allfrombytes(self._buf, c2s = False)
                if p:
                    self._message_callback(self, p)
                    self._buf = self._buf[p.size():]


    def _connection_made(self):
        self._loop.remove_writer(self._sock.fileno())

        e = self._sock.getsockopt(socket.SOL_SOCKET, socket.SO_ERROR)
        if not e:
            self._state_callback(self, self.CONNECTED, True)
            self.state = self.CONNECTED
            self._loop.add_reader(self._sock.fileno(), self._data_ready)
        else:
            print('connection error:', e)
            self.close(True)
