import asyncio
import socket
import errno
import logging
import collections
import sys
import ouou
import optparse
import random
from datetime import datetime,timedelta
from ouou import util, message

start_time = datetime.now() - timedelta(minutes=random.randint(5, 10))

msg_5501 = message.Request_5501()
msg_5501.stockid = 'HK|00700'
msg_5501.extra_data = 1
msg_5501.fields = util.range_str_to_list('8-14,16,20,31,33-44,46,50')
msg_5501.date_time = int(start_time.strftime('%y%m%d%H%M')) # yyMMddHHmm
msg_5501.extra_fields = util.range_str_to_list('1-14')

msg_5502 = message.Request_5502()
msg_5502.pid = 1
msg_5502.sort_field = 10
msg_5502.sort_method = util.SORT_ORDER_DESC
msg_5502.start_pos = 0
msg_5502.req_num = random.randint(5, 10)
msg_5502.fields = util.range_str_to_list('3-5,9-13')
msg_5502.req_type = util.REQ_TYPE_SELF_STOCK
msg_5502.content = ['HK|00700','HK|00001','NASDAQ|AAPL']

msg_5503 = message.Request_5503()
msg_5503.stockid = 'HK|00700'
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
msg_5511.stockid = 'HK|00700'
msg_5511.version = 1
msg_5511.pid = 1
msg_5511.push_type = util.PUSH_TYPE_ONCE
msg_5511.extend_data = 0

msg_5512 = message.Request_5512()
msg_5512.pid = 1
msg_5512.push_type = util.PUSH_TYPE_ONCE
msg_5512.stockid = 'HK|00700'
msg_5512.fields = [42,44,45,46]

msg_5513 = message.Request_5513()
msg_5513.pid = 1
msg_5513.push_type = util.PUSH_TYPE_ONCE
msg_5513.stockid = 'HK|00700'
msg_5513.fields = util.range_str_to_list('1-8')
msg_5513.date_time = int(start_time.strftime('%y%m%d%H%M')) # yyMMddHHmm

msg_5514 = message.Request_5514()
msg_5514.pid = 1
msg_5514.push_type = util.PUSH_TYPE_ONCE
msg_5514.stockid = 'HK|00700'
msg_5514.fields = util.range_str_to_list('1-8')
msg_5514.flag = 1
msg_5514.value = int(start_time.strftime('%H%M%S')) # HHmmss
msg_5514.nr_of_roots = random.randint(2, 10)

msg_5515 = message.Request_5515()
msg_5515.pid = 1
msg_5515.push_type = util.PUSH_TYPE_ONCE
msg_5515.stockid = 'HK|00700'
msg_5515.number = 40

msg_5516 = message.Request_5516()
msg_5516.pid = 1
msg_5516.push_type = util.PUSH_TYPE_ONCE
msg_5516.stockid = 'HK|00700'
msg_5516.number = 40

messages = {
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
             # 5517 : msg_5517,
             # 5518 : msg_5518,
           }

message_to_send = []

failed_connection = 0
closed_by_server = 0
request_start_time = {}
round_trip = collections.defaultdict(list)
CHECK_PERIOD = 2
waiting_for_push = False

clients = set()

logging.basicConfig(format = '%(asctime)s %(name)-12s %(levelname)s %(message)s',
                datefmt = '%F %T',
                level = logging.INFO)

def print_result():
    if failed_connection:
        logging.info('failed connection: {}'.format(failed_connection))


    if closed_by_server:
        logging.info('closed before response: {}'.format(closed_by_server))

    complete_cnt = 0
    for x, v in round_trip.items():
        if len(v) == len(request_start_time):
            logging.info('request {}, count: {}, response count: {}'.format(
                    x, len(request_start_time), len(v))
            )

            v.sort()
            avg = sum(v)/len(v)
            logging.info(('min time: {}ms, max time: {}ms, ' +
                   'average: {:.1f}ms').format(
                       v[0], v[-1], avg
                  ))
            complete_cnt += 1
    if complete_cnt == len(round_trip):
        global waiting_for_push
        waiting_for_push = True


def check_response(loop):
    if len(request_start_time) and not waiting_for_push:
        if len(round_trip) == len(request_start_time):
            print_result()
        else:
            logging.info('request count: {}, response count: {}'.format(len(request_start_time), len(round_trip)))

    if not clients:
        loop.stop()
    else:
        loop.call_later(CHECK_PERIOD, check_response, loop)


def run_case(concurrent):
    loop = asyncio.get_event_loop()

    for i in range(concurrent):
        c = ouou.net.Client(loop)
        c.set_message_callback(message_callback)
        c.set_state_callback(state_callback)
        request_start_time[c.cid] = util.get_milliseconds()
        clients.add(c.cid)
        c.connect('202.104.236.88', 1862)

    loop.call_later(CHECK_PERIOD, check_response, loop)
    # loop.call_later(180, stop_loop, loop)
    try:
        loop.run_forever()
    except Exception as e:
        print(e)
        loop.stop()


def message_callback(c, m):
    def handle_single(message):
        if not m.is_push_pkg():
            round_trip[m.msg_id].append(util.get_milliseconds() - request_start_time[c.cid])

        print(m)
        if not c.is_waiting_for_msg() or not any(
                (c.is_waiting_for_response(), util.is_trade_time())):
            c.close()

    if m.is_multiple_message():
        for message in m:
            handle_single(m)
    else:
        handle_single(m)


def state_callback(c, new_state):
    if new_state == c.CLOSED:
        if c.state != c.CLOSING:
            print('{} closed by remote'.format(c))
            if c.state == c.CONNECTING:
                global failed_connection
                failed_connection += 1
            else:
                global closed_by_server
                closed_by_server += 1

        request_start_time.pop(c.cid)
        clients.remove(c.cid)

    elif new_state == c.CONNECTED:
        c.send_message(*message_to_send)


if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option('-m', '--message', action='store', type = int, dest='msg_id', default = 0,
                    help ='message to be sent')
    parser.add_option('-p', '--push', action='store_true', dest='push', default=False,
                      help='receive push message')
    parser.add_option('-c', '--count', action='store_const', dest='count', default=1,
                      help='concurrent count')

    options, args = parser.parse_args()
    message_to_send = [messages[options.msg_id]] if options.msg_id else list(messages.values())
    if options.push:
        for m in message_to_send:
            if hasattr(m, push_type):
                m.push_type = util.PUSH_TYPE_REGISTER

    run_case(options.count)
