import random
from datetime import datetime,timedelta
from ouou import message, util

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
request_5512.fields = [55,56,57]

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

request_5519 = message.Request_5519()
request_5519.pid = 1
request_5519.push_type = message.PUSH_TYPE_ONCE
request_5519.kline_type = message.KLINE_TYPE_DAY
request_5519.stock_id = 'HK|01250'
request_5519.start_time = 0
request_5519.req_num = 1
request_5519.restore_type = message.RESTORE_RIGHT_AFTER

all_messages = dict((int(x.split('_')[1]), y) for (x, y) in locals().items() if
        x.startswith('request_'))

def use_stock(stock_id=''):
    if stock_id:
        for m in all_messages.values():
            if hasattr(m, 'stock_id'):
                setattr(m, 'stock_id', stock_id)
