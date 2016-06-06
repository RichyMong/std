import struct
from .basetypes import *
from .functions import *

__all__ = [ 'TypeList', 'Array', 'Vector', 'ByteVector',
            'CString', 'String', 'GBKString' ]

def CString(array_size, encoding = 'utf-8'):
    class Wrapped(str):
        @classmethod
        def fromstream(cls, reader, **kwargs):
            b = reader.read_bytes(array_size)
            return Wrapped(b.decode(encoding))

        def tobytes(self):
            x = self
            while len(x) < array_size:
                x += '\0'
            return x.encode(encoding)

    return Wrapped

def EncodedString(encoding):
    class WrapperString(str):
        @classmethod
        def fromstream(cls, reader, **kwargs):
            n = reader.read_short()
            s = reader.read_bytes(n)
            return cls(s.decode(encoding = encoding))

        def size(self):
            '''
            Either from or to bytes, the size is decided by encoding. Since we
            inherit str, we are not able to add a length field to save the size.
            '''
            return 2 + len(self.encode(encoding = encoding))

        def tobytes(self):
            s = self.encode(encoding = encoding)
            b = Short.pack(len(s))
            return b + struct.pack('{}s'.format(len(s)), s)

    return WrapperString

String = EncodedString('utf-8')
GBKString = EncodedString('gbk')

def TypeList(elem_cls):
    class WrapperList(list):
        def append(self, obj):
            if not isinstance(obj, elem_cls):
                obj = elem_cls(obj)
            super().append(obj)

        def tobytes(self):
            b = b''
            for elem in self:
                if isinstance(elem, elem_cls):
                    b += elem.tobytes()
                else:
                    b += elem_cls(elem).tobytes()
            return b

        def size(self):
            if hasattr(elem_cls, 'type_size'):
                return elem_cls.type_size * len(self)
            else:
                return sum(x.size() for x in self)

        def __str__(self):
            if issubclass(elem_cls, int):
                return '['  + to_range_str(self) + '] ({})'.format(len(self))
            elif issubclass(elem_cls, str):
                return '[ ' + ' '.join(x for x in self) + '] ({})'.format(len(self))
            else:
                r = '共 {} 个'.format(len(self))
                for i, x in enumerate(self):
                    r += '{p}{sep}{no}{sep}{p}{x}'.format(p = '\n    ',
                             sep = 30 * '-', no = i + 1, x = x)
                return r

    return WrapperList

def Array(array_size, elem_cls):
    class Wrapped(TypeList(elem_cls)):
        if hasattr(elem_cls, 'type_size'):
            type_size = array_size * elem_cls.type_size

        @classmethod
        def fromstream(cls, reader, **kwargs):
            return cls(reader.read_times(elem_cls, array_size))

        def __str__(self):
            if issubclass(elem_cls, str):
                try:
                    idx = self.index('\0')
                except:
                    idx = -1
                return ''.join(x for x in self[:idx])
            else:
                return super().__str__()

        def tobytes(self):
            if len(self) != array_size:
                self.extend(elem_cls() for i in range(array_size - len(self)))
            return super().tobytes()

    return Wrapped

def Vector(size_cls, elem_cls):
    class Wrapped(TypeList(elem_cls)):
        @classmethod
        def fromstream(cls, reader, **kwargs):
            size = reader.read_type(size_cls)
            return cls(reader.read_times(elem_cls, size))

        def size(self):
            return size_cls.type_size + super().size()

        def tobytes(self):
            return size_cls.pack(len(self)) + super().tobytes()

    return Wrapped

ByteVector = Vector(Byte, Byte)
