#!/usr/bin/env python

import os
import sys
from pprint import pprint
import clang.cindex
import fileinput

os.environ['PATH'] = os.environ['PATH']  + os.getcwd()

def add_class_method(cursor, method):
    if cursor.kind != clang.cindex.CursorKind.CLASS_DECL:
        return

    last_location = None
    next_location = None

    for c in cursor.get_children():
        if c.kind == clang.cindex.CursorKind.CXX_METHOD:
            if c.spelling == 'setData':
                last_location = c.location
        elif last_location and c.kind == clang.cindex.CursorKind.CXX_ACCESS_SPEC_DECL:
            next_location = c.location
            break

    if last_location:
        filepath = cursor.location.file.name
        for (i, line) in enumerate(fileinput.input(filepath, inplace = 1)):
            print(line, end = '')
            if i + 1 == last_location.line:
                print(method)

    print(last_location, next_location)

def process_file(filename):
    from clang.cindex import Index

    index = Index.create()
    tu = index.parse(None, filename)
    if not tu:
        print("unable to load input")

    for c in tu.cursor.get_children():
        if c.kind == clang.cindex.CursorKind.CLASS_DECL:
            add_class_method(c, '    void setData(const char*);')

def main():
    from pprint import pprint

    from optparse import OptionParser, OptionGroup

    global opts

    parser = OptionParser("usage: %prog {filename} [clang-args*]")
    parser.disable_interspersed_args()
    (opts, args) = parser.parse_args()

    if len(args) == 0:
        print('invalid number arguments')

    process_file(args)

if __name__ == '__main__':
    main()
