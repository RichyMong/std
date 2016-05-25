class Decorator(object):
    def __init__(self, greeting = 'hi'):
        print('{} __init__'.format(greeting))
        self.greeting = greeting

    def __call__(self, func):
        print('__call__')
        def new_func(*args, **kwargs):
            print('{} calling new_func'.format(self.greeting))
            return func(*args, **kwargs)
        new_func.__name__ = func.__name__
        new_func.__doc__ = func.__doc__
        return new_func

    def doit(self):
        print('do it', self.greeting)

class DecoratorNoArg(object):
    def __init__(self, f):
        print('DecoratorNoArg __init__')
        self.f = f

    def __call__(self, *args, **kwargs):
        return self.f(*args, **kwargs)

    def doit(self):
        print('do it')

def enter_func(f):
    def wrapper(*args, **kwargs):
        print('wrapping', f.__name__)
        f(*args, **kwargs)
    return wrapper

def func_decorator_arg(message = 'hello world'):
    print('func_decorator_arg')
    def decorated(func):
        print('decorated')
        def wrapper(*args, **kwargs):
            print('wrapper, message is', message)
            print(func.__name__)
            func(*args, **kwargs)
        return wrapper
    return decorated

print('module begin')
@func_decorator_arg
def func_with_args(x):
    print('func_with_args', x)
    return None

print('begin to test func_with_args')
print(func_with_args)
t = func_with_args(func_with_args)(1)
print(type(t))
print

@func_decorator_arg()
def func_with_args2(x):
    print('func_with_args', x)
    return None

print('begin to test func_with_args2')
t = func_with_args2(1)
print(type(t))
print

@enter_func
def func_enter():
    print('hello enter')
print('enter decorator test begin')
func_enter()

@DecoratorNoArg
def func1():
    print('func1')

print('DecoratorNoArg func1')
print(type(func1))
func1()
func1.doit()

@Decorator('hello')
def func2(x):
    print('func2', x)

print('defined func2')
print(type(func2))
print(type(map))
print(func2.__name__)
func2('body')
