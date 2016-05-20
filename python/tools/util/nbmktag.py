#!/usr/bin/env python
import os
import datetime
import subprocess
import xml.etree.ElementTree as ET

BASE_DIR = os.getcwd()
SRC_FILES = datetime.datetime.now().strftime('srcfile_%Y%m%d')
SUPPORTED_EXTENSIONS = ('c', 'cpp', 'cc', 'h', 'hpp')

with open(SRC_FILES, 'w') as fp:
    file_cnt = 0

    for path in os.listdir(BASE_DIR):
        ext = os.path.splitext(path)
        if len(ext) > 1 and ext[1][1:] in SUPPORTED_EXTENSIONS:
            fp.write(path  + '\n')
            file_cnt += 1
    fp.flush()

    if file_cnt:
        subprocess.call(['ctags', '-L', SRC_FILES])
        subprocess.call(['cscope', '-bqu', '-i', SRC_FILES])
