from .basetypes import *

__all__ = [ 'Reader' ]

class Reader(object):
    def __init__(self, blob, offset = 0):
        self._buf = blob
        self._offset = offset

    def buf(self):
        return self._buf[self._offset:]

    def offset(self):
        return self._offset

    def available(self):
        return len(self._buf) - self._offset

    def reset(self, buf):
        self._buf = buf
        self._offset = 0

    def read_bytes(self, size):
        v = self._buf[self._offset:self._offset+size]
        self._offset += size
        return v

    def read_type(self, cls, **kwargs):
        return cls.fromstream(self, **kwargs)

    def read_times(self, cls, times = 1, **kwargs):
        for i in range(times):
            yield self.read_type(cls, **kwargs)

    def read_byte(self):
        return self.read_type(Byte)

    def read_char(self):
        return self.read_type(Char)

    def read_short(self):
        return self.read_type(Short)

    def read_ushort(self):
        return self.read_type(UShort)

    def read_int(self):
        return self.read_type(Int)

    def read_uint(self):
        return self.read_type(UInt)

    def read_large_int(self):
        return self.read_type(LargeInt)

    def read_string(self):
        return self.read_type(String)

    def read_vector(self, cls):
        return self.read_type(cls)
