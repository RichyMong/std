import struct
import zlib
import re
import collections
from .const import *
from emoney.util.basetypes import Byte, Char, UShort
from emoney.util.stream import Reader

request_messages = {}
response_messages = {}
push_messages = {}

__all__ = [ 'Header', 'Message', 'MultipleMessage' ]

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
                try:
                    setattr(self, field, reader.read_type(field_type))
                except Exception as e:
                    print('processing message {}, attribute: {} error: {}'.format(cls.__name__, field, e))
                    raise

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
            return 'Empty'

        return r

    def body2bytes(self):
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
        return self.body2bytes()

    def size(self):
        total_size = 0
        for (field, field_type, *_) in self.attributes_info:
            total_size += getattr(self, field).size()

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
        self.c2s = kwargs.get('c2s', True)

    def __str__(self):
        attr = (self.flag << 8) & self.unused
        return "prefix: '{}', type: {}, attribute: 0x{:x}, size: {}".format(
                    self.prefix, self.msg_id, attr, self.payload_size
                )

    @property
    def msg_size(self):
        return self.size() + self.payload_size + len(MSG_TAIL)

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

class Message(BinaryObject):
    MSG_ID = 0

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.header = kwargs.pop('header', Header(msg_id = self.MSG_ID))

    def empty(self):
        return not self.header.payload_size

    def size(self):
        return self.header.size() + self.header.payload_size + len(MSG_TAIL)

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
        if compress:
            self.header.set_compressed()
            self.header.set_need_compress()
            cbody = zlib.compress(body)
            body = Short.pack(len(body)) + Short.pack(len(cbody)) + cbody
        self.header.payload_size = len(body)
        return self.header.tobytes() + body + MSG_TAIL

    def __str__(self):
        return type(self).__name__ + PRINT_PREFIX + super().__str__()

    @staticmethod
    def fromstream(reader, header, c2s = True):
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

        cls = None
        if c2s:
            cls = request_messages[header.msg_id]
        elif header.is_push_msg():
            cls = push_messages[header.msg_id]
        else:
            cls = response_messages[header.msg_id]
        return cls._fromstream(reader, header = header)

    @staticmethod
    def allfromstream(reader, c2s = True):
        n = reader.available()
        if n < Header.type_size:
            return None

        h = Header.fromstream(reader, c2s = c2s)
        if n < Header.type_size + h.payload_size + 1:
            return None

        return Message.fromstream(reader, h, c2s = c2s)

    @staticmethod
    def frombytes(buf, header, c2s = True):
        '''
        Parse a message from the specified buffer. The buffer must be at least
        header.payload_size bytes.
        '''
        return Message.fromstream(Reader(buf), header, c2s = c2s)

    @staticmethod
    def allfrombytes(buf, c2s = True):
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
        self.messages = []

    def add_message(self, message):
        self.messages.append(message)

    def __iter__(self):
        return iter(self.messages)

    def body2bytes(self):
        b = b''
        for m in self.messages:
            body = m.body2bytes()
            m.header.payload_size = len(body)
            b += m.header.tobytes() + body
        return b

    def __str__(self):
        r = 'MultipleMessage: {}'.format(self.MSG_ID)
        for m in self.messages:
            r += PRINT_PREFIX + str(m)
        return r

    @classmethod
    def _fromstream(cls, reader, **kwargs):
        self = cls(**kwargs)
        while reader.available() >= Header.type_size:
            m = Message.allfromstream(reader)
            if not m:
                raise RuntimeError('incomplete {} message'.format(cls.MSG_ID))
            self.messages.append(m)
        return self
