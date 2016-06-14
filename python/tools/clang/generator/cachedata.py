import importlib
import util
import re
from baseunit import BaseUnit
from clang.cindex import CursorKind, TypeKind
from jinja2 import Environment

update_method_tmpl = Environment().from_string('''\
{{indent}}CAutoLockWrite lock(m_rwlock);
{{indent}}{{cache_name}} old = {{member}};
{{indent}}Convert(*pData, {{member}});
{{indent}}_Notify({{reg_type}}, m_dict.m_pchCode, m_dict.m_cMarket, &{{member}});

{{indent}}return true;
''')

class MultiCache(object):
    def __init__(self, config = 'config'):
        self.config = importlib.import_module(config)
        self.def_unit = BaseUnit('multicachedata', 'stdstructdef.h', self.config.root)
        self.mcd_unit = BaseUnit('multicachedata', 'multicachedata.h', self.config.root)

    def add_new_class(self, name):
        if name.startswith(self.config.STD_CLASS_PREFIX):
            cache_class_name = self.config.CACHE_CLASS_PREFIX + \
                    name[len(self.config.STD_CLASS_PREFIX):]
            srccursor = self.copy_stdqt_class(name, cache_class_name)
            self.add_to_embeded_class(srccursor, cache_class_name)
            self.add_notify_method(cache_class_name)
            self.add_mds_staff(cache_class_name)
        else:
            print('class name "{}" does not start with "{}"'.format(name,
                    MultiCache.STD_CLASS_PREFIX))

    def copy_stdqt_class(self, name, dstname):
        stdunit = BaseUnit('multicompress', 'StdQtStruct.h', self.config.root)
        cursor = stdunit.find_class_cursor(name)
        if not cursor:
            raise RuntimeError('cannot find class "{}" in file "{}"'.format(
                      name, stdunit.filepath))

        exists, target = util.find_or_insert(self.def_unit.cursor,
                            CursorKind.CLASS_DECL,
                            lambda c : self.is_cache_class(c.spelling),
                            lambda c : c.spelling == dstname)
        if exists:
            print('Class "{}" already exists in file "{}" at line {}'.format(
                    dstname, target.translation_unit.spelling, target.location.line
                ))
        else:
            util.copy_class(cursor, target.extent.end, dstname)

        return cursor

    def _add_reg_type(self):
        cursor = self.mcd_unit.find(CursorKind.ENUM_DECL,
                    lambda c : c.spelling == 'DataModelType')
        if cursor:
            exist, pos = util.find_or_insert(
                cursor,
                CursorKind.ENUM_CONSTANT_DECL,
                lambda c : True,
                lambda c : c.spelling == self.config.REG_TYPE
            )
            if not exist:
                util.insert_after_line(self.mcd_unit.filepath, pos.extent.end.line,
                        util.INDENT + self.config.REG_TYPE + ',')
        else:
            print('Error: cannot find enum "{}" in file {}'.format(
                self.config.embed_class, self.mcd_unit.filepath)
            )

    def add_to_embeded_class(self, srccursor, cache_class_name):
        impl_class = self.config.embed_impl[1]
        impl_header = BaseUnit('multicachedata',
                               self.config.embed_impl[0]+ '.h',
                               self.config.root)
        impl_cpp = BaseUnit('multicachedata',
                            self.config.embed_impl[0]+ '.cpp',
                            self.config.root)
        def add_convert_method(srccursor):
            method_name = 'Convert'
            result_type = 'void'
            args = [ ('const {} &'.format(srccursor.spelling), 'src'),
                    ('{} &'.format(cache_class_name), 'dest') ]

            impl_header.add_method_declaration(
                impl_class, method_name, result_type, args
            )

            body = ''
            for c in srccursor.get_children():
                if c.kind == CursorKind.FIELD_DECL:
                    body += util.INDENT
                    if c.type.kind == TypeKind.CONSTANTARRAY:
                        body += 'memcpy(dest.{0}, src.{0}, sizeof(dest.{0}));\n'.format(c.spelling)
                    else:
                        body += 'dest.{0} = src.{0};\n'.format(c.spelling)

            impl_cpp.add_method_definitaion(
                impl_class, method_name, result_type, args, body = body
            )

        def add_update_method(data_type):
            method_name = 'Update'
            result_type = 'bool'
            args = [ ('const {} *'.format(data_type), 'data') ]
            impl_header.add_method_declaration(
                impl_class, method_name, result_type, args, func_type = True
            )

            body = ''
            if self.config.embed_impl[0] == 'stddataunitimp':
                body = update_method_tmpl.render(indent = util.INDENT,
                        cache_name = cache_class_name, reg_type = self.config.REG_TYPE,
                        member = self.field_name(cache_class_name)
                    )

            impl_cpp.add_method_definitaion(
                impl_class, method_name, result_type, args, body = body)

        if not self.config.is_embeded:
            return

        cls_cursor = self.mcd_unit.find_class_cursor(self.config.embed_class)
        if not cls_cursor:
            print('Error: cannot find class "{}"'.format(self.config.embed_class))
            return

        # Add field declaration into the embeded class
        exist, cursor = util.find_or_insert(cls_cursor, CursorKind.FIELD_DECL,
                            lambda c : self.is_cache_class(c.type.spelling),
                            lambda c : c.type.spelling == cache_class_name)
        if exist:
            print('Error: "{}" already has field "{}" of type "{}" at line {}'.format(
                   self.config.embed_class, cursor.spelling, cache_class_name,
                   cursor.location.line))
        else:
            util.insert_after_line(self.mcd_unit.filepath, cursor.extent.end.line,
                    util.INDENT + self.field_decl(cache_class_name))

        self._add_reg_type()
        add_convert_method(srccursor)
        add_update_method(srccursor.spelling)

    def add_notify_method(self, cache_class_name):
        method_name = 'Notify'
        result_type = 'void'
        args = [ ('const vector<SCltRegInfo> &', 'info') ]
        if self.config.notify.stockid:
            args.append(('const char *', 'stockID'))
        if self.config.notify.market:
            args.append(('unsigned short', 'marketID'))
        args.append(('const {} *'.format(cache_class_name), 'data'))
        args.append(('const {} *'.format(cache_class_name), 'old'))

        self.mcd_unit.add_method_declaration(
            'CDataNotify', method_name, result_type, args,
            func_type = util.PURE_VIRTUAL
        )

        unit = BaseUnit('multicachedata', 'notifytransfer.h', self.config.root)
        unit.add_method_declaration(
            'CNotifyTransfrer', method_name, result_type, args,
            func_type = util.VIRTUAL
        )

    def add_mds_staff(self, cache_class_name):
        ph_unit = BaseUnit('multidatasvr', 'process.h', self.config.root)
        pcpp_unit = BaseUnit('multidatasvr', 'process.cpp', self.config.root)

        if self.config.stock_data:
            macro_name = 'PACK_PC_STK_{}'.format(self.config.DATA_MODEL)
        else:
            macro_name = 'PACK_PC_{}'.format(self.config.DATA_MODEL)

        same, def_cursor = ph_unit.find_or_insert(CursorKind.MACRO_DEFINITION,
                                lambda c : c.spelling.startswith('PACK_PC'),
                                lambda c : c.spelling == macro_name
                           )
        if not same:
            # TODO: libclang does not find macro definition at now.
            last_macro = () # 4-elements tuple(line, length, name, value)
            pattern = re.compile(r'#define +(PACK_PC_\w+) +(\d+)')
            with open(ph_unit.filepath) as f:
                for i, line in enumerate(f):
                    m = pattern.search(line)
                    if m:
                        last_macro = (i, m.group(1), m.group(2))
                        if m.group(2) == macro_name:
                            break

            if last_macro[1] == macro_name:
                print('Macro "{}" is already there'.format(macro_name))
            else:
                text = '#define {:<{}} {}'.format(macro_name, len(last_macro[1]),
                        int(last_macro[2]) + 1)
                util.insert_after_line(ph_unit.filepath, last_macro[0] + 1, text)

        # add the case statement to process.cpp
        cursor = pcpp_unit.find(CursorKind.CXX_METHOD,
                lambda c : c.spelling == '_ProcReq')
        if cursor:
            for c in cursor.get_children():
                if c.kind == CursorKind.COMPOUND_STMT:
                    for c in cursor.get_children():
                        print(c.kind, c.spelling)

    def field_decl(self, cache_class_name):
        return cache_class_name + ' ' + MultiCache.field_name(cache_class_name)  + ';'

    def field_name(self, cache_class_name):
        name = cache_class_name[len(self.config.CACHE_CLASS_PREFIX):]
        return 'm_' + name[0].lower() + name[1:]

    def is_cache_class(self, class_name):
        return class_name.startswith(self.config.CACHE_CLASS_PREFIX)
