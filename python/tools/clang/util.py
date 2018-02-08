import subprocess
import re

def get_gcc_version():
    try:
        f = subprocess.Popen(['g++', '--version'], stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT)
        m = re.search(r'g\+\+ \(GCC\) ([\d.]+)', f.stdout.read().decode())
        if m:
            return m.group(1).strip()
    except OSError:
        pass
    return 'unknown'

