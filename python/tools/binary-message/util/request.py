import message


class RequestMessage(message.Message):
    fields_info = (
                      ('a', '<H'),
                      ('b', '<I'),
                      ('c', '*<B<H'),
                      ('d', 's'),
                      ('e', '*<Bs'),
                  )

    def __init__(self, **kwargs):
        super().__init__(**kwargs)


p = Message()
p.a = 0x1234
p.b = 0xffff5678
p.c = [1, 2, 5, 8]
p.d = '你好'
p.e = ['abcd', '什么', 'efg', '哈哈']
print(p.a, p.b, p.c, p.d, p.e)
v =  p.tohex()
p = Message.fromhex(v)
print(p.a, p.b, p.c, p.d, p.e)
p = Message()
