import struct

__all__ = ['Char', 'Byte', 'Short', 'UShort', 'Int', 'UInt', 'Long', 'ULong', 'Array']

def tobytes(obj):
    if isinstance(obj, str):
        return struct.pack(obj.pack_fmt, obj.encode())
    else:
        return struct.pack(obj.pack_fmt, obj)

def size(obj):
    '''
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
    pack_fmt = 'c'

    def __init__(self, arg):
        if len(arg) > 1:
            raise ValueError('Char contains more than one character')
        super(str, Char).__init__(arg)

class Byte(int, metaclass = LegacyTypeMeta):
    pack_fmt = 'B'

class Short(int, metaclass = LegacyTypeMeta):
    pack_fmt = 'h'

class UShort(int, metaclass = LegacyTypeMeta):
    pack_fmt = 'H'

class Int(int, metaclass = LegacyTypeMeta):
    pack_fmt = 'i'

class UInt(int, metaclass = LegacyTypeMeta):
    pack_fmt = 'I'

class Long(int, metaclass = LegacyTypeMeta):
    pack_fmt = 'l'

class ULong(int, metaclass = LegacyTypeMeta):
    pack_fmt = 'L'

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

def Array(lcls, ecls):
    class Wrapped(list):
        len_cls = lcls
        elem_cls = ecls

        @staticmethod
        def frombytes(buf, offset = 0):
            w = Wrapped()
            size = w.len_cls.frombytes(buf, offset)
            offset += w.len_cls.type_size
            for i in range(size):
                w.append(w.elem_cls.frombytes(buf, offset))
                offset += w.elem_cls.type_size
            return w

        def size(self):
            return self.len_cls.type_size + self.elem_cls.type_size * len(self)


        def tobytes(self):
            b = self.len_cls(len(self)).tobytes()
            for x in self:
                b += self.elem_cls(x).tobytes()
            return b

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

    ay = Array(Byte, Int)()
    ay.append(1)
    ay.append(2)
    ay.append(1)
    ay.append(1)
    print(len(ay), ay)
    b = ay.tobytes()
    print(len(b))
    ay = Array(Byte, Int).frombytes(b)
    print(len(ay), ay)
