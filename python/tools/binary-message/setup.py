'''
Add this directory to the pth file. We don't use distutils here since if we
did, we would need to install the library every time it's modified.
'''
import sys
import os

script = os.path.abspath(__file__)

def run_as_superuser():
    params = ' '.join([script] + sys.argv[1:])
    if sys.platform != 'win32':
        os.execlp('sudo', 'sudo', sys.executable, params)
    else:
        print('please run this script as Administrator.')
        sys.exit(1)

req_version = (3, 4)
if sys.version_info < req_version:
    raise RuntimeError('require version {}.{} or later'.format(
              req_version[0], req_version[1j]))

wd = os.path.dirname(script)
reverse = sys.path[-1::-1]
try:
    index = reverse.index(wd)
except ValueError:
    index = -1

# Don't add if the index is found in the middle of the path.
if index in (-1, len(reverse) - 1):
    for path in sys.path:
        if path.endswith('site-packages')  or path.endswith('dist-packages'):
            filepath = os.path.join(path, 'ouou-autotest.pth')
            try:
                with open(filepath, 'w') as f:
                    print('writing', filepath)
                    f.write(wd + '\n')
            except PermissionError:
                run_as_superuser()

            break
else:
    print('path already added')
