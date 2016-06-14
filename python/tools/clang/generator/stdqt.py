import os
import fileinput
import util

class StdData(object):
    CLASS_PREFIX = 'CStd'

    def __init__(self, root = None):
        self.unit = util.parse_from_dir(root, 'multicompress', 'src/StdQtStruct.h')

    def find_class_cursor(self, classname):
        return util.find_class_cursor(self.unit, classname)
