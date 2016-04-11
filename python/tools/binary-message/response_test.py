from ouou.message import *
from ouou.util import *
from ouou.message.response import PriceDisplay

def test_5515():
    r = Response_5515()
    r.pid = 1
    r.stock_id = 'HK|00700'
    r.req_num = 10
    r.buy_num = 5
    r.sell_num = 5
    r.buy_queue = [('B', 200), ('B', 30), ('S', 200), ('B', 5), ('B', 10)]
    r.sell_queue = [('B', 100), ('B', 50), ('S', 20), ('B', 15), ('B', 10)]
    b = r.tobytes()
    t = Message.allfrombytes(b, c2s = False)
    print(t)

def test_5503():
    r = Response_5503()
    r.close_price =  159000
    r.data_pos = 100
    r.need_clear_local = 0
    r.fields = range_str_to_list('1-4')
    r.deal_data.append(((411, 1030, 159500, 800000)))
    r.deal_data.append(((411, 1031, 159600, 50000)))
    b = r.tobytes()
    t = Message.allfrombytes(b, c2s = False)
    print(t)

def test_5504():
    r = Response_5504()
    r.return_data.append(r.data_type((1, 1030, 2, 16000)))
    r.return_data.append(r.data_type((1, 9300, 2, 16000)))
    b = r.tobytes()
    t = Message.allfrombytes(b, c2s = False)
    print(t)

def test_5506():
    r = Response_5506()
    r.result = 1
    r.server_inc_id = 3
    r.return_data.append((1, '116', '香港', 'HK', 2, 0, 1))
    r.return_data.append((2, '119', '香港', 'HK', 2, 0, 2))
    b = r.tobytes()
    t = Message.allfrombytes(b, c2s = False)
    print(t)

def test_5511():
    r = Response_5511()
    r.stock_id = 'HK|00700'
    r.pid = 1
    r.data.append((1, 'name', PriceDisplay(0x03, 0, '开盘价')))
    r.data.append((2, 'stock', PriceDisplay(0x04, 0, ['现手', '现手方向'])))
    b = r.tobytes()
    t = Message.allfrombytes(b, c2s = False)
    print(t)

test_5511()
test_5503()
test_5504()
test_5506()
test_5515()
