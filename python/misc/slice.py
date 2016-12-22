class SliceObject(object):
    def __init__(self, x):
        self._a = 1
        self._b = 2
        self._c = 3
        self._d = 4
        self._e = 5
        self._attrs = ['_a', '_b', '_c', '_d', '_e']

    def __getitem__(self, key):
        if isinstance(key, slice):
            indices = key.indices(len(self._attrs))
            return [getattr(self, self._attrs[x]) for x in range(*indices)]
        elif isinstance(key, int):
            if key >= len(self._attrs):
                raise IndexError('overread')
            return getattr(self, self._attrs[key])
        else:
            raise TypeError('only support integer and slice')

    def __setitem__(self, key, value):
        if isinstance(key, slice):
            indices = key.indices(len(self._attrs))
            for x, y in zip(list(range(*indices)), value):
                setattr(self, self._attrs[x], y)
        elif isinstance(key, int):
            if key >= len(self._attrs):
                raise IndexError('overread')
            setattr(self, self._attrs[key], value)
        else:
            raise TypeError('only support integer and slice')

s = SliceObject(10)
print(s[4])
print(s[1:5])
s[2:] = [6, 7, 8]
print(s[2:-1])
print(s[-1])
