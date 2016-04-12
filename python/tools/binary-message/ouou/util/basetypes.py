import struct

__all__ = ['Char', 'Byte', 'Short', 'UShort', 'Int', 'DigitInt', 'UInt', 'LargeInt',
           'Long', 'ULong' ]

BYTE_ORDER = '<'

def tobytes(obj):
    if isinstance(obj, str):
        return struct.pack(obj.pack_fmt, obj.encode())
    else:
        return struct.pack(BYTE_ORDER + obj.pack_fmt, obj)

def size(obj):
    '''
    Though the sizes of most of the types are fixed, we use an instance
    method so that we provide the same interfaces as the types which
    are determined by instances, such as String, Vector.
    '''
    return type(obj).type_size


class LegacyTypeMeta(type):
    def __new__(meta, classname, supers, classdict):
        cls = type.__new__(meta, classname, supers, classdict)
        cls.tobytes = tobytes
        cls.type_size = struct.calcsize(cls.pack_fmt)
        cls.size = size

        return cls

    def pack(cls, obj):
        if isinstance(obj, str):
            return struct.pack(cls.pack_fmt, obj.encode())
        else:
            return struct.pack(BYTE_ORDER + cls.pack_fmt, obj)

    def frombytes(cls, buf, offset = 0):
        value = struct.unpack_from(cls.pack_fmt, buf, offset)[0]
        if issubclass(cls, str):
            return cls(value.decode())
        else:
            return cls(value)

    def fromstream(cls, reader, **kwargs):
        buf = reader.read_bytes(cls.type_size)
        return cls.frombytes(buf)


class Char(str, metaclass = LegacyTypeMeta):
    pack_fmt = 'c'
    def __init__(self, s = ''):
        if len(s) > 1:
            raise ValueError('char value "{}" is longer than 1'.format(s))
        self = s


class Byte(int, metaclass = LegacyTypeMeta):
    pack_fmt = 'B'


class Short(int, metaclass = LegacyTypeMeta):
    pack_fmt = 'h'


class UShort(int, metaclass = LegacyTypeMeta):
    pack_fmt = 'H'


class Int(int, metaclass = LegacyTypeMeta):
    pack_fmt = 'i'


def DigitInt(digit_size):
    class Wrapper(Int):
        def __str__(self):
            return '{:.{digits}f}'.format(float(self) / pow(10, digit_size),
                         digits = digit_size)
    return Wrapper


class UInt(int, metaclass = LegacyTypeMeta):
    pack_fmt = 'I'


class Long(int, metaclass = LegacyTypeMeta):
    pack_fmt = 'l'


class ULong(int, metaclass = LegacyTypeMeta):
    pack_fmt = 'L'


class LargeInt(int, metaclass = LegacyTypeMeta):
    pack_fmt = 'i'

    @staticmethod
    def frombytes(buf, offset = 0):
        value = Int.frombytes(buf, offset)
        negative = False
        if value < 0:
            negative = True
            value = -value
        tmp = value >> 29
        if tmp:
            value &= 0x1fffffff
            value <<= tmp * 4

        return LargeInt(value) if not negative else LargeInt(-value)

    def __len__(self):
        return 4

    def tobytes(self):
        return Int(self.value).tobytes()

if __name__ == '__main__':
    sh = Short(0x1234)
    i = Int(0x4578ffff)
    c = Char('h')
    print(type(c))
    print(sh, i, c)
    b = sh.tobytes() + i.tobytes() + c.tobytes()
    print(len(b))
    sh = Short.frombytes(b, 0)
    i = Int.frombytes(b, 2)
    c = Char.frombytes(b, 6)
    print(sh, i, c)
    print(sh.size())
