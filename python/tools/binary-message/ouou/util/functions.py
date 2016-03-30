def range_str(iterable):
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
