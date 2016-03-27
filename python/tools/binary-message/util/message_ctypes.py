import struct
from legacytypes import *

request_messages = {}
response_messages = {}
push_messages = {}

class BinaryObjectMeta(type):
    def __new__(mcs, name, bases, attrs):
        cls = type.__new__(mcs, name, bases, attrs)

        cls._fields = { x[0] : x[1] for x in cls.fields_info }

        return cls


class RequestMessageMeta(BinaryObjectMeta):
    def __new__(mcs, name, bases, attrs):
        cls = super().__new__(mcs, name, bases, attrs)

        if cls.payload_id in request_messages:
            raise RuntimeError('duplicated message type {} '.format(cls.payload_id))

        request_messages[cls.payload_id] = cls

        return cls


class ResponseMessageMeta(BinaryObjectMeta):
    '''
    The meta class for response messages. The according class is supposed to
    define a 'payload_id' class attribute and it will be registered to process
    response messages of this type. If there's no class registered to process
    push messages of this type, we will also register it in push messages.
    '''
    def __new__(mcs, name, bases, attrs):
        cls = type.__new__(mcs, name, bases, attrs)

        response_messages[cls.payload_id] = cls

        # Be careful not to override the push message already registered.
        if cls.payload_id not in push_messages:
            push_messages[cls.payload_id] = cls

        return cls


class PushMessageMeta(BinaryObjectMeta):
    '''
    The meta class for push messages. If not provided, ResponseMessageMeta
    will register the response message class as default.
    '''
    def __new__(mcs, name, bases, attrs):
        cls = type.__new__(mcs, name, bases, attrs)

        if cls.payload_id and type(cls) is PushMessageMeta:
            raise RuntimeError('duplicated message type {} '.format(cls.payload_id))
        push_messages[cls.payload_id] = cls

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
            setattr(self, name, kwargs.get(name, None))


    def __setattr__(self, name, value):
        if name in self._fields:
            if value and not isinstance(value, self._fields[name]):
                value = self._fields[name](value)
            super().__setattr__(name, value)
        else:
            raise ValueError("no field named '{}' in {}".format(
                      name, type(self).__name__)
                  )


    def tobytes(self):
        b = b''
        for (field, field_type) in self.fields_info:
            v = getattr(self, field)
            if v is None:
                raise ValueError('{}.{} is not set'.format(
                        type(self).__name__, field)
                      )
            b += v.tobytes()
        return b

    @classmethod
    def frombytes(cls, buf):
        offset = 0
        p = cls()
        for (field, field_type) in cls.fields_info:
            value = field_type.frombytes(buf, offset)
            offset += value.size()
            setattr(p, field, value)
        return p


class Header(BinaryObject):
    fields_info = (
                      ('prefix', Char),
                      ('payload_id', UShort),
                      ('attr', UShort),
                      ('payload_size', UShort),
                  )

    SIZE = 7

    def __repr__(self):
        return "prefix: '{}', type: {}, attribute: 0x{:x}, size: {}".format(
                    self.prefix, self.payload_id, self.attr, self.payload_size
                )


class Message(BinaryObject):
    def __init__(self, **kwargs):
        if 'header' in kwargs:
            self.header = kwargs.pop('header')
        else:
            self.header = Header()
            self.header.payload_id = kwargs.pop('payload_id', 0)
        super().__init__(**kwargs)


    def __setattr__(self, name, value):
        if name == 'header':
            self.__dict__['header'] = value
        else:
            super().__setattr__(name, value)


    @staticmethod
    def __new__(cls, *args, **kwargs):
        if cls is Message:
            raise ValueError("Please don't use Message directly")
        else:
            return super().__new__(cls, *args, **kwargs)

class RequestMessage(Message, metaclass = RequestMessageMeta):
    payload_id = 1000


if __name__ == '__main__':
    h = Header()
    h.prefix = '{'
    h.payload_id = 5500
    h.attr = 0x1234
    h.payload_size = 0xffff
    print(h)
    b = h.tobytes()
    print('packaged size', len(b))
    y = Header.frombytes(b)
    print(y)
