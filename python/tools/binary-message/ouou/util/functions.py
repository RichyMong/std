from itertools import groupby
import operator
import time
from datetime import datetime

def repeat_func(func, times, *args):
    for i in range(times):
        yield func(*args)

def to_range_str(iterable):
    '''
    Convert an iterable object of integers to a string in with consecutive
    numbers combined into a range.
    '''
    result = ''
    sorted_list = sorted(iterable)
    for (_, g) in groupby(enumerate(sorted_list), lambda t : t[0] - t[1]):
        rlist = list(map(operator.itemgetter(1), g))
        if len(rlist) > 1:
            single = '{}-{}'.format(rlist[0], rlist[-1])
        else:
            single = '{}'.format(rlist[0])
        if not result:
            result = single
        else:
            result += ' {}'.format(single)

    return result

def range_str_to_list(s):
    ret = []
    for f in s.strip(' ,\n\t').replace(',', ' ').split():
        r = list(map(int, f.split('-')))
        ret.extend(range(r[0], r[-1] + 1))

    return ret

def get_milliseconds():
    return int(time.time()) * 1000 + (datetime.now().microsecond) // 1000

def is_trade_time():
    now = datetime.now()
    return now.isoweekday() in (1, 2, 3, 4, 5) and now.hour in range(9, 16)

# This must be put at the last line!
__all__ = [x for x in locals() if not x.startswith('__')]
