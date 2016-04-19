import sys
import asyncio
import logging
import collections
import ouou
import optparse
import config
from itertools import zip_longest
from ouou import util, message

logging.basicConfig(format = '%(asctime)s %(name)-12s %(levelname)s %(message)s',
                datefmt = '%F %T',
                level = logging.INFO)

class ConcurrentCase(object):
    def __init__(self):
        config.request_5512.fields = [55]
        config.request_5513.fields = [5]
        config.request_5514.fields = [3]
        config.request_5512.push_type = message.PUSH_TYPE_REGISTER
        config.request_5513.push_type = message.PUSH_TYPE_REGISTER
        config.request_5514.push_type = message.PUSH_TYPE_REGISTER
        self.messages_to_send = [config.request_5512, config.request_5513, config.request_5514]
        self.loop = asyncio.get_event_loop()
        self.client = None
        self.last_push_tp = None
        self.price_time = collections.defaultdict(list)

    def print_result(self):
        if not self.price_time:
            return
        count = max(len(x) for x in self.price_time.values())
        width = 20
        keys = list(sorted(self.price_time.keys()))
        print(' '.join('{:<{}}'.format(k, width) for k in keys))
        values = tuple(self.price_time[k] for k in keys)
        filled = (0, 0)
        for x in zip_longest(*values, fillvalue = filled):
            line = ''
            for v in x:
                if v != filled:
                    s = '{:.1f}({})'.format(v[0], v[1])
                else:
                    s = ' '
                line += '{:{}s}'.format(s, width)
            print(line)

    def run(self, ip, port):
        self.client = c = ouou.net.Client(self.loop)
        c.set_message_callback(self.handle_message)
        c.set_state_callback(self.state_callback)
        c.connect((ip, port))

        try:
            self.loop.run_forever()
        except Exception as e:
            print(e)
            self.loop.stop()

    def handle_single(self, c, m):
        if m.msg_id == 5517:
            assert False
            return

        price = 0
        if not m.is_push_msg():
            return

        if m.msg_id == 5512:
            price = m.data[0]
        elif m.msg_id == 5513:
            price = m.data.data[0][0]
        elif m.msg_id == 5514:
            price = m.data[0][0]
        else:
            raise RuntimeError('unexpected message')
        print(m.msg_id, price)

        if self.last_push_tp is None:
            self.last_push_tp = util.get_milliseconds()
        else:
            d = util.get_milliseconds() - self.last_push_tp
            self.price_time[m.msg_id].append((price / 1000, d))

    def handle_message(self, c, m):
        if m.empty():
            return
        if m.is_multiple_message():
            for message in m:
                self.handle_single(c, message)
        else:
            self.handle_single(c, m)

        if self.last_push_tp and util.get_milliseconds() - self.last_push_tp > 60000:
            self.print_result()
            sys.exit(0)

    def state_callback(self, c, new_state):
        if new_state == c.CLOSED:
            if c.state != c.CLOSING:
                logging.info('{} closed by remote'.format(c))

        elif new_state == c.CONNECTED:
            print('connected to server')
            c.send_message(*self.messages_to_send)

if __name__ == '__main__':
    ns = config.parse_args(sys.argv[1:], receive_push=True)

    case = ConcurrentCase()
    try:
        case.run(ns.address, ns.port)
    except KeyboardInterrupt:
        case.print_result()
