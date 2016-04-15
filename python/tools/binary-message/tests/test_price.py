import asyncio
import logging
import collections
import emoney
import optparse
from config import *
from emoney import util, message

logging.basicConfig(format = '%(asctime)s %(name)-12s %(levelname)s %(message)s',
                datefmt = '%F %T',
                level = logging.INFO)

class ConcurrentCase(object):
    def __init__(self, messages):
        self.messages_to_send = messages
        self.loop = asyncio.get_event_loop()
        self.client = None
        self.last_push_tp = collections.defaultdict(int)
        self.price_time = collections.defaultdict(list)

    def print_result(self):
        import pdb; pdb.set_trace()
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
        self.client = c = emoney.net.Client(self.loop)
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

def test_messages(messages, options):
    messages_to_send = []
    for mid in sorted(messages):
        m = all_messages[mid]
        if hasattr(m, 'push_type'):
            if options.push:
                m.push_type = message.PUSH_TYPE_REGISTER
            else:
                m.push_type = message.PUSH_TYPE_ONCE
        messages_to_send.append(m)

    case = ConcurrentCase(messages_to_send)
    try:
        case.run(options.ip, options.port)
    except KeyboardInterrupt:
        case.print_result()

if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option('-m', '--message', action='store', type = str, dest='msg_id', default = '',
                    help ='message to be sent')
    # receive push, -p is ideal but we have --port option
    parser.add_option('-s', '--server', action='store', type = str, dest='server', default='88',
                      help='the server to use, 84/86/88, default 88')
    parser.add_option('-i', '--ip', action='store', type = str, dest='ip', default='',
                      help='the ip address of the server')
    parser.add_option('-p', '--port', action='store', type = int, dest='port', default=1862,
                      help='the port of the server, default 1862')
    parser.add_option('-r', '--receive-push', action='store_true', dest='push', default=False,
                      help='receive push message')
    parser.add_option('-c', '--count', action='store', type=int, dest='count', default=1,
                      help='concurrent count')

    options, args = parser.parse_args()
    messages = set()
    if options.msg_id:
        for mid in util.range_str_to_list(options.msg_id):
            messages.add(mid)
    else:
        messages = all_messages.keys()

    if options.ip: ip = options.ip
    else: options.ip = '202.104.236.{}'.format(options.server)

    test_messages(messages, options)
