import xml.etree.ElementTree as ET
import fileinput
from os import path as opath
from clang.cindex import Index, CursorKind

NB_PRJ_CFG = opath.join('nbproject', 'configurations.xml')

INDENT = 4 * ' '

NORMAL = 0
VIRTUAL = 1
PURE_VIRTUAL = 2

def get_netbeans_incdirs(srcdir):
    absdir = opath.abspath(srcdir)

    nbcfg = opath.join(absdir, NB_PRJ_CFG)

    incdirs = []

    # The caller ensures the file exists
    tree = ET.parse(nbcfg)
    for conf in tree.findall('confs/conf'):
        if conf.attrib['name'] == 'Debug':
            for elem in conf.findall('compileType/ccTool/incDir/pElem'):
                incdirs.append(opath.abspath(opath.join(absdir, elem.text)))
            break
    return incdirs

def parse_cpp_unit(filepath, incdirs = None, compile_args = None):
    args = [ filepath, '-x', '-c++', '-std=c++11', '-isystem', '/usr/include/c++/6.1.1' ]
    if compile_args:
        args.extend(compile_args)

    if incdirs:
        for d in incdirs:
            args.append('-I')
            args.append(d)

    index = Index.create()
    return index.parse(None, args)

def find_class_cursor(unit, classname):
    for c in unit.cursor.get_children():
        if c.location.file.name != unit.spelling:
            continue

        if (c.kind == CursorKind.CLASS_DECL and
            c.spelling.lower() == classname.lower()):
            return c
    return None

def find_methods(cursor, filter_func = None):
    classes = []

    for c in cursor.get_children():
        if c.kind == CursorKind.CXX_METHOD:
            if filter_func and filter_func(c):
                classes.append(c)

    return classes

def find(cursor, kind, func):
    for c in cursor.get_children():
        if c.kind == kind:
            if func(c):
                return c
    return None

def find_or_insert(cursor, kind, match_func, equal_func = None):
    '''
    If there's a cursor satisfying the following two conditions:
        1) match_func(cursor) is True
        2) equal_func(cursor) is True
    If there's no such cursor, find the last cursor for which match_func(cursor)
    is True. In this case, we can insert our pattern after this cursor if we
    want to.
    '''
    last_match = None

    for c in cursor.get_children():
        if c.location.file.name != cursor.translation_unit.spelling:
            continue

        if c.kind == kind:
            if match_func(c):
                if equal_func and equal_func(c):
                    return (True, c)
                else:
                    last_match = c
            elif last_match:
                break

    return (False, last_match)

POD_CLASS_TEMPLATE = '''
class {name} {{
public:
    {name}() {{
        memeset(this, 0, sizeof(*this));
    }}

    {fields}
}};
'''
def copy_class(cursor, location, destcls):
    lines = open(cursor.location.file.name).readlines()
    cls_fields = ''
    for c in cursor.get_children():
        if c.kind == CursorKind.CXX_ACCESS_SPEC_DECL:
            access = c.access_specifier.name.lower()
        elif c.kind == CursorKind.FIELD_DECL and access == 'public':
            lineno = c.location.line - 1
            line = lines[lineno]
            comment = line[c.extent.end.column:].strip()
            cls_fields += '\n    {}'.format(comment)
            cls_fields += '    {}'.format(line[c.extent.start.column - 1:c.extent.end.column])

    insert_after_line(location.file.name, location.line,
            '\n' + POD_CLASS_TEMPLATE.format(name = destcls, fields = cls_fields))

def insert_after_line(filepath, lineno, content):
    if lineno > 0:
        for (i, line) in enumerate(fileinput.input(filepath, inplace = 1)):
            print(line, end = '')
            if i + 1 == lineno:
                print(content)
    else:
        with open(filepath, 'a') as f:
            f.write(content)

if __name__ == '__main__':
    import sys

    if len(sys.argv) < 2:
        print('usage: {} <dir>'.format(sys.argv[0]))
        sys.exit(1)

    get_netbeans_incdirs(sys.argv[1])
