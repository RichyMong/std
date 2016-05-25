class X(object):
    attr1 = 1
    def __init__(self):
        self.attr2 = 2

    def __getattr__(self, attr):
        if attr == 'attr3':
            return 3
        else:
            return object.__getattr__(self, attr)

class Y(object):
    __slots__ = ('attr1', 'attr2')

    def __init__(self):
        self.attr1 = 1
        self.attr2 = 2

x = X()
y = Y()
# print(x.__dict__['attr1'])
print(x.__dict__['attr2'])
# print(x.__dict__['attr3'])
print(getattr(x, 'attr1'))
print(getattr(x, 'attr2'))
print(getattr(x, 'attr3'))
print(getattr(y, 'attr1'))
print(getattr(y, 'attr2'))
# print(getattr(y, 'attr3'))
print(y.__dict__['attr1'])
print(y.__dict__['attr2'])
print(y.__dict__['attr3'])
