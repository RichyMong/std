import struct

__all__ = ['Char', 'Byte', 'Short', 'UShort', 'Int', 'UInt', 'Long', 'ULong', 'Array']

class StructDecorator:
    def __init__(self, fmt):
        self.fmt = fmt

    def __call__(self, origin_cls):
        class Wrapped(origin_cls):
            @classmethod
            def frombytes(cls, buf, offset = 0):
                value = struct.unpack_from(Wrapped.fmt, buf, offset)[0]
                if issubclass(cls, str):
                    return cls(value.decode())
                else:
                    return cls(value)

            @staticmethod
            def size():
                return struct.calcsize(Wrapped.fmt)

            def tobytes(self):
                if issubclass(origin_cls, str):
                    return struct.pack(Wrapped.fmt, self.encode())
                else:
                    return struct.pack(Wrapped.fmt, self)
            fmt = self.fmt

        return Wrapped

@StructDecorator('<s')
class Char(str):
    def __init__(self, arg):
        if len(arg) > 1:
            raise ValueError('Char contains more than one character')
        super(str, Char).__init__(arg)

@StructDecorator('<B')
class Byte(int):
    pass

@StructDecorator('<h')
class Short(int):
    pass

@StructDecorator('<H')
class UShort(int):
    pass

@StructDecorator('<i')
class Int(int):
    pass

@StructDecorator('<I')
class UInt(int):
    pass

@StructDecorator('<l')
class Long(int):
    pass

@StructDecorator('<L')
class ULong(int):
    pass

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
            offset += w.len_cls.size()
            for i in range(size):
                w.append(w.elem_cls.frombytes(buf, offset))
                offset += w.elem_cls.size()
            return w

        def size(self):
            return self.len_cls.size() + self.elem_cls.size() * len(self)


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
