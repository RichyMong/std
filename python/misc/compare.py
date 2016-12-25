import collections

def compare(lhs, rhs, prefix = ''):
    '''
    Simple compare. Cross reference is not taken into consideration.
    '''
    assert type(lhs) is type(rhs)
    name_func = lambda x : '.'.join((prefix, x)) if prefix else x
    if isinstance(lhs, (bool, int, float, str)):
        if lhs != rhs:
            return (prefix, lhs, rhs)
    elif isinstance(lhs, collections.Iterable):
        if len(lhs) != len(rhs):
            return (name_func('__len__()'), len(lhs), len(rhs))

        for i, (x, y) in enumerate(zip(lhs, rhs)):
            r = compare(x, y, '{}[{}]'.format(prefix, i))
            if r is not None:
                return r
    else:
        if hasattr(lhs, '__dict__') and lhs.__dict__:
            lattr = set(x for x in lhs.__dict__ if not (x.startswith('__') or callable(getattr(lhs, x))))
            hattr = set(x for x in rhs.__dict__ if not (x.startswith('__') or callable(getattr(rhs, x))))
        else:
            lattr = set(x for x in dir(lhs) if not (x.startswith('__') or callable(getattr(lhs, x))))
            hattr = set(x for x in dir(rhs) if not (x.startswith('__') or callable(getattr(rhs, x))))
        for x in lattr.intersection(hattr):
            lv = getattr(lhs, x)
            rv = getattr(rhs, x)
            r = compare(lv, rv, name_func(x))
            if r is not None:
                return r
    return None

class ObjX(object):
    def __init__(self, name, value):
        self.name = name
        self.value = value
        self.scores = []

    def add_score(self, score):
        self.scores.append(score)

class ObjY(object):
    def __init__(self):
        self.objs = []

    def add_obj(self, obj):
        self.objs.append(obj)


if __name__ == '__main__':
    ly = ObjY()
    for i in range(5):
        x = ObjX('obj_'.format(i+1), i+1)
        for j in range(4):
            x.add_score(82 + j)
        ly.add_obj(x)

    ry = ObjY()
    for i in range(5):
        x = ObjX('obj_'.format(i+1), i+1)
        for j in range(4):
            x.add_score(82 + j)
        if i == 4:
            x.scores[-1] = 100
            x.add_score(82 + i)
        ry.add_obj(x)

    r = compare(ly, ry)
    print(r)
    print(eval('ry.{}'.format(r[0])))
