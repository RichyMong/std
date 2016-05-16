#!/usr/bin/env python3
import os
import sys
import shutil

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
GIT_IGNORE_FILE = os.path.join(SCRIPT_DIR, '.gitignore')

def make_chapter_dir(chpno):
    relname = 'chp{}'.format(chpno)
    absname = os.path.join(SCRIPT_DIR, relname)
    dirname = relname if os.getcwd() == SCRIPT_DIR else absname
    if os.path.exists(absname):
        print('Directory {} already exists.'.format(dirname))
    else:
        os.mkdir(dirname)
        shutil.copy(GIT_IGNORE_FILE, dirname)

if __name__ == '__main__':
    if len(sys.argv) > 1:
        make_chapter_dir(int(sys.argv[1]))
    else:
        print('usage: {} <chapter-number>'.format(sys.argv[0]))
        sys.exit(1)
