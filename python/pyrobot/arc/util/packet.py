import struct
import tea
from const import *

class OutPacket(object):
    def __init__(self, mid, flag = 0):
        self.flag     = flag
        self.mid      = mid
        self.data_len = 0
        self.data_    = b''


    def encrypt(self, key):
        if self.data_len:
            self.data_ = tea.encode(self.data_, key)
            self.data_len = len(self.data_)
            self.flag |= PKT_FLAG_ENCRYPT


    def append_raw(self, *args):
        for data in args:
            data_len = len(data)
            self.data_ += struct.pack('{}s'.format(data_len), data)
            self.data_len += data_len


    def append_bytes(self, *args):
        for data in args:
            data_len = len(data)
            self.data_ += struct.pack('H{}s'.format(data_len), data_len, data)
            self.data_len += data_len + 2


    def append_uint32(self, value):
        self.data_ += struct.pack('I', value)
        self.data_len += 4


    def append_uint16(self, value):
        self.data_ += struct.pack('H', value)
        self.data_len += 2


    @property
    def data(self):
        return self.data_


    @property
    def buf(self):
        return struct.pack(PKT_HDR_FMT, PKT_MAGIC, self.flag, self.data_len, self.mid, 0) + self.data


class InPacket(object):
    '''
    Parse the packet as the Arc Client does.
    We don't add any boundary check since it's only used by us...
    '''
    def __init__(self, buf):
        pkt_len = len(buf)
        if pkt_len < PKT_HDR_LEN:
            raise ValueError("buf length {} is smaller than header length".format(pkt_len))

        _, _, data_len, mid_, _ = struct.unpack(PKT_HDR_FMT, buf[0:PKT_HDR_LEN])
        if pkt_len < data_len + PKT_HDR_LEN:
            raise ValueError("Invalid data length, pkt_len {}, data_len {}".format(pkt_len, data_len))

        self.mid_    = mid_
        self.data_   = struct.unpack_from('{}s'.format(data_len), buf, PKT_HDR_LEN)[0]
        self.offset_ = 0


    def decrypt(self, key):
        if key and len(self.data_):
            self.data_ = tea.decode(bytearray(self.data_), key)


    @property
    def mid(self):
        return self.mid_


    @property
    def data(self):
        return self.data_


    def extract_string(self):
        length = struct.unpack_from('H', self.data_, self.offset_)
        self.offset_ += 2

        data = struct.unpack_from('{}s'.format(length[0]), self.data_, self.offset_)
        self.offset_ += length[0]

        return data[0]


    def extract_strings(self, count):
        for i in range(count):
            yield self.extract_string()


    def extract_char(self):
        c = struct.unpack_from('b', self.data, self.offset_)[0]
        self.offset_ += 1
        return c


    def extract_chars(self, count):
        for i in range(count):
            yield self.extract_char()


    def extract_short(self):
        c = struct.unpack_from('H', self.data, self.offset_)[0]
        self.offset_ += 2
        return c

    def extract_shorts(self, count):
        for i in range(count):
            yield self.extract_short()


class PacketCombiner(object):
    def __init__(self, buf = ''):
        self._buf    = buf
        self._remain = len(buf)


    def append(self, buf):
        self._buf    += buf
        self._remain += len(buf)


    def __iter__(self):
        return self


    def next(self):
        if self._remain < PKT_HDR_LEN:
            raise StopIteration

        _, _, data_len, mid_, _ = struct.unpack(PKT_HDR_FMT, self._buf[:PKT_HDR_LEN])

        pkt_len = data_len + PKT_HDR_LEN
        if self._remain < pkt_len:
            raise StopIteration

        pkt = InPacket(self._buf[:pkt_len])

        self._buf     = self._buf[pkt_len:]
        self._remain -= pkt_len

        return pkt
