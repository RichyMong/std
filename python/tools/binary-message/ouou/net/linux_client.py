import socket
import errno
from . import base_client
from emoney.message import Header, Message

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
            self._close()
        else:
            if not len(data):
                self._close()
            else:
                self._buf += data
                p = Message.allfrombytes(self._buf, c2s = False)
                if p:
                    self.handle_message(p)
                    self._buf = self._buf[p.size()+1:]


    def _connection_made(self):
        self._loop.remove_writer(self._sock.fileno())

        e = self._sock.getsockopt(socket.SOL_SOCKET, socket.SO_ERROR)
        if not e:
            self.change_state(self.CONNECTED)
            self._loop.add_reader(self._sock.fileno(), self._data_ready)
        else:
            base_client.LOGGER.error('connection error: {}'.format(e))
            self._close()
