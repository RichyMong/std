import functools

class Tracer(object):
    def __init__(self, func):
        self.func = func

    def __call__(self, *args, **kwargs):
        print('__call__', self.func.__name__)
        return self.func(*args, **kwargs)

    def __get__(self, obj, owner):
        return functools.partial(self.__call__, obj)

class Suite(object):
    def __init__(self, name):
        self.name = name

    @Tracer
    def greeting(self, to):
        print(self.name,  'greeting to', to)

Suite('Rich').greeting('ouou')
