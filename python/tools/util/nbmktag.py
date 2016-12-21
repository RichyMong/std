#!/usr/bin/env python
import os
import sys
import subprocess
import argparse
import glob
import os.path as op
import xml.etree.ElementTree as ET
from datetime import datetime

SUPPORTED_EXTENSIONS = ('c', 'cpp', 'cc', 'h', 'hpp')

YCM_EXTRA_CONF_FILE = '.ycm_extra_conf.py'

YCM_CONF_TEMPLATE = '''
# This file is NOT licensed under the GPLv3, which is the license for the rest
# of YouCompleteMe.
#
# Here's the license text for this file:
#
# This is free and unencumbered software released into the public domain.
#
# Anyone is free to copy, modify, publish, use, compile, sell, or
# distribute this software, either in source code form or as a compiled
# binary, for any purpose, commercial or non-commercial, and by any
# means.
#
# In jurisdictions that recognize copyright laws, the author or authors
# of this software dedicate any and all copyright interest in the
# software to the public domain. We make this dedication for the benefit
# of the public at large and to the detriment of our heirs and
# successors. We intend this dedication to be an overt act of
# relinquishment in perpetuity of all present and future rights to this
# software under copyright law.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
# For more information, please refer to <http://unlicense.org/>

import os
import ycm_core

# These are the compilation flags that will be used in case there's no
# compilation database set (by default, one is not set).
# CHANGE THIS LIST OF FLAGS. YES, THIS IS THE DROID YOU HAVE BEEN LOOKING FOR.
flags = [
'-Wall',
'-Wextra',
'-Werror',
'-Wno-long-long',
'-Wno-variadic-macros',
'-fexceptions',
'-DNDEBUG',
# THIS IS IMPORTANT! Without a "-std=<something>" flag, clang won't know which
# language to use when compiling headers. So it will guess. Badly. So C++
# headers will be compiled as C headers. You don't want that so ALWAYS specify
# a "-std=<something>".
# For a C project, you would set this to something like 'c99' instead of
# 'c++11'.
'-std=c++11',
# ...and the same thing goes for the magic -x option which specifies the
# language that the files to be compiled are written in. This is mostly
# relevant for c++ headers.
# For a C project, you would set this to 'c' instead of 'c++'.
'-x',
'c++',
LOCAL_INCLUDE_FILE
'-isystem',
'/usr/include',
'-isystem',
'/usr/local/include',
]


# Set this to the absolute path to the folder (NOT the file!) containing the
# compile_commands.json file to use that instead of 'flags'. See here for
# more details: http://clang.llvm.org/docs/JSONCompilationDatabase.html
#
# You can get CMake to generate this file for you by adding:
#   set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )
# to your CMakeLists.txt file.
#
# Most projects will NOT need to set this to anything; you can just change the
# 'flags' list of compilation flags. Notice that YCM itself uses that approach.
compilation_database_folder = ''

if os.path.exists( compilation_database_folder ):
  database = ycm_core.CompilationDatabase( compilation_database_folder )
else:
  database = None

SOURCE_EXTENSIONS = [ '.cpp', '.cxx', '.cc', '.c', '.m', '.mm' ]

def DirectoryOfThisScript():
  return os.path.dirname( os.path.abspath( __file__ ) )


def MakeRelativePathsInFlagsAbsolute( flags, working_directory ):
  if not working_directory:
    return list( flags )
  new_flags = []
  make_next_absolute = False
  path_flags = [ '-isystem', '-I', '-iquote', '--sysroot=' ]
  for flag in flags:
    new_flag = flag

    if make_next_absolute:
      make_next_absolute = False
      if not flag.startswith( '/' ):
        new_flag = os.path.join( working_directory, flag )

    for path_flag in path_flags:
      if flag == path_flag:
        make_next_absolute = True
        break

      if flag.startswith( path_flag ):
        path = flag[ len( path_flag ): ]
        new_flag = path_flag + os.path.join( working_directory, path )
        break

    if new_flag:
      new_flags.append( new_flag )
  return new_flags


def IsHeaderFile( filename ):
  extension = os.path.splitext( filename )[ 1 ]
  return extension in [ '.h', '.hxx', '.hpp', '.hh' ]


def GetCompilationInfoForFile( filename ):
  # The compilation_commands.json file generated by CMake does not have entries
  # for header files. So we do our best by asking the db for flags for a
  # corresponding source file, if any. If one exists, the flags for that file
  # should be good enough.
  if IsHeaderFile( filename ):
    basename = os.path.splitext( filename )[ 0 ]
    for extension in SOURCE_EXTENSIONS:
      replacement_file = basename + extension
      if os.path.exists( replacement_file ):
        compilation_info = database.GetCompilationInfoForFile(
          replacement_file )
        if compilation_info.compiler_flags_:
          return compilation_info
    return None
  return database.GetCompilationInfoForFile( filename )


def FlagsForFile( filename, **kwargs ):
  if database:
    # Bear in mind that compilation_info.compiler_flags_ does NOT return a
    # python list, but a "list-like" StringVec object
    compilation_info = GetCompilationInfoForFile( filename )
    if not compilation_info:
      return None

    final_flags = MakeRelativePathsInFlagsAbsolute(
      compilation_info.compiler_flags_,
      compilation_info.compiler_working_dir_ )

    # NOTE: This is just for YouCompleteMe; it's highly likely that your project
    # does NOT need to remove the stdlib flag. DO NOT USE THIS IN YOUR
    # ycm_extra_conf IF YOU'RE NOT 100% SURE YOU NEED IT.
    try:
      final_flags.remove( '-stdlib=libc++' )
    except ValueError:
      pass
  else:
    relative_to = DirectoryOfThisScript()
    final_flags = MakeRelativePathsInFlagsAbsolute( flags, relative_to )

  return {
    'flags': final_flags,
    'do_cache': True
  }
'''

class NoSourceError(IOError):
    pass

class ProjectProcessor(object):
    def __init__(self, path):
        assert op.isdir(path)

        self.path = op.abspath(path)
        self.file_cnt = 0
        self.nbcfg = op.join(self.path, 'nbproject', 'configurations.xml')
        prj_src = op.join(self.path, 'src')
        if not op.exists(prj_src):
            prj_src = op.join(self.path, op.basename(self.path))
            if not op.exists(prj_src):
                prj_src = self.path
        self.prj_src = prj_src
        self.src_dirs = [ prj_src ]
        self.inc_dirs = []

    def add_dir_files(self, fp, dirpath):
        for fpath in glob.iglob('{}/**/*'.format(dirpath), recursive=True):
            ext = op.splitext(fpath)
            if len(ext) > 1 and ext[1][1:] in SUPPORTED_EXTENSIONS:
                fp.write(op.join(dirpath, fpath)  + '\n')
                self.file_cnt += 1

    def make_ycm_extra_conf(self):
        local_included = ''
        for dirpath in self.src_dirs:
            local_included += "'-I',\n'{}',\n".format(
                    op.relpath(dirpath, self.path))

        with open(op.join(self.path, YCM_EXTRA_CONF_FILE), 'w') as fp:
            fp.write(YCM_CONF_TEMPLATE.replace('LOCAL_INCLUDE_FILE', local_included[:-1]))

    def get_inc_dirs(self):
        # The caller ensures the file exists
        tree = ET.parse(self.nbcfg)
        for conf in tree.findall('confs/conf'):
            if conf.attrib['name'].startswith('Debug'):
                return [ op.join(self.path, elem.text) for elem in conf.findall(
                                  'compileType/ccTool/incDir/pElem') ]
        return []

    def get_preprocessors(self):
        for conf in ET.parse(self.nbcfg).findall('confs/conf'):
            if conf.attrib['name'] == 'Debug':
                return [ elem.text for elem in conf.findall(
                            'compileType/ccTool/preprocessorList/Elem') ]
        return []

    def make_tags(self):
        files = op.join(self.prj_src, datetime.now().strftime('srcfile_%Y%m%d'))

        with open(files, 'w') as fp:
            for dirpath in self.src_dirs:
                self.add_dir_files(fp, op.abspath(dirpath))

            fp.flush()

            if self.file_cnt:
                old = os.getcwd()
                os.chdir(self.prj_src)

                subprocess.call(['ctags', '-L', files])
                subprocess.call(['cscope', '-bqu', '-i', files])

                os.chdir(old)

    def run(self):
        self.src_dirs.extend(self.get_inc_dirs())
        self.make_tags()
        self.make_ycm_extra_conf()

def process_project(path):
    if not op.isdir(path):
        return

    print('Processing', path)
    try:
        ProjectProcessor(path).run()
    except NoSourceError as ie:
        print(ie)

if __name__ == '__main__':
    cwd = os.getcwd()
    default_src = os.path.join(cwd, 'src')
    parser = argparse.ArgumentParser(description='create YCM conf file and' +
                                     ' tags from netbeans project')
    parser.add_argument('-v', '--verbosity', action='count', default=2,
                        help='print verbose')
    parser.add_argument('-r', '--recursive', action='store_true',
                        help='process all the subdirectories')
    parser.add_argument('dirs', type=str, nargs='+', default=[os.getcwd()],
                         help='projects directories')
    ns = parser.parse_args(args=sys.argv[1:])
    if ns.recursive:
        for d in ns.dirs:
            for x in os.listdir(op.abspath(d)):
                process_project(x)
    else:
        for d in ns.dirs:
            process_project(d)
