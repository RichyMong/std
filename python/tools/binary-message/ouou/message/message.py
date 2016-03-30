import struct
import zlib
from emoney.util.const import *
from emoney.util.etypes import *

request_messages = {}
response_messages = {}
push_messages = {}

__all__ = [ 'Header', 'Message' ]

class BinaryObjectMeta(type):
    def __new__(mcs, name, bases, attrs):
        cls = type.__new__(mcs, name, bases, attrs)

        cls._fields = { x[0] : x[1] for x in attrs.get('fields_info') }

        cls._extra_fields = { x[0] : x[1] for x in attrs.get('extra_fields_info', []) }

        return cls


class BinaryObject(metaclass = BinaryObjectMeta):
    '''
    The elements of the fields_info are (name, format) tuples.
    The format is almost the same as descirbed in the "struct" module with some
    exception:
        1. '*NF' denotes an array field, where N is the format of the type used
           to denote the size and F is the format of the elements' type.
        2. '' (an empty string) denotes the field is a BinaryObject.
    The format will be used to calculate the offset of the following fields.
    The offset is correct until an array filed appears and after that the
    offset becomes incorrect.
    '''
    fields_info = ()

    def __init__(self, **kwargs):
        for name in self._fields:
            if not hasattr(self, name):
                setattr(self, name, kwargs.get(name, None))


    def __setattr__(self, name, value):
        if name in self._fields or name in self._extra_fields:
            d = self._fields if name in self._fields else self._extra_fields
            if value is not None and not isinstance(value, d[name]):
                value = d[name](value)
            super().__setattr__(name, value)
        else:
            super().__setattr__(name, value)


    def tobytes(self):
        b = b''
        for (field, field_type, *_) in self.fields_info:
            v = getattr(self, field)
            if v is None:
                raise ValueError('{}.{} is not set'.format(
                        type(self).__name__, field)
                      )
            b += v.tobytes()
        return b


    def fieldsfrombytes(self, buf, offset = 0):
        for (field, field_type, *_) in self.fields_info:
            value = field_type.frombytes(buf, offset)
            offset += value.size()
            setattr(self, field, value)


    @classmethod
    def _frombytes(cls, buf, offset = 0, **kwargs):
        p = cls(**kwargs)
        p.fieldsfrombytes(buf, offset)
        return p


class Header(BinaryObject):
    fields_info = (
                      ('prefix', Char),
                      ('msg_id', UShort),
                      ('flag', Byte),
                      ('unused', Byte),
                      ('msg_size', UShort),
                  )

    type_size = sum(x[1].type_size for x in fields_info)

    def __init__(self, **kwargs):
        self.prefix = '{'
        self.flag = 0
        self.unused = 0
        self.msg_size = 0
        super().__init__(**kwargs)
        self.c2s = kwargs.get('c2s', True)


    def __repr__(self):
        attr = (self.flag << 8) & self.unused
        return "prefix: '{}', type: {}, attribute: 0x{:x}, size: {}".format(
                    self.prefix, self.msg_id, attr, self.msg_size
                )


    def size(self):
        return self.type_size


    def is_pushed_pkg(self):
        return self.flag & PKG_FLAG_PUSH == PKG_FLAG_PUSH

    def set_need_compress(self):
        self.flag |= PKG_FLAG_REQ_COMPRESS

    def set_compressed(self):
        self.flag |= PKG_FLAG_COMPRESSED

    def need_compress(self):
        return self.flag & PKG_FLAG_REQ_COMPRESS == PKG_FLAG_REQ_COMPRESS


    def is_compressed(self):
        return ((not self.c2s and self.flag & PKG_FLAG_COMPRESSED) or
                (self.c2s and self.need_compress()))


    @classmethod
    def frombytes(cls, buf, offset = 0, **kwargs):
        return super()._frombytes(buf, offset, **kwargs)


class Message(BinaryObject):
    fields_info = ()

    msg_id = 0

    def __init__(self, **kwargs):
        if 'header' in kwargs:
            self.header = kwargs.pop('header')
        else:
            self.header = Header()
            self.header.msg_id = self.msg_id
        super().__init__(**kwargs)


    def __setattr__(self, name, value):
        if name == 'header':
            self.__dict__['header'] = value
        else:
            super().__setattr__(name, value)


    def size(self):
        return self.header.size() + self.header.msg_size


    @staticmethod
    def is_complete(buf):
        if len(buf) >= Header.type_size:
            h = Header.frombytes(buf)
            if len(buf) >= h.size() + h.msg_size:
                return True
        return False


    @classmethod
    def allfrombytes(cls, buf, c2s = True):
        if not cls.is_complete(buf):
            return None

        header = Header.frombytes(buf, c2s = c2s)
        data = buf[header.type_size:header.type_size+header.msg_size]

        return Message.frombytes(data, header, c2s)


    @staticmethod
    def frombytes(buf, header, c2s = True):
        if not header.msg_id:
            return Message(header = header)

        if header.is_compressed():
            buf = zlib.decompress(buf)

        if c2s:
            return request_messages[header.msg_id]._frombytes(buf, header = header)
        elif header.is_pushed_pkg():
            return push_messages[header.msg_id]._frombytes(buf, header = header)
        else:
            return response_messages[header.msg_id]._frombytes(buf, header = header)


    def tobytes(self, compress = False):
        body = super().tobytes()
        if compress:
            self.header.set_compressed()
            self.header.set_need_compress()
            body = zlib.compress(body)
        self.header.msg_size = len(body)
        return self.header.tobytes() + body + Char('}').tobytes()


    def __repr__(self):
        r = type(self).__name__
        for (name, _, desc) in self.fields_info:
            r += '\n\t{}: {}'.format(desc, getattr(self, name))
        return r
