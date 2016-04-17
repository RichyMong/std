'''
Add this directory to the pth file. We don't use distutils here since if we
did, we would need to install the library every time it's modified.
'''
import sys
import os

req_version = (3, 4)
if sys.version_info < req_version:
    raise RuntimeError('require version {}.{} or later'.format(
              req_version[0], req_version[1j]))

wd = os.path.abspath(os.path.dirname(__file__))
if wd not in sys.path:
    for path in sys.path:
        if path.endswith('site-packages')  or path.endswith('dist-packages'):
            filepath = os.path.join(path, 'ouou-autotest.pth')
            with open(filepath, 'w') as f:
                print('writing', filepath)
                f.write(wd + '\n')
            break
else:
    print('path already added')
