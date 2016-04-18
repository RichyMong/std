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

    import win32com.shell.shell as shell
    if '--admin' in sys.argv:
        return None
    params += ' asadmin'
    shell.ShellExecuteEx(lpVerb='runas', lpFile=sys.executable, lpParameters=params)
    sys.exit(0)
    import win32event, win32api, win32process
    SEE_MASK_NO_CONSOLE = 0x00008000
    SEE_MASK_NOCLOSE_PROCESS = 0x00000040
    process = shell.ShellExecuteEx(lpVerb='runas', lpFile=sys.executable,
        lpParameters=params, fMask=SEE_MASK_NO_CONSOLE|SEE_MASK_NOCLOSE_PROCESS)
    hProcess = process['hProcess']
    if not hProcess:
        raise Exception("Could not identify administrator process to install drivers")
    # It is necessary to wait for the elevated process or else
    #  stdin lines are shared between 2 processes: they get one line each
    INFINITE = -1
    win32event.WaitForSingleObject(hProcess, INFINITE)
    exitcode = win32process.GetExitCodeProcess(hProcess)
    win32api.CloseHandle(hProcess)
    return exitcode

req_version = (3, 4)
if sys.version_info < req_version:
    raise RuntimeError('require version {}.{} or later'.format(
              req_version[0], req_version[1j]))

wd = os.path.dirname(script)
if wd not in sys.path or sys.path[0] == wd:
    for path in sys.path:
        if path.endswith('site-packages')  or path.endswith('dist-packages'):
            filepath = os.path.join(path, 'emoney-autotest.pth')
            try:
                with open(filepath, 'w') as f:
                    print('writing', filepath)
                    f.write(wd + '\n')
            except PermissionError:
                run_as_superuser()

            break
else:
    print('path already added')
