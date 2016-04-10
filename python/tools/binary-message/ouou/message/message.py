import struct
import zlib
import re
import collections
from .const import *
from ouou.util.basetypes import Byte, Char, UShort
from ouou.util.stream import Reader

request_messages = {}
response_messages = {}
push_messages = {}

__all__ = [ 'Header', 'Message' ]

NamedField = collections.namedtuple('NamedField', ['name', 'type', 'desc'])

class DependentAttribute(object):
    def tobytes(self):
        raise NotImplementedError('')

    @classmethod
    def fromstream(cls, reader, target, **kwargs):
        raise NotImplementedError('')

class DependentFieldsAttribute(DependentAttribute):
    def tobytes(self, target):
        raise NotImplementedError('')

class BinaryObjectMeta(type):
    def __new__(mcs, name, bases, attrs):
        cls = type.__new__(mcs, name, bases, attrs)

        cls._fields = { x[0] : x[1] for x in attrs.get('attributes_info', []) }

        return cls

class BinaryObject(metaclass = BinaryObjectMeta):
    '''
    '''
    attributes_info = ()

    def __init__(self, **kwargs):
        for name in self._fields:
            if not hasattr(self, name):
                setattr(self, name, kwargs.get(name, None))

    def __setattr__(self, name, value):
        if name in self._fields or name in self._optional_fields:
            d = self._fields if name in self._fields else self._optional_fields
            if value is not None and not isinstance(value, d[name]):
                value = d[name](value)
        super().__setattr__(name, value)

    def __repr__(self):
        r = ''
        display_prefix = '\n\t'

        for (name, _, desc) in self.attributes_info:
            r += display_prefix + '{}: {}'.format(desc, getattr(self, name))

        if not self.attributes_info:
            return 'Empty ' + r

        return r

    def _tobytes(self):
        b = b''
        for (field, field_type, *_) in self.attributes_info:
            v = getattr(self, field)
            if v is None:
                raise ValueError('{}.{} is not set'.format(
                        type(self).__name__, field)
                      )
            if isinstance(v, DependentFieldsAttribute):
                b += v.tobytes(self)
            else:
                b += v.tobytes()

        name = 'optional_fields'
        for info in getattr(self, name, []):
            for v in info:
                b += v.tobytes()

        return b

    def tobytes(self):
        return self._tobytes()

    def size(self):
        total_size = 0
        for (field, field_type, *_) in self.attributes_info:
            total_size += getattr(self, field).size()

        return total_size

    @classmethod
    def _fromstream(cls, reader, **kwargs):
        self = cls(**kwargs)
        for (field, field_type, *_) in self.attributes_info:
            if issubclass(field_type, DependentAttribute):
                dattr = field_type.fromstream(reader, self)
                if dattr:
                    setattr(self, field, dattr)
            else:
                setattr(self, field, reader.read_type(field_type))

        return self

    @classmethod
    def fromstream(cls, reader, **kwargs):
        return cls._fromstream(reader, **kwargs)

    @classmethod
    def frombytes(cls, buf, **kwargs):
        return cls.fromstream(Reader(buf), **kwargs)

def create_class(name, fields):
    def __init__(self, args = ()):
        for i, v in enumerate(args):
            setattr(self, fields[i].name, v)

    cls = BinaryObjectMeta.__new__(BinaryObjectMeta, name, (BinaryObject, ),
                           {'attributes_info':fields})
    cls.__init__ = __init__
    return cls

class Header(BinaryObject):
    attributes_info = (
        NamedField('prefix', Char, 'prefix'),
        NamedField('msg_id', UShort, 'msg_id'),
        NamedField('flag', Byte, 'flag'),
        NamedField('unused', Byte, 'unused'),
        NamedField('msg_size', UShort, 'msg_size'),
    )

    type_size = sum(x[1].type_size for x in attributes_info)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.prefix = '{'
        self.flag = 0
        self.msg_id = kwargs.get('msg_id', 0)
        self.unused = 0
        self.msg_size = 0
        self.c2s = kwargs.get('c2s', True)

    def __repr__(self):
        attr = (self.flag << 8) & self.unused
        return "prefix: '{}', type: {}, attribute: 0x{:x}, size: {}".format(
                    self.prefix, self.msg_id, attr, self.msg_size
                )

    def size(self):
        return self.type_size

    def is_push_pkg(self):
        return self.flag & PKG_FLAG_PUSH == PKG_FLAG_PUSH

    def set_push(self):
        self.flag |= PKG_FLAG_PUSH

    def set_need_compress(self):
        self.flag |= PKG_FLAG_REQ_COMPRESS

    def set_compressed(self):
        self.flag |= PKG_FLAG_COMPRESSED

    def need_compress(self):
        return self.flag & PKG_FLAG_REQ_COMPRESS == PKG_FLAG_REQ_COMPRESS

    def is_compressed(self):
        return ((not self.c2s and self.flag & PKG_FLAG_COMPRESSED) or
                (self.c2s and self.need_compress()))

class Message(BinaryObject):
    msg_id = 0

    def __init__(self, **kwargs):
        self.header = kwargs.pop('header', Header(msg_id = self.msg_id))
        super().__init__(**kwargs)

    def size(self):
        return self.header.size() + self.header.msg_size

    def is_push_pkg(self):
        return self.header.is_push_pkg()

    def is_asked_for_push(self):
        return (self.header.msg_id > 5510 and getattr(self, 'push_type', None) == 1)

    def is_multiple_message(self):
        return self.header.msg_id == 5500

    def tobytes(self, compress = False):
        body = self._tobytes()
        if compress:
            self.header.set_compressed()
            self.header.set_need_compress()
            cbody = zlib.compress(body)
            body = Short(len(body)).tobytes() + Short(len(cbody)).tobytes() + cbody
        self.header.msg_size = len(body)
        return self.header.tobytes() + body + Char('}').tobytes()

    def __repr__(self):
        return type(self).__name__ + super().__repr__()

    @staticmethod
    def fromstream(reader, header, c2s = True):
        '''
        Parse a message from the specified buffer. The buffer must be at least
        header.msg_size bytes.
        '''
        if not header.msg_id:
            return Message(header = header)

        if header.is_compressed():
            orig_len = reader.read_short()
            cmp_len = reader.read_short()
            buf = zlib.decompress(reader.buf())
            assert len(buf) == orig_len
            reader.reset(buf)

        if c2s:
            return request_messages[header.msg_id]._fromstream(reader, header = header)
        elif header.is_push_pkg():
            return push_messages[header.msg_id]._fromstream(reader, header = header)
        else:
            return response_messages[header.msg_id]._fromstream(reader, header = header)

    @staticmethod
    def allfromstream(reader, c2s = True):
        n = reader.available()
        if n < Header.type_size:
            return None

        h = Header.fromstream(reader, c2s = c2s)
        if n < Header.type_size + h.msg_size + 1:
            return None

        if h.msg_size:
            return Message.fromstream(reader, h, c2s = c2s)
        else:
            return Message(header = h)

    @staticmethod
    def frombytes(buf, header, c2s = True):
        '''
        Parse a message from the specified buffer. The buffer must be at least
        header.msg_size bytes.
        '''
        return Message.fromstream(header, Reader(buf), c2s = c2s)

    @staticmethod
    def allfrombytes(buf, c2s = True):
        return Message.allfromstream(Reader(buf), c2s = c2s)

class MessageMeta(BinaryObjectMeta):
    '''
    This meta class will get message id from the class name. If the name is
    not it the format 'xxx_1234' or 'xxx1234' where 1234 is the msg_id,
    the msg_id field must be defined explicitly in the class.
    '''
    name_id_pattern = re.compile(r'[^\d]+(\d+)')

    def __new__(mcs, name, bases, attrs):
        cls = super().__new__(mcs, name, bases, attrs)

        match = MessageMeta.name_id_pattern.match(name)
        if match:
            cls.msg_id = int(match.group(1))

        return cls
