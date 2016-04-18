import struct
import zlib
import re
import collections
from .const import *
from ..util.basetypes import Byte, Char, UShort
from ..util.stream import Reader

request_messages = {}
response_messages = {}
push_messages = {}

__all__ = [ 'Header', 'Message', 'MultipleMessage' ]

Attribute = collections.namedtuple('Attribute', ['name', 'type', 'desc'])

OptionalAttribute = collections.namedtuple('Attribute',
                     ['name', 'type', 'desc', 'condition'])

class ReadDepAttr(object):
    @classmethod
    def fromstream(cls, reader, **kwargs):
        raise NotImplementedError('')

class WriteDepAttr(object):
    def tobytes(self, owner):
        raise NotImplementedError('')

class DepAttr(ReadDepAttr, WriteDepAttr):
    '''
    Actually, if how to write the attribute depends on the owner, it's absolute
    that the read also depends on the stream.
    '''

class ParseError(Exception):
    ''''''

class BinaryObjectMeta(type):
    def _fromstream(cls, reader, **kwargs):
        self = cls(**kwargs)
        for attr in self.attributes_info:
            field, field_type, *extra = attr
            if isinstance(attr, OptionalAttribute) and not extra[1](self):
                continue

            if issubclass(field_type, (ReadDepAttr, BinaryObject)):
                dattr = field_type.fromstream(reader, owner = self)
                if dattr is not None:
                    setattr(self, field, dattr)
            else:
                try:
                    setattr(self, field, reader.read_type(field_type))
                except Exception as e:
                    raise ParseError('{} - attribute: "{}", error: {}'.format(
                             cls.__name__, field, e))

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
        keyword arguments. kwargs may contain 'owner' to specify which object
        owns this object.
        '''
        if iterable:
            for (i, arg) in enumerate(iterable):
                attr = self.attributes_info[i]
                setattr(self, attr.name, arg)
        self.owner = kwargs.get('owner', None)

        for attr in self.attributes_info:
            if isinstance(attr, Attribute) and not hasattr(self, attr.name):
                if issubclass(attr.type, BinaryObject):
                    default = attr.type(owner = self)
                else:
                    default = attr.type()
                setattr(self, attr.name, kwargs.get(attr.name, default))

    @property
    def attributes(self):
        for attr in self.attributes_info:
            if not isinstance(attr, OptionalAttribute) or attr.condition(self):
                yield attr

    def __iter__(self):
        for attr in self.attributes:
            yield getattr(self, attr.name)

    def __eq__(self, other):
        if isinstance(other, collections.Iterable):
            return tuple(self) == tuple(other)
        else:
            return False

    def __len__(self):
        return len(self.attributes_info)

    def __str__(self):
        r = ''

        for i, attr in enumerate(self.attributes):
            if i: r += PRINT_PREFIX
            if attr.desc: r += '{}: '.format(attr.desc)
            r += '{}'.format(getattr(self, attr.name))

        return r or 'Empty'

    def body2bytes(self):
        b = b''
        cls_name = type(self).__name__
        for attr in self.attributes:
            v = getattr(self, attr.name)
            if v is None:
                raise ValueError('{}.{} is not set'.format(cls_name, attr.name))
            try:
                if not isinstance(v, attr.type):
                    v = attr.type(v)

                if issubclass(attr.type, WriteDepAttr):
                    b += v.tobytes(self)
                else:
                    b += v.tobytes()
            except Exception as e:
                raise RuntimeError("attribue '{}' in class {}: {}".format(
                       attr.name, cls_name, e))
        return b

    def tobytes(self):
        return self.body2bytes()

    def size(self):
        total_size = 0
        for x in self:
            total_size += x.size()

        return total_size

    def extra_parse(self, reader):
        pass

def create_class(name, attributes):
    return  BinaryObjectMeta.__new__(
                BinaryObjectMeta,
                name,
                (BinaryObject, ),
                { 'attributes_info' : attributes }
            )

class Header(BinaryObject):
    attributes_info = (
        Attribute('prefix', Char, 'prefix'),
        Attribute('msg_id', UShort, 'msg_id'),
        Attribute('flag', Byte, 'flag'),
        Attribute('unused', Byte, 'unused'),
        Attribute('payload_size', UShort, 'payload_size'),
    )

    type_size = sum(x[1].type_size for x in attributes_info)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.prefix = '{'
        self.msg_id = kwargs.get('msg_id', 0)
        self.c2s = kwargs.get('c2s', False)

    def __str__(self):
        attr = (self.flag << 8) & self.unused
        return "prefix: '{}', type: {}, attribute: 0x{:x}, size: {}".format(
                    self.prefix, self.msg_id, attr, self.payload_size
                )

    def body_size(self):
        return self.payload_size + len(MSG_TAIL_BYTES)

    def size(self):
        return self.type_size

    def is_sub_msg(self):
        return self.flag & MSG_FLAG_SUB == MSG_FLAG_SUB

    def is_push_msg(self):
        return self.flag & MSG_FLAG_PUSH == MSG_FLAG_PUSH

    def set_push(self):
        self.flag |= MSG_FLAG_PUSH

    def set_need_compress(self):
        self.flag |= MSG_FLAG_REQ_COMPRESS

    def set_compressed(self):
        self.flag |= MSG_FLAG_COMPRESSED

    def need_compress(self):
        return self.flag & MSG_FLAG_REQ_COMPRESS == MSG_FLAG_REQ_COMPRESS

    def is_compressed(self):
        return ((not self.c2s and self.flag & MSG_FLAG_COMPRESSED) or
                (self.c2s and self.need_compress()))

    def get_owner_cls(self):
        if self.c2s:
            return request_messages[self.msg_id]
        elif self.is_push_msg():
            return push_messages[self.msg_id]
        else:
            return response_messages[self.msg_id]

class Message(BinaryObject):
    MSG_ID = 0

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.header = kwargs.pop('header', Header(msg_id = self.MSG_ID))

    def empty(self):
        return not self.header.payload_size

    def size(self):
        n = self.header.size() + self.header.payload_size
        if self.owner is None:
            n += len(MSG_TAIL_BYTES)
        return n

    def is_push_msg(self):
        return self.header.is_push_msg()

    def is_asked_for_push(self):
        return (self.header.msg_id > 5510 and getattr(self, 'push_type', None) == 1)

    def is_multiple_message(self):
        return self.header.msg_id == MSG_TYPE_MULTI

    @property
    def msg_id(self):
        return self.header.msg_id

    def tobytes(self, compress = False):
        body = self.body2bytes()
        if compress and self.owner is None:
            self.header.set_compressed()
            self.header.set_need_compress()
            cbody = zlib.compress(body)
            body = Short.pack(len(body)) + Short.pack(len(cbody)) + cbody
        self.header.payload_size = len(body)
        if self.owner is None:
            return self.header.tobytes() + body + MSG_TAIL_BYTES
        else:
            return self.header.tobytes() + body

    def __str__(self):
        if self.empty():
            s = self.header.get_owner_cls().__name__
        else:
            s = type(self).__name__

        if self.is_push_msg():
            s += ' [P]'
        return s + PRINT_PREFIX + super().__str__()

    @staticmethod
    def fromstream(reader, header):
        '''
        Parse a message from the specified buffer. The buffer must be at least
        header.payload_size bytes.
        '''
        if not header.msg_id or not header.payload_size:
            return Message(header = header)

        if header.is_compressed():
            orig_len = reader.read_short()
            cmp_len = reader.read_short()
            buf = zlib.decompress(reader.buf())
            assert len(buf) == orig_len
            reader.reset(buf)

        return header.get_owner_cls()._fromstream(reader,
                  header = header)

    @staticmethod
    def allfromstream(reader, c2s = False):
        n = reader.available()
        if n < Header.type_size:
            return None

        h = Header.fromstream(reader, c2s = c2s)
        if n < Header.type_size + h.payload_size:
            return None

        return Message.fromstream(reader, h)

    @staticmethod
    def frombytes(buf, header):
        '''
        Parse a message from the specified buffer. The buffer must be at least
        header.payload_size bytes.
        '''
        if buf[-1] == ord('}'):
            buf = buf[:-1]

        return Message.fromstream(Reader(buf), header)

    @staticmethod
    def allfrombytes(buf, c2s = False):
        if buf and buf[-1] == ord('}'):
            buf = buf[:-1]

        return Message.allfromstream(Reader(buf), c2s = c2s)

class MessageMeta(BinaryObjectMeta):
    '''
    This meta class will get message id from the class name. If the name is
    not it the format 'xxx_1234' or 'xxx1234' where 1234 is the MSG_ID,
    the MSG_ID field must be defined explicitly in the class.
    '''
    name_id_pattern = re.compile(r'[^\d]+(\d+)')

    def __new__(mcs, name, bases, attrs):
        cls = super().__new__(mcs, name, bases, attrs)

        match = MessageMeta.name_id_pattern.match(name)
        if match:
            cls.MSG_ID = int(match.group(1))

        return cls

class MultipleMessage(Message, metaclass = MessageMeta):
    MSG_ID = MSG_TYPE_MULTI

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.attributes_info = []

    def add_message(self, m):
        if m.owner is None:
            import copy
            m = copy.deepcopy(m)
            m.owner = self
        name = 'submsg_{}'.format(len(self.attributes_info) + 1)
        self.attributes_info.append(Attribute(name, type(m), name))
        setattr(self, name, m)

    @classmethod
    def _fromstream(cls, reader, **kwargs):
        self = cls(**kwargs)
        while reader.available() >= Header.type_size:
            m = Message.allfromstream(reader, owner = self,
                                      c2s = kwargs.get('c2s', True))
            if not m:
                raise ParseError('incomplete {} message'.format(cls.MSG_ID))
            self.add_message(m)
        return self
