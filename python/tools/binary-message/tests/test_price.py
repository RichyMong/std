import asyncio
import logging
import collections
import ouou
import optparse
import config
from ouou import util, message

logging.basicConfig(format = '%(asctime)s %(name)-12s %(levelname)s %(message)s',
                datefmt = '%F %T',
                level = logging.INFO)

class ConcurrentCase(object):
    def __init__(self, messages):
        self.messages_to_send = list(messages)
        self.loop = asyncio.get_event_loop()
        self.client = None
        self.last_push_tp = collections.defaultdict(int)
        self.price_time = collections.defaultdict(list)

    def print_result(self):
        count = max(len(x) for x in self.price_time.values())
        width = 20
        for i in range(count):
            line = ''
            for x in sorted(self.price_time.keys()):
                v = self.price_time[v]
                if len(v) < count:
                    s = ''
                else:
                    s = '{:.3f}({})'.format(v[i][0], v[i][1])

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
        if m.is_push_msg():
            price = 0
            if m.msg_id == 5512:
                price = m.latest_price
            elif m.msg_id == 5513:
                price = m.data.return_data[0].close_price
            else:
                price = m.return_data[0].price

        if self.price_time[m.msg_id]:
            d = util.get_milliseconds() - self.last_push_tp[m.msg_id]
            self.price_time[m.msg_id].append((price, d))
        else:
            self.last_push_tp[m.msg_id] = util.get_milliseconds()

    def handle_message(self, c, m):
        if m.empty():
            return
        if m.is_multiple_message():
            for message in m:
                self.handle_single(c, message)
        else:
            self.handle_single(c, m)

    def state_callback(self, c, new_state):
        if new_state == c.CLOSED:
            if c.state != c.CLOSING:
                logging.info('{} closed by remote'.format(c))

        elif new_state == c.CONNECTED:
            c.send_message(*self.messages_to_send)

if __name__ == '__main__':
    import sys
    ns = config.parse_args(sys.argv[1:], receive_push=True)

    case = ConcurrentCase(ns.message.values())
    try:
        case.run(ns.address, ns.port)
    except KeyboardInterrupt:
        case.print_result()
