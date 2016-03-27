import struct

request_messages = {}
response_messages = {}
push_messages = {}


class BinaryObjectMeta(type):
    def __new__(mcs, name, bases, attrs):
        cls = type.__new__(mcs, name, bases, attrs)

        cls._fields = tuple(x[0] for x in cls.fields_info)

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


class BinaryObject(object, metaclass = BinaryObjectMeta):
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
            super().__setattr__(name, value)
        else:
            raise ValueError("no field named '{}' in {}".format(
                      name, type(self).__name__)
                  )


    def tohex(self):
        b = b''
        for (name, fmt) in self.fields_info:
            if not len(fmt):
                continue

            v = getattr(self, name)
            if v is None:
                raise ValueError('{}.{} is not set'.format(
                        type(self).__name__name)
                      )
            if fmt[0] == '*':
                b += struct.pack(fmt[1:3], len(v))
                for x in v:
                    b += self.pack_single(fmt[3:], x)
            else:
                b += self.pack_single(fmt, v)
        return b

    @staticmethod
    def pack_single(fmt, e):
        if fmt[-1] == 's':
            e = e.encode('utf-8')
            return struct.pack('<H', len(e)) + struct.pack('{}s'.format(len(e)), e)
        else:
            return struct.pack(fmt, e)

    @staticmethod
    def unpack_single(fmt, buf, offset):
        if fmt[-1] == 's':
            size = struct.unpack_from('<H', buf, offset)[0]
            b = struct.unpack_from('{}s'.format(size), buf, offset + 2)[0]
            return (b.decode('utf-8'), size + 2)
        else:
            return (struct.unpack_from(fmt, buf, offset)[0], struct.calcsize(fmt))

    @classmethod
    def fromhex(cls, buf):
        offset = 0
        p = cls()
        for (name, fmt) in cls.fields_info:
            if not len(fmt):
                continue

            if fmt[0] == '*':
                value = []
                size = struct.unpack_from(fmt[1:3], buf, offset)[0]
                offset += struct.calcsize(fmt[1:3])
                for x in range(size):
                    r = BinaryObject.unpack_single(fmt[3:], buf, offset)
                    value.append(r[0])
                    offset += r[1]
                setattr(p, name, value)
            else:
                r = BinaryObject.unpack_single(fmt, buf, offset)
                offset += r[1]
                setattr(p, name, r[0])
        return p


class Header(BinaryObject):
    fields_info = (
                      ('prefix', '<c'),
                      ('payload_id', '<H'),
                      ('attr', '*<B'),
                      ('payload_size', '<H'),
                  )

    SIZE = 7


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

p = RequestMessage()
print(type(p))
