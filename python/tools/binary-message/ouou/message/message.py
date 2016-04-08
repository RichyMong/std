import struct
import zlib
import re
import collections
from . import stream
from ouou.util.const import *
from ouou.util.etypes import *

request_messages = {}
response_messages = {}
push_messages = {}

__all__ = [ 'Header', 'Message' ]


NamedField = collections.namedtuple('NamedField', ['name', 'type', 'desc'])

UnamedField = collections.namedtuple('UnamedField', ['desc', 'type'])

UnamedFieldInfo = collections.namedtuple('UnamedFieldInfo',
        ['count_iter', 'fields_iter', 'fields'])


class BinaryObjectMeta(type):
    def __new__(mcs, name, bases, attrs):
        cls = type.__new__(mcs, name, bases, attrs)

        cls._fields = { x[0] : x[1] for x in attrs.get('named_fields_info', []) }

        opt_fields = {}
        for opt_info in attrs.get('unamed_fields_info', []):
            for field in opt_info.fields:
                opt_fields[field.desc] = field.type
        cls._optional_fields = opt_fields

        return cls


class BinaryObject(metaclass = BinaryObjectMeta):
    '''
    '''
    named_fields_info = ()
    unamed_fields_info = []

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
        else:
            super().__setattr__(name, value)


    def __repr__(self):
        r = type(self).__name__
        display_prefix = '\n\t'

        for (name, _, desc) in self.named_fields_info:
            r += display_prefix + '{}: {}'.format(desc, getattr(self, name))

        if self.unamed_fields_info:
            for i, info in enumerate(self.optional_fields):
                _, iter_func, unamed_fields = self.unamed_fields_info[i]

                for _, sub_info in enumerate(info):
                    r += display_prefix + 40 * '-'
                    if iter_func:
                        for j, field in enumerate(iter_func(self)):
                            r += display_prefix + '{}: {}'.format(unamed_fields[field - 1].desc,
                                       sub_info[j - 1])
                    else:
                        for j, v in enumerate(sub_info):
                            r += display_prefix + '{}: {}'.format(unamed_fields[j].desc, v)


        if not any(self.named_fields_info or self.unamed_fields_info):
            return 'Empty ' + r

        return r


    def _tobytes(self):
        b = b''
        for (field, field_type, *_) in self.named_fields_info:
            v = getattr(self, field)
            if v is None:
                raise ValueError('{}.{} is not set'.format(
                        type(self).__name__, field)
                      )
            try:
                b += v.tobytes()
            except Exception as e:
                raise RuntimeError('Exception while processing field "{}": {}'.format(field, e))

        name = 'optional_fields'
        for info in getattr(self, name, []):
            for v in info:
                b += v.tobytes()

        return b


    def fieldsfrombytes(self, buf, offset = 0):
        reader = stream.Reader(buf, offset)
        for (field, field_type, *_) in self.named_fields_info:
            setattr(self, field, reader.read_etype(field_type))

        if hasattr(self, 'unamed_fields_info'):
            self.optional_fields = list()
            for opt_fields_info in self.unamed_fields_info:
                info = []
                for i in range(opt_fields_info.count_iter(self)):
                    sub_info = []
                    if opt_fields_info.fields_iter:
                        for fid in opt_fields_info.fields_iter(self):
                            name_, field_type, *_ = opt_fields_info.fields[fid - 1]
                            sub_info.append(reader.read_etype(field_type))
                    else:
                        for field in opt_fields_info.fields:
                            sub_info.append(reader.read_etype(field.type))
                    info.append(sub_info)
                self.optional_fields.append(info)

        return reader.offset() - offset


    def size(self):
        total_size = 0
        for (field, field_type, *_) in self.named_fields_info:
            total_size += getattr(self, field).size()

        name = 'unamed_fields_info'
        if hasattr(self, name):
            for i, info in enumerate(self.optional_fields):
                n = 0
                for sub_info in info:
                    for v in sub_info:
                        n += v.size()
                total_size += n * len(info)

        return total_size


    @classmethod
    def _frombytes(cls, buf, offset = 0, **kwargs):
        '''
        Parse binary object from the buffer. If the class' type is Message,
        the buffer does not contain the header part.
        '''
        p = cls(**kwargs)
        p.fieldsfrombytes(buf, offset)
        return p


    @classmethod
    def frombytes(cls, buf, offset = 0, **kwargs):
        return cls._frombytes(buf, offset = offset, **kwargs)


class Header(BinaryObject):
    named_fields_info = (
        NamedField('prefix', Char, 'prefix'),
        NamedField('msg_id', UShort, 'msg_id'),
        NamedField('flag', Byte, 'flag'),
        NamedField('unused', Byte, 'unused'),
        NamedField('msg_size', UShort, 'msg_size'),
    )

    type_size = sum(x[1].type_size for x in named_fields_info)

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


    def tobytes(self):
        return super()._tobytes()


class Message(BinaryObject):
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


    def is_push_pkg(self):
        return self.header.is_push_pkg()


    def is_asked_for_push(self):
        return (self.header.msg_id > 5510 and getattr(self, 'push_type', None) == 1)


    def is_multiple_message(self):
        return self.header.msg_id == 5500


    @staticmethod
    def is_complete(buf):
        if len(buf) >= Header.type_size:
            h = Header.frombytes(buf)
            tail = h.size() + h.msg_size + 1
            if len(buf) >= tail:
                return True
        return False


    @classmethod
    def allfrombytes(cls, buf, c2s = True):
        if not cls.is_complete(buf):
            return None

        header = Header.frombytes(buf, c2s = c2s)
        if header.msg_size:
            data = buf[header.type_size:header.type_size+header.msg_size]
            return Message.frombytes(data, header, c2s)
        else:
            return Message(header = header)


    @staticmethod
    def frombytes(buf, header, c2s = True):
        '''
        Parse a message from the specified buffer. The buffer must be at least
        header.msg_size bytes.
        '''
        if not header.msg_id:
            return Message(header = header)

        if header.is_compressed():
            orig_len = Short.frombytes(buf)
            cmp_len = Short.frombytes(buf, 2)
            buf = zlib.decompress(buf[4:])
            assert len(buf) == orig_len

        if c2s:
            return request_messages[header.msg_id]._frombytes(buf, header = header)
        elif header.is_push_pkg():
            return push_messages[header.msg_id]._frombytes(buf, header = header)
        else:
            return response_messages[header.msg_id]._frombytes(buf, header = header)


    def tobytes(self, compress = False):
        body = self._tobytes()
        if compress:
            self.header.set_compressed()
            self.header.set_need_compress()
            cbody = zlib.compress(body)
            body = Short(len(body)).tobytes() + Short(len(cbody)).tobytes() + cbody
        self.header.msg_size = len(body)
        return self.header.tobytes() + body + Char('}').tobytes()


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
