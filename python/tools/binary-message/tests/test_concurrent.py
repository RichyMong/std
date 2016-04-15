import asyncio
import logging
import collections
import emoney
import optparse
import config
from emoney import util, message

logging.basicConfig(format = '%(asctime)s %(name)-12s %(levelname)s %(message)s',
                datefmt = '%F %T',
                level = logging.INFO)

ResponseTime = collections.namedtuple('ResponseTime', ['min', 'max', 'avg'])

CHECK_PERIOD = 2

class ConcurrentCase(object):
    def __init__(self, count, messages):
        self.count = count
        self.messages_to_send = messages
        self.failed_connection = 0
        self.loop = asyncio.get_event_loop()
        self.clients = set()
        self.connection_tp = {}
        self.failed_response = collections.defaultdict(int)
        self.request_tp = collections.defaultdict(dict)
        self.response_tp = collections.defaultdict(dict)

    def print_progress(self):
        pass

    def print_result(self):
        if self.failed_connection:
            logging.info('failed connection: {}'.format(self.failed_connection))

        completed = []
        for msg in self.messages_to_send:
            k = msg.msg_id
            if len(self.connection_tp) == (self.failed_connection +
                    self.failed_response[k] + len(self.response_tp[k])):
                completed.append(k)

        if len(completed) != len(self.messages_to_send):
            logging.info('message complete percent [{}/{}]'.format(
                     len(completed), len(self.messages_to_send)))

        for (i, k) in enumerate(completed):
            logging.info(80 * '-')
            total_trip, once_trip = [], []
            req = self.request_tp[k]
            resp = self.response_tp[k]
            r = 'message {}, request made: {}, response received: {}'.format(
                        k, len(req), len(resp))
            if k in self.failed_response:
                r += ', failed: {}'.format(self.failed_response[k])
            logging.info(r)

            for (cid, tp) in resp.items():
                total_trip.append(tp - self.connection_tp[cid])
                once_trip.append(tp - req[cid])
            total_trip.sort()
            once_trip.sort()
            avg = sum(total_trip)/len(total_trip)
            logging.info(('receive-connect: min time: {}ms, max time: {}ms, avg: {:.1f}ms').format(
                   total_trip[0], total_trip[-1], avg
                )
            )
            avg = sum(once_trip)/len(once_trip)
            logging.info(('receive-send: min time: {}ms, max time: {}ms, avg: {:.1f}ms').format(
                   once_trip[0], once_trip[-1], avg
                )
            )

    def check_response(self):
        if self.clients:
            self.print_progress()
            self.loop.call_later(CHECK_PERIOD, self.check_response)
        else:
            self.print_result()
            self.loop.stop()

    def run(self, ip, port):
        for i in range(self.count):
            c = emoney.net.Client(self.loop)
            c.set_message_callback(self.handle_message)
            c.set_state_callback(self.state_callback)
            self.connection_tp[c.cid] = util.get_milliseconds()
            self.clients.add(c.cid)
            c.connect((ip, port))

        self.loop.call_later(CHECK_PERIOD, self.check_response)
        try:
            self.loop.run_forever()
        except Exception as e:
            print(e)
            self.loop.stop()

    def handle_single(self, c, m):
        if not m.is_push_msg():
            self.response_tp[m.msg_id][c.cid] = util.get_milliseconds()

        if len(self.connection_tp) == 1:
            print(m)
        if not c.is_waiting_for_msg() or not any(
                (c.is_waiting_for_response(), util.is_trade_time())):
            c.close()

    def handle_message(self, c, m):
        if m.is_multiple_message():
            for message in m:
                self.handle_single(c, message)
        else:
            self.handle_single(c, m)

    def state_callback(self, c, new_state):
        if new_state == c.CLOSED:
            if c.state != c.CLOSING:
                logging.info('{} closed by remote'.format(c))
                if c.state == c.CONNECTING:
                    self.failed_connection += 1
                else:
                    for mid, r in self.request_tp.items():
                        if c.cid in r:
                            r.pop(c.cid)
                            self.failed_response[mid] += 1

            self.clients.remove(c.cid)

        elif new_state == c.CONNECTED:
            for m in self.messages_to_send:
                self.request_tp[m.msg_id][c.cid] = util.get_milliseconds()
            c.send_message(*self.messages_to_send)

def test_messages(messages, options):
    messages_to_send = []
    for mid in sorted(messages):
        m = config.all_messages[mid]
        if hasattr(m, 'push_type'):
            if options.push:
                m.push_type = message.PUSH_TYPE_REGISTER
            else:
                m.push_type = message.PUSH_TYPE_ONCE
        messages_to_send.append(m)

    case = ConcurrentCase(options.count, messages_to_send)
    try:
        case.run(options.ip, options.port)
    except KeyboardInterrupt:
        case.print_result()

if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option('-m', '--message', action='store', type = str, dest='msg_id',
                      default = '', help ='message to be sent')
    # receive push, -p is ideal but we have --port option
    parser.add_option('-s', '--server', action='store', type = str, dest='server',
                      default='88', help='the server to use, 84/86/88, default 88')
    parser.add_option('-n', '--stockid', action='store', type = str, dest='stock_id', default='',
                      help='the stock to use, random if not set')
    parser.add_option('-i', '--ip', action='store', type = str, dest='ip', default='',
                      help='the ip address of the server')
    parser.add_option('-p', '--port', action='store', type = int, dest='port', default=1862,
                      help='the port of the server, default 1862')
    parser.add_option('-r', '--receive-push', action='store_true', dest='push', default=False,
                      help='receive push message')
    parser.add_option('-c', '--count', action='store', type=int, dest='count', default=1,
                      help='concurrent count')

    options, args = parser.parse_args()
    config.use_stock(options.stock_id)

    messages = set()
    if options.msg_id:
        for mid in util.range_str_to_list(options.msg_id):
            messages.add(mid)
    else:
        messages = config.all_messages.keys()

    if options.ip: ip = options.ip
    else: options.ip = '202.104.236.{}'.format(options.server)

    test_messages(messages, options)
