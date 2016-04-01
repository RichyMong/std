from emoney.util.etypes import *

class Reader(object):
    def __init__(self, blob, offset = 0):
        self._buf = blob
        self._offset = offset


    def read_bytes(self, size):
        v = self.data[:size]
        self._offset += size
        return v

    def read_etype(self, cls, **kwargs):
        v = cls.frombytes(self._buf, self._offset, **kwargs)
        self._offset += v.size()
        return v


    def read_byte(self):
        return self.read_etype(Byte)


    def read_char(self):
        return self.read_etype(Char)


    def read_short(self):
        return self.read_etype(Short)


    def read_ushort(self):
        return self.read_etype(UShort)


    def read_int(self):
        return self.read_etype(UInt)


    def read_uint(self):
        return self.read_etype(UInt)


    def read_large_int(self):
        return self.read_etype(LargeInt)


    def read_string(self, encoding = 'utf-8'):
        return self.read_etype(String, encoding = encoding)


    def read_vector(self, cls):
        return self.read_etype(cls)



class Writer(object):
    def __init__(self, msg_type, **kwargs):
        '''
        kwargs are as the following:
            prefix - '{' or ':', DEFAULT '{'
            require_compress - True or False, DEFAULT False
            compress - True or False, DEFAULT False
        '''
        self.buf = b''
        self.write_char(kwargs.get('prefix', '{'))
        self.write_short(msg_type)
        self.compress = kwargs.get('compress', False)
        flag = 0
        if kwargs.get('require_compress', False):
            flag |= PKG_FLAG_REQ_COMPRESS
        self.write_byte(0)
        self.write_byte(flag)
        self.write_ushort(0)


    def write_bytes(self, b):
        self.buf += b


    def write_ctype(self, fmt, v):
        size_tbl = { 'B' : 1, 'C' : 1, 'H' : 2, 'I' : 4, 'L' : 8}

        size = size_tbl[fmt.upper()]

        self.buf += struct.pack('<' + fmt, v)


    def write_byte(self, v):
        self.write_ctype('B', v)


    def write_char(self, v):
        self.write_byte(ord(v))


    def write_short(self, v):
        self.write_ctype('h', v)


    def write_ushort(self, v):
        self.write_ctype('H', v)


    def write_int(self, v):
        self.write_ctype('i', v)


    def write_uint(self, v):
        self.write_ctype('I', v)


    def write_str(self, s, encoding = 'utf-8'):
        self.write_short(len(s))
        self.write_bytes(s.encode(encoding))


    def write_barray(self, v):
        self.write_byte(len(v))
        for b in v:
            self.write_byte(b)


    def write_sarray(self, v):
        self.write_byte(v)
        for s in v:
            self.write_short(s)


    def send(self, sock):
        self.buf = self.buf[:5] + struct.pack('<H', len(self.buf) - 7) + self.buf[7:]
        if self.compress:
            self.buf = self.buf[:7] + zlib.compress(self.buf[7:])
            self.compress = False
        self.write_char('}')

        sock.sendall(self.buf)

