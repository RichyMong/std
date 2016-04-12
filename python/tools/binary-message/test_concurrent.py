import asyncio
import socket
import errno
import logging
import collections
import sys
import emoney
import optparse
import random
from datetime import datetime,timedelta
from emoney import util, message

logging.basicConfig(format = '%(asctime)s %(name)-12s %(levelname)s %(message)s',
                datefmt = '%F %T',
                level = logging.INFO)

ResponseTime = collections.namedtuple('ResponseTime', ['min', 'max', 'avg'])

start_time = datetime.now() - timedelta(minutes=random.randint(5, 10))

msg_5501 = message.Request_5501()
msg_5501.stock_id = 'HK|00700'
msg_5501.extra_data = 1
msg_5501.fields = util.range_str_to_list('8-14,16,20,31,33-44,46,50')
msg_5501.date_time = int(start_time.strftime('%y%m%d%H%M')) # yyMMddHHmm
msg_5501.extra_fields = util.range_str_to_list('1-14')

msg_5502 = message.Request_5502()
msg_5502.pid = 1
msg_5502.sort_field = 10
msg_5502.sort_method = message.SORT_ORDER_DESC
msg_5502.start_pos = 0
msg_5502.req_num = random.randint(5, 10)
msg_5502.fields = util.range_str_to_list('3-5,9-13')
msg_5502.req_type = message.REQ_TYPE_SELF_STOCK
msg_5502.content = ['HK|00700','HK|00001','NASDAQ|AAPL']

msg_5503 = message.Request_5503()
msg_5503.stock_id = 'HK|00700'
msg_5503.fields = util.range_str_to_list('1-8')
msg_5503.data_pos = 1
msg_5503.date_time = int(start_time.strftime('%y%m%d%H%M')) # yyMMddHHmm
msg_5503.request_num = 5

msg_5504 = message.Request_5504()
msg_5504.market_code = 'HK|00700'

msg_5505 = message.Request_5505()

msg_5506 = message.Request_5506()
msg_5506.increment_id = 1
msg_5506.current_count = 0

msg_5508 = message.Request_5508()

msg_5509 = message.Request_5509()

msg_5510 = message.Request_5510()

msg_5511 = message.Request_5511()
msg_5511.stock_id = 'HK|00700'
msg_5511.version = 1
msg_5511.pid = 1
msg_5511.push_type = message.PUSH_TYPE_ONCE
msg_5511.extend_data = 0

msg_5512 = message.Request_5512()
msg_5512.pid = 1
msg_5512.push_type = message.PUSH_TYPE_ONCE
msg_5512.stock_id = 'HK|00700'
msg_5512.fields = [42,44,45,46]

msg_5513 = message.Request_5513()
msg_5513.pid = 1
msg_5513.push_type = message.PUSH_TYPE_ONCE
msg_5513.stock_id = 'HK|00700'
msg_5513.fields = util.range_str_to_list('1-8')
msg_5513.date_time = int(start_time.strftime('%y%m%d%H%M')) # yyMMddHHmm

msg_5514 = message.Request_5514()
msg_5514.pid = 1
msg_5514.push_type = message.PUSH_TYPE_ONCE
msg_5514.stock_id = 'HK|00700'
msg_5514.fields = util.range_str_to_list('1-8')
msg_5514.flag = 1
msg_5514.value = int(start_time.strftime('%H%M%S')) # HHmmss
msg_5514.nr_of_roots = random.randint(2, 10)

msg_5515 = message.Request_5515()
msg_5515.pid = 1
msg_5515.push_type = message.PUSH_TYPE_ONCE
msg_5515.stock_id = 'HK|00700'
msg_5515.number = 40

msg_5516 = message.Request_5516()
msg_5516.pid = 1
msg_5516.push_type = message.PUSH_TYPE_ONCE
msg_5516.sort_field = 10
msg_5516.sort_method = message.SORT_ORDER_DESC
msg_5516.start_pos = 0
msg_5516.req_num = random.randint(5, 10)
msg_5516.fields = util.range_str_to_list('3-5,9-13')
msg_5516.req_type = message.REQ_TYPE_SELF_STOCK
msg_5516.content = ['HK|00700','HK|00001','NASDAQ|AAPL']

msg_5517 = message.Request_5517()
msg_5517.pid = 1

msg_5518 = message.Request_5518()
msg_5518.pid = 1
msg_5518.md5 = '2CD843BA279539E84A498B189956E272'.encode()

all_messages = {
             5501 : msg_5501,
             5502 : msg_5502,
             5503 : msg_5503,
             5504 : msg_5504,
             5505 : msg_5505,
             5506 : msg_5506,
             5508 : msg_5508,
             5509 : msg_5509,
             5510 : msg_5510,
             5511 : msg_5511,
             5512 : msg_5512,
             5513 : msg_5513,
             5514 : msg_5514,
             5515 : msg_5515,
             5516 : msg_5516,
             5517 : msg_5517,
             5518 : msg_5518,
           }

CHECK_PERIOD = 2

class ConcurrentCase(object):
    def __init__(self, count, messages):
        self.count = count
        self.messages_to_send = messages
        self.failed_connection = 0
        self.closed_by_server = 0
        self.request_timestamp = {}
        self.loop = asyncio.get_event_loop()
        self.clients = set()
        self.round_trip = collections.defaultdict(list)
        self.completed_messages = {}


    def all_response_ok(self):
        return len(self.completed_messages) == len(self.request_timestamp)


    def print_result(self):
        request_cnt = len(self.request_timestamp)
        for k, v in  self.round_trip.items():
            if len(v) >= request_cnt:
                if k not in self.completed_messages:
                    logging.info('message {} completed, request {}, response {}'.format(
                            k, request_cnt, len(v)
                        )
                    )

                    v.sort()
                    avg = sum(v)/len(v)
                    self.completed_messages[k] = ResponseTime(v[0], v[-1], avg)
                    logging.info(('min time: {}ms, max time: {}ms, avg: {:.1f}ms').format(
                           v[0], v[-1], avg
                        )
                    )
            else:
                logging.info('message {}, request count: {}, response count: {}'.format(
                        k, len(self.request_timestamp), len(v)
                    )
                )

        if self.all_response_ok():
            logging.info('all messages response are received')

            if self.failed_connection:
                logging.info('failed connection: {}'.format(self.failed_connection))

            if self.closed_by_server:
                logging.info('closed before response: {}'.format(self.closed_by_server))

            for k,rs in self.completed_messages.items():
                logging.info(('message {}, min time: {}ms, max time: {}ms, avg: {:.1f}ms').format(
                       k, rs.min, rs.max, rs.avg
                    )
                )


    def check_response(self):
        if self.request_timestamp and not self.all_response_ok():
            self.print_result()

        if self.clients:
            self.loop.call_later(CHECK_PERIOD, self.check_response)
        else:
            self.loop.stop()


    def run(self, ip, port):
        for i in range(self.count):
            c = emoney.net.Client(self.loop)
            c.set_message_callback(self.handle_message)
            c.set_state_callback(self.state_callback)
            self.request_timestamp[c.cid] = util.get_milliseconds()
            self.clients.add(c.cid)
            c.connect(ip, port)

        self.loop.call_later(CHECK_PERIOD, self.check_response)
        try:
            self.loop.run_forever()
        except Exception as e:
            print(e)
            self.loop.stop()


    def handle_single(self, c, m):
        if not m.is_push_msg():
            duration = util.get_milliseconds() - self.request_timestamp[c.cid]
            self.round_trip[m.MSG_ID].append(duration)

        print(m)
        if not c.is_waiting_for_msg() or not any(
                (c.is_waiting_for_response(), util.is_trade_time())):
            c.close()


    def handle_message(self, c, m):
        if m.empty():
            logging.info('got empty {} message'.format(m.msg_id))
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
                if c.state == c.CONNECTING:
                    self.failed_connection += 1
                else:
                    self.closed_by_server += 1

            self.request_timestamp.pop(c.cid)
            self.clients.remove(c.cid)

        elif new_state == c.CONNECTED:
            c.send_message(*self.messages_to_send)

def test_messages(messages, ip, port = 1862):
    messages_to_send = []
    for mid in sorted(messages):
        m = all_messages[mid]
        if hasattr(m, 'push_type'):
            if options.push:
                m.push_type = message.PUSH_TYPE_REGISTER
            else:
                m.push_type = message.PUSH_TYPE_ONCE
        messages_to_send.append(m)

    case = ConcurrentCase(options.count, messages_to_send)
    case.run(ip, port)

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
    parser.add_option('-c', '--count', action='store_const', dest='count', default=1,
                      help='concurrent count')

    options, args = parser.parse_args()
    messages = set()
    if options.msg_id:
        for mid in util.range_str_to_list(options.msg_id):
            messages.add(mid)
    else:
        messages = all_messages.keys()

    if options.ip: ip = options.ip
    else: ip = '202.104.236.{}'.format(options.server)

    test_messages(messages, ip, options.port)
