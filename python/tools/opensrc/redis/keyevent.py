import redis
import re

r = redis.StrictRedis(password='test')
p = r.pubsub()
p.psubscribe('__keyspace@*:waipan:mds:*')


rep = re.compile(r'__keyspace@\d+__:(.+)')
for msg in p.listen():
    t = msg['type']
    if t == 'pmessage':
        c = msg['channel'].decode()
        m = rep.search(c)
        print(m.group(1))
        v = r.get(m.group(1))
        print(v)
