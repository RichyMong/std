import random
import argparse
from collections import OrderedDict
from datetime import datetime,timedelta

try:
    from emoney import util, message
except ImportError:
    import sys
    import os
    p = os.path.realpath(os.path.join(os.path.dirname(__file__), '..'))
    if p not in sys.path:
        sys.path.append(p)
    from emoney import util, message

SERVER = ('202.104.236.88', 1862)

SERVER_VERSION = 20160418

start_time = datetime.now() - timedelta(minutes=random.randint(5, 10))

# we will choose a stock for a request randomly from these.
sample_stocks = [ 'HK|00700', 'HK|00001', 'NASDAQ|AAPL', 'HK|00941', 'NASDAQ|BIDU']

request_5501 = message.Request_5501()
request_5501.stock_id = random.choice(sample_stocks)
request_5501.request_extra = 1
request_5501.fields = util.range_str_to_list('1-14,16,20,31,33-44,46,50')
request_5501.date_time = int(start_time.strftime('%y%m%d%H%M')) # yyMMddHHmm
request_5501.extra_fields = util.range_str_to_list('1-14')

# Be sure to add 2 (stock_id) in the fields list since price representation
# differs by markets. For example, HK has three digits but USA has two.
request_5502 = message.Request_5502()
request_5502.pid = 1
request_5502.sort_field = 4
request_5502.sort_method = message.SORT_ORDER_DESC
request_5502.start_pos = 0
request_5502.req_num = random.randint(5, 10)
request_5502.fields = util.range_str_to_list('1-5,9-13')
request_5502.req_type = message.REQ_TYPE_SELF_STOCK
request_5502.content = ['HK|00700','HK|00001', 'NASDAQ|AAPL']

request_5503 = message.Request_5503()
request_5503.stock_id = random.choice(sample_stocks)
request_5503.fields = util.range_str_to_list('1-8')
request_5503.data_pos = 1
request_5503.date_time = int(start_time.strftime('%y%m%d%H%M')) # yyMMddHHmm
request_5503.request_num = 5

request_5504 = message.Request_5504()
request_5504.market_code = 'NASDAQ|AAPL'

request_5505 = message.Request_5505()

request_5506 = message.Request_5506()
request_5506.increment_id = 1
request_5506.current_count = 0

request_5508 = message.Request_5508()

request_5509 = message.Request_5509()

request_5510 = message.Request_5510()

request_5511 = message.Request_5511()
request_5511.stock_id = random.choice(sample_stocks)
request_5511.version = 1
request_5511.pid = 1
request_5511.push_type = message.PUSH_TYPE_ONCE
request_5511.extend_data = 0

request_5512 = message.Request_5512()
request_5512.pid = 1
request_5512.push_type = message.PUSH_TYPE_ONCE
request_5512.stock_id = random.choice(sample_stocks)
request_5512.fields = [52]

request_5513 = message.Request_5513()
request_5513.pid = 1
request_5513.push_type = message.PUSH_TYPE_ONCE
request_5513.stock_id = random.choice(sample_stocks)
request_5513.fields = util.range_str_to_list('1-8')
request_5513.date_time = int(start_time.strftime('%y%m%d%H%M')) # yyMMddHHmm

request_5514 = message.Request_5514()
request_5514.pid = 1
request_5514.push_type = message.PUSH_TYPE_ONCE
request_5514.stock_id = random.choice(sample_stocks)
request_5514.fields = util.range_str_to_list('1-8')
request_5514.flag = message.REQ_FLAG_START_TIME
request_5514.value = int(start_time.strftime('%H%M%S')) # HHmmss
request_5514.nr_of_roots = random.randint(2, 10)

request_5515 = message.Request_5515()
request_5515.pid = 1
request_5515.push_type = message.PUSH_TYPE_ONCE
request_5515.stock_id = random.choice(sample_stocks)
request_5515.number = 40

request_5516 = message.Request_5516()
request_5516.pid = 1
request_5516.push_type = message.PUSH_TYPE_ONCE
request_5516.sort_field = 10
request_5516.sort_method = message.SORT_ORDER_DESC
request_5516.start_pos = 0
request_5516.req_num = random.randint(5, 10)
request_5516.fields = util.range_str_to_list('1-5,9-13')
request_5516.req_type = message.REQ_TYPE_SELF_STOCK
request_5516.content = ['HK|00700','HK|00001','NASDAQ|AAPL']

request_5517 = message.Request_5517()
request_5517.pid = 1

request_5518 = message.Request_5518()
request_5518.pid = 1
request_5518.md5 = '2CD843BA279539E84A498B189956E272'

all_messages = {
     5501 : request_5501,
     5502 : request_5502,
     5503 : request_5503,
     5504 : request_5504,
     5505 : request_5505,
     5506 : request_5506,
     5508 : request_5508,
     5509 : request_5509,
     5510 : request_5510,
     5511 : request_5511,
     5512 : request_5512,
     5513 : request_5513,
     5514 : request_5514,
     5515 : request_5515,
     5516 : request_5516,
     5517 : request_5517,
     5518 : request_5518,
}

def use_stock(stock_id=''):
    if stock_id:
        for m in all_messages.values():
            if hasattr(m, 'stock_id'):
                setattr(m, 'stock_id', stock_id)

def create_parser(description=''):
    parser = argparse.ArgumentParser(description=description)
    group = parser.add_argument_group('server information')
    group.add_argument('-a', '--address', action='store', default='88',
                         help='ip address or an integer in which case the'+
                             ' address will be 202.104.236.xx, default 88')
    group.add_argument('-p', '--port', action='store', type = int, default=1862,
                        help='the port of the server, default 1862')
    group = parser.add_argument_group('stock information')
    group.add_argument('-m', '--message', nargs='*',
                        help ='message ids, delimited by space')
    group.add_argument('-s', '--stock-id', action='store',
                        help='the stock id to use, random if not set')
    parser.add_argument('-v', '--verbosity', action='count', default=2,
                        help='print verbose')
    return parser


def parse_args(args, parser = None, **kwargs):
    defaults = dict(message=[], stock_id=None, address='88', port=1862,
                    verbosity=2)
    defaults.update(kwargs)
    ns = argparse.Namespace(**defaults)
    if not parser:
        parser = create_parser()
    parser.parse_args(args=args, namespace=ns)
    if ns.stock_id:
        use_stock(ns.stock_id)

    if ns.message:
        message_ids = set(int(msg_id) for msg_id in ns.message)
    else:
        message_ids = set(all_messages.keys())

    if ns.address.rfind('.') < 0:
        ns.address = '202.104.236.{}'.format(ns.address)

    ns.message = OrderedDict()
    for mid in sorted(message_ids):
        m = all_messages[mid]
        if hasattr(m, 'push_type'):
            if ns.receive_push:
                m.push_type = message.PUSH_TYPE_REGISTER
            else:
                m.push_type = message.PUSH_TYPE_ONCE
        ns.message[mid] = m

    return ns
