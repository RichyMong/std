import struct
from emoney import util

__all__ = ['Char', 'Byte', 'Short', 'UShort', 'Int', 'UInt', 'Long', 'ULong',
           'Array', 'Vector', 'String']

def tobytes(obj):
    if isinstance(obj, str):
        return struct.pack(obj.pack_fmt, obj.encode())
    else:
        return struct.pack(obj.pack_fmt, obj)

def size(obj):
    '''
    Though the sizes of most of the types are fixed, we use an instance
    method so that we provide the same interfaces as the types which
    are determined by instances, such as String, Vector.
    '''
    return type(obj).type_size

class LegacyTypeMeta(type):
    def __new__(meta, classname, supers, classdict):
        classdict['tobytes'] = tobytes
        classdict['size'] = size
        classdict['type_size'] = struct.calcsize(classdict['pack_fmt'])

        return type.__new__(meta, classname, supers, classdict)

    def frombytes(cls, buf, offset = 0):
        value = struct.unpack_from(cls.pack_fmt, buf, offset)[0]
        if issubclass(cls, str):
            return cls(value.decode())
        else:
            return cls(value)


class Char(str, metaclass = LegacyTypeMeta):
    pack_fmt = '<c'

    def __init__(self, arg):
        if len(arg) > 1:
            raise ValueError('Char contains more than one character')
        super(str, Char).__init__(arg)

class Byte(int, metaclass = LegacyTypeMeta):
    pack_fmt = '<B'

class Short(int, metaclass = LegacyTypeMeta):
    pack_fmt = '<h'

class UShort(int, metaclass = LegacyTypeMeta):
    pack_fmt = '<H'

class Int(int, metaclass = LegacyTypeMeta):
    pack_fmt = '<i'

class UInt(int, metaclass = LegacyTypeMeta):
    pack_fmt = '<I'

class Long(int, metaclass = LegacyTypeMeta):
    pack_fmt = '<l'

class ULong(int, metaclass = LegacyTypeMeta):
    pack_fmt = '<L'

class LargeInt(int):
    def __init__(self, value):
        self.value = value

    @staticmethod
    def frombytes(buf, offset = 0):
        value = Int.frombytes(buf, offset)

    def __len__(self):
        return 4

    def tobytes(self):
        return Int(self.value).tobytes()


class String(str):
    @staticmethod
    def frombytes(buf, offset = 0):
        n = Short.frombytes(buf, offset)
        s = struct.unpack_from('{}s'.format(n), buf, offset + n.size())[0]
        return String(s.decode())


    def size(self):
        return 2 + len(self)

    def tobytes(self):
        s = self.encode()
        b = Short(len(s)).tobytes()
        return b + struct.pack('{}s'.format(len(s)), s)


def Array(ecls, length):
    class Wrapped(list):
        elem_cls = ecls
        array_size = length

        @staticmethod
        def frombytes(buf, offset = 0):
            w = Wrapped()
            for i in range(w.array_size):
                w.append(w.elem_cls.frombytes(buf, offset))
                offset += w.elem_cls.type_size
            return w

        def size(self):
            return self.elem_cls.type_size * len(self)


        def tobytes(self):
            b = b''
            for x in self:
                b += self.elem_cls(x).tobytes()
            return b

        def __repr__(self):
            return ''.join(str(x) for x in self)

    return Wrapped


def Vector(lcls, ecls):
    class Wrapped(list):
        len_cls = lcls
        elem_cls = ecls

        @staticmethod
        def frombytes(buf, offset = 0):
            w = Wrapped()
            size = w.len_cls.frombytes(buf, offset)
            offset += w.len_cls.type_size
            for i in range(size):
                elem = w.elem_cls.frombytes(buf, offset)
                w.append(elem)
                offset += elem.size()
            return w


        def size(self):
            if hasattr(self.elem_cls, 'type_size'):
                return self.len_cls.type_size + self.elem_cls.type_size * len(self)
            else:
                return self.len_cls.type_size + sum(x.size() for x in self)


        def tobytes(self):
            b = self.len_cls(len(self)).tobytes()
            for x in self:
                b += self.elem_cls(x).tobytes()
            return b

        def __repr__(self):
            return '['  + util.range_str(self) + '] ({})'.format(len(self))

    return Wrapped

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

    ay = Vector(Byte, Int)()
    ay.append(1)
    ay.append(2)
    ay.append(1)
    ay.append(1)
    print(len(ay), ay)
    b = ay.tobytes()
    print(len(b))
    ay = Vector(Byte, Int).frombytes(b)
    print(len(ay), ay)
    s = String('hello')
    b = s.tobytes()
    print(len(b), b)
    print(String.frombytes(b))
