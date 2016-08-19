from __future__ import print_function

import sys

gv = 1

def f1():
    print('S1: ', gv)
    exec("locals()['gv'] = 100")
    print('S2: ', gv)
    print(locals() is f1.__dict__)
    print(sys.modules[__name__].__dict__ is globals())

def f2():
    print('S3: ', gv)
    locals()['gv'] = 1000
    print('S4: ', gv)

def f3():
    if True:
        gv = 10000
    else:
        global gv
    print('S5: ', gv)

f1(), f2(), f3(), gv
print(locals())
print(globals())
print(globals() is locals())
