import os
import socket
import errno
from . import base_client
from ..message import Header, Message

LOGGER = base_client.LOGGER

class Client(base_client.BaseClient):
    def __init__(self, loop = None):
        super().__init__(loop)
        self._buf = b''

    def async_connect(self, addr):
        self._sock.setblocking(False)
        ec = self._sock.connect_ex(addr)
        if ec == errno.EINPROGRESS:
            self._loop.add_writer(self._sock.fileno(), self._connection_made)
        else:
            LOGGER.error('connect to {}: {}'.format(addr, os.strerror(ec)))

    def set_loop(self, new_loop):
        if not self._sock:
            return

        if not self._loop:
            if self.state == self.CONNECTING:
                self._loop.remove_writer(self._sock.fileno())
            if self.state == self.CONNECTED:
                self._loop.remove_reader(self._sock.fileno())

        if new_loop:
            self._sock.setblocking(False)
            if self.state == self.CONNECTING:
                new_loop.add_writer(self._sock.fileno())
            if self.state == self.CONNECTED:
                new_loop.add_reader(self._sock.fileno())
        else:
            self._sock.setblocking(True)
        self._loop = new_loop

    def async_send_data(self, data):
        self._sock.sendall(data)

    def _close(self):
        if self.state == self.CONNECTED:
            self._loop.remove_reader(self._sock.fileno())
        super()._close()

    def _data_ready(self):
        try:
            data = self._sock.recv(65536)
        except socket.error as e:
            if e.args[0] not in (errno.EWOULDBLOCK, errno.EAGAIN):
                raise
        else:
            if not len(data):
                self._close()
            else:
                self._buf += data
                while True:
                    p = Message.allfrombytes(self._buf)
                    if not p:
                        break
                    self.handle_message(p)
                    self._buf = self._buf[p.size():]

    def _connection_made(self):
        self._loop.remove_writer(self._sock.fileno())

        e = self._sock.getsockopt(socket.SOL_SOCKET, socket.SO_ERROR)
        if not e:
            self.change_state(self.CONNECTED)
            self._loop.add_reader(self._sock.fileno(), self._data_ready)
        else:
            LOGGER.error('connection: {}'.format(os.strerror(e)))
            self._close()
