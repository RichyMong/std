import time
from datetime import datetime

def to_range_str(iterable):
    '''
    iteratools.groupby could by used here
    '''
    values = list(iterable)
    if not len(values):
        return ''

    values.sort()
    result = ''

    s, l = 0, 0
    for i in range(1, len(values)):
        if values[i] != values[i - 1] + 1:
            if s == l:
                result += '{} '.format(values[s])
            else:
                result += '{}-{} '.format(values[s], values[l])
            s = i

        l = i

    if s == l:
        result += '{}'.format(values[s])
    else:
        result += '{}-{}'.format(values[s], values[l])

    return result


def range_str_to_list(s):
    ret = []
    for f in s.strip(' ,\n\t').replace(',', ' ').split():
        r = f.split('-')
        for i in range(int(r[0]), int(r[-1]) + 1):
            ret.append(i)

    return ret


def get_milliseconds():
    return int(time.time()) * 1000 + (datetime.now().microsecond) // 1000


def is_trade_time():
    now = datetime.now()
    return now.isoweekday() in (1, 2, 3, 4, 5) and now.hour in range(9, 16)


# This must be put at the last line!
__all__ = [x for x in locals() if not x.startswith('__')]
