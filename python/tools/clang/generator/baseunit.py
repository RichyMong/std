import os
import util
from clang.cindex import CursorKind

class BaseUnit(object):
    def __init__(self, prjname, filename, root = None):
        self.root = root or os.getcwd()
        self.unit = self.parse_from_dir(self.root, prjname, filename)
        self.relpath = os.path.relpath(self.filepath, self.root)

    @property
    def filepath(self):
        return self.unit.spelling

    @property
    def cursor(self):
        return self.unit.cursor

    @staticmethod
    def parse_from_dir(root, prjname, filename):
        prjdir = os.path.join(root or os.getcwd(), prjname)
        incdirs = util.get_netbeans_incdirs(prjdir)
        if prjname == 'multicachedata':
            filepath = os.path.join(prjname, filename)
        else:
            filepath = os.path.join('src', filename)
        return util.parse_cpp_unit(os.path.join(prjdir, filepath), incdirs)

    def find_classes(self):
        for c in self.unit.cursor.get_children():
            if c.location.file.name != self.unit.spelling:
                continue

            if c.kind in (CursorKind.CLASS_DECL, CursorKind.STRUCT_DECL):
                yield c

    def find(self, kind, func):
        return util.find(self.unit.cursor, kind, func)

    def find_class_cursor(self, class_name):
        for c in self.find_classes():
            if c.spelling.lower() == class_name.lower():
                return c
        return None

    def add_method_declaration(self, class_name, method_name, result_type,
            arg_type_names, func_type = util.NORMAL):
        cursor = self.find_class_cursor(class_name)
        if not cursor:
            raise RuntimeError('cannot find class "{}" in file "{}"'.format(
                      class_name, self.unit.spelling))

        arg_types = [x[0] for x in arg_type_names]

        arg_type_func = lambda c : [x.type.spelling for x in c.get_arguments()]

        func_decl = ''
        if func_type in (util.VIRTUAL, util.PURE_VIRTUAL):
            func_decl = 'virtual '
        func_decl += '{} {}({})'.format(result_type, method_name,
                        ', '.join(x[0] + ' ' + x[1] for x in arg_type_names))
        if func_type == util.PURE_VIRTUAL:
            func_decl += ' = 0'
        func_decl += ';'

        print('Add declaration of "{}::{}" to file "{}"...'.format(class_name,
            method_name, self.relpath), end = '')

        exists, target = util.find_or_insert(cursor,
                    CursorKind.CXX_METHOD,
                    lambda c : c.spelling.startswith(method_name),
                    lambda c : arg_type_func(c) == arg_types)
        if exists:
            print('already there at line {}'.format(target.location.line))
        else:
            util.insert_after_line(self.filepath, target.extent.end.line,
                    util.INDENT + func_decl)
            print('done')

    def add_method_definitaion(self, class_name, method_name, result_type,
            arg_type_names, body = None):
        arg_types = [x[0] for x in arg_type_names]

        arg_type_func = lambda c : [x.type.spelling for x in c.get_arguments()]

        func_def = '{} {}::{}({})'.format(result_type, class_name, method_name,
                    ', '.join(x[0] + ' ' + x[1] for x in arg_type_names))

        print('Add definition of "{}::{}" to file "{}"...'.format(class_name,
            method_name, self.relpath), end = '')

        exists, target = self.find_or_insert(CursorKind.CXX_METHOD,
                            lambda c : (c.semantic_parent.spelling == class_name and
                                    c.spelling.startswith(method_name)),
                            lambda c : c.is_definition() and arg_type_func(c) == arg_types)
        if exists:
            print('already there at line {}'.format(target.location.line))
        else:
            util.insert_after_line(self.filepath,
                    (target and target.extent.end.line) or -1,
                    '\n{}\n{{\n{}\n}}'.format(func_def, body))
            print('done')

    def find_or_insert(self, kind, match_func, equal_func = None):
        return util.find_or_insert(self.unit.cursor, kind, match_func, equal_func)
