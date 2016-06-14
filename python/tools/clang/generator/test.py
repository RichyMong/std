context ='''\
virtual void hello(const char* str,
                   int len);
'''

with open('test.txt', 'w') as f:
    f.write(context)
