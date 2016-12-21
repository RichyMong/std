#!/usr/bin/env python
import sys
import cachedata

def main():
    import argparse

    parser = argparse.ArgumentParser(description="add new interfaces automatically")
    parser.add_argument('-c', '--classname', action='store', help='class name in "StdQtStruct.h"')
    ns = parser.parse_args(args=sys.argv[1:])

    if not ns.classname:
        print('invalid arguments')

    cache = cachedata.MultiCache()
    cache.add_new_class(ns.classname)

if __name__ == '__main__':
    main()
