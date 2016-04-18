import asyncio
import logging
import collections
import argparse
import config
import emoney
from emoney import util, message

logging.basicConfig(format = '%(asctime)s %(name)-12s %(levelname)s %(message)s',
                datefmt = '%F %T',
                level = logging.INFO)

ResponseTime = collections.namedtuple('ResponseTime', ['min', 'max', 'avg'])

CHECK_PERIOD = 2

class ConcurrentCase(object):
    def __init__(self, count, messages):
        self.loop = asyncio.get_event_loop()
        self.clients = set()
        self.count = count
        self.failed_connection = 0
        self.messages_to_send = list(messages)
        self.connection_tp = {}
        self.failed_response = collections.defaultdict(int)
        self.request_tp = collections.defaultdict(dict)
        self.response_tp = collections.defaultdict(dict)

    def print_progress(self):
        pass

    def print_result(self):
        if self.count <= 1:
            return

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

        fmt = '{:<8} {:<8} {:<8} {:<8}'
        logging.info(fmt.format('message', 'request', 'response', 'failed'))

        total_summary = []
        once_summary = []

        for (i, k) in enumerate(completed):
            total_trip, once_trip = [], []
            req = self.request_tp[k]
            resp = self.response_tp[k]
            logging.info(fmt.format(k, len(req), len(resp), self.failed_response[k]))

            for (cid, tp) in resp.items():
                total_trip.append(tp - self.connection_tp[cid])
                once_trip.append(tp - req[cid])
            total_trip.sort()
            once_trip.sort()
            total_summary.append((total_trip[0], total_trip[-1],
                                sum(total_trip)/len(total_trip)))
            once_summary.append((once_trip[0], once_trip[-1],
                                sum(once_trip)/len(once_trip)))

        fmt = '{:<7} {:<6} {:<6} {:<6} {:<6} {:<6} {:<6}'
        logging.info(fmt.format('message', 'crmin', 'crmax', 'cravg', 'rrmin',
                                'rrmax', 'rravg'))
        for (i, (tv, ov)) in enumerate(zip(total_summary, once_summary)):
            logging.info(fmt.format(self.messages_to_send[i].msg_id,
                tv[0], tv[1], tv[2], ov[0], ov[1], ov[2]))
        logging.info("'cr': connect-response time, 'rr': request-response time")

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
                        if r.pop(c.cid, None) is not None:
                            self.failed_response[mid] += 1

            self.clients.remove(c.cid)

        elif new_state == c.CONNECTED:
            for m in self.messages_to_send:
                self.request_tp[m.msg_id][c.cid] = util.get_milliseconds()
            c.send_message(*self.messages_to_send)

def runtest(args, **kwargs):
    parser = config.create_parser('make concurrent requests')
    # receive push, -p is ideal but we have --port option
    parser.add_argument('-r', '--receive-push', action='store_true', default=False,
                        help='receive push message')
    parser.add_argument('-c', '--count', action='store', type=int, default=1,
                         help='concurrent count')
    ns = config.parse_args(args, parser = parser, **kwargs)
    case = ConcurrentCase(ns.count, ns.message.values())
    try:
        case.run(ns.address, ns.port)
    except KeyboardInterrupt:
        case.print_result()

if __name__ == '__main__':
    import sys
    runtest(sys.argv[1:])
