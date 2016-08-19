from __future__ import print_function

global_env = { 'ABCDE' : 100 }
local_env = {}

my_code = """
import sys
class A(object):
  pass

print('locals: %s' % locals())
print('localsA: %s' % locals()['A'])
print('A: %s' % A)
globals()['A'] = A
locals()['ABCDE'] = 1000
print(locals() is sys.modules['__main__'].local_env)

class B(object):
  import sys
  print(locals() is sys.modules['__main__'].local_env)
  a_ref = globals()['A']
"""

def scope():
    import sys
    print(locals() is sys.modules['__main__'].local_env, globals() is sys.modules['__main__'].global_env)

my_code_AST = compile(my_code, "My Code", "exec")
exec(my_code_AST, global_env, local_env)
print(type(my_code_AST), my_code_AST)
exec("print(locals() is sys.modules['__main__'].local_env, globals() is sys.modules['__main__'].global_env)", global_env, local_env)
exec(scope.__code__, global_env, local_env)

print(local_env)
