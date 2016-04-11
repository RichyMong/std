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

Attribute = collections.namedtuple('Attribute', ['name', 'type', 'desc'])

class DependentAttribute(object):
    '''
    This class of attributes depend on the other attributes in the same target
    class. As a result, we need the host object when creating an instance of
    this class from a stream.
    '''
    def tobytes(self):
        raise NotImplementedError('')

    @classmethod
    def fromstream(cls, reader, target, **kwargs):
        raise NotImplementedError('')

class DependentFieldsAttribute(DependentAttribute):
    '''
    This class of attributes is like DependentAttribute. However, it needs
    more information. We are not able to write it without the target.
    '''
    def tobytes(self, target):
        raise NotImplementedError('')

class BinaryObjectMeta(type):
    def __new__(mcs, name, bases, attrs):
        cls = type.__new__(mcs, name, bases, attrs)

        cls._fields = {}

        for x in attrs.get('attributes_info', []):
            if x in cls._fields:
                raise RuntimeError('duplicate attribute {}'.format(x.name))
            cls._fields[x.name] = x.type

        return cls

    def _fromstream(cls, reader, **kwargs):
        self = cls(**kwargs)
        for (field, field_type, *_) in self.attributes_info:
            if issubclass(field_type, DependentAttribute):
                dattr = field_type.fromstream(reader, self)
                if dattr:
                    setattr(self, field, dattr)
            else:
                setattr(self, field, reader.read_type(field_type))

        self.extra_parse(reader)

        return self

    def fromstream(cls, reader, **kwargs):
        return cls._fromstream(reader, **kwargs)

    def frombytes(cls, buf, **kwargs):
        return cls.fromstream(Reader(buf), **kwargs)

class BinaryObject(metaclass = BinaryObjectMeta):
    '''
    '''
    attributes_info = ()

    def __init__(self, iterable = None, **kwargs):
        '''
        The object could be initialized from an iterable object as well as
        keyword arguments.
        '''
        if iterable:
            for (i, arg) in enumerate(iterable):
                attr = self.attributes_info[i]
                setattr(self, attr.name, arg)

        for name, cls in self._fields.items():
            if not hasattr(self, name):
                setattr(self, name, kwargs.get(name, cls()))

    def __str__(self):
        r = ''

        for i, (name, _, desc) in enumerate(self.attributes_info):
            if i: r += PRINT_PREFIX

            if desc:
                r += '{}: {}'.format(desc, getattr(self, name))
            else:
                r += '{}'.format(getattr(self, name))

        if not self.attributes_info:
            return 'Empty ' + r

        return r

    def _tobytes(self):
        b = b''
        cls_name = type(self).__name__
        for (field, field_type, *_) in self.attributes_info:
            v = getattr(self, field)
            if v is None:
                raise ValueError('{}.{} is not set'.format( cls_name, field))
            try:
                if not isinstance(v, field_type):
                    v = field_type(v)

                if issubclass(field_type, DependentFieldsAttribute):
                    b += v.tobytes(self)
                else:
                    b += v.tobytes()
            except Exception as e:
                raise RuntimeError("attribue '{}' in class {}: {}".format(
                       field, cls_name, e))

        return b

    def tobytes(self):
        return self._tobytes()

    def size(self):
        total_size = 0
        for (field, field_type, *_) in self.attributes_info:
            total_size += getattr(self, field).size()

        return total_size

    def extra_parse(self, reader):
        pass

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
        Attribute('prefix', Char, 'prefix'),
        Attribute('msg_id', UShort, 'msg_id'),
        Attribute('flag', Byte, 'flag'),
        Attribute('unused', Byte, 'unused'),
        Attribute('msg_size', UShort, 'msg_size'),
    )

    type_size = sum(x[1].type_size for x in attributes_info)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.prefix = '{'
        self.msg_id = kwargs.get('msg_id', 0)
        self.c2s = kwargs.get('c2s', True)

    def __str__(self):
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

    def empty(self):
        return not self.header.msg_size

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

    def __str__(self):
        return type(self).__name__ + PRINT_PREFIX + super().__str__()

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
