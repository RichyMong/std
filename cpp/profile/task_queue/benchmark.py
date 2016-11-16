import re
import sys

def calculate_used_time(path):
    times = []
    pattern = re.compile(r'used +(\d+) +microseconds')
    with open(path) as f:
        for line in f:
            m = re.search(pattern, line)
            if m:
                times.append(int(m.group(1)))
    print('average: {}'.format(sum(times) / len(times)))

if __name__ == '__main__':
    calculate_used_time(sys.argv[1])
