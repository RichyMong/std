class B(object):
    SIZE = 'BSIZE'
    __slots__ = ('a', 'b')
    def __init__(self):
        print(type(self))
        print(self.SIZE)


class D(B):
    SIZE = 'DSIZE'
    __slots__ = ('c', 'd')

    def __init__(self):
        super().__init__()
        print(self.SIZE)

d = D()
d.a = 10
d.c = 20
d.e = 20
