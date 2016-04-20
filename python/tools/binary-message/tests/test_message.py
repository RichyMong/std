import sys
import unittest
import random
import functools
import locale
from config import *
from ouou import net, message
from datetime import datetime,timedelta,tzinfo

class FixedOffset(tzinfo):
    def __init__(self, offset, name = 'ANONYMOUS'):
        self._offset = timedelta(hours=offset)
        self._dst = timedelta(hours=offset-1)
        self._name = name

    def utcoffset(self, dt):
        return self._offset + self.dst(dt)

    def dst(self, dt):
        # DST starts last Sunday in March
        d = datetime(dt.year, 4, 1)   # ends last Sunday in October
        self.dston = d - timedelta(days=d.weekday() + 1)
        d = datetime(dt.year, 11, 1)
        self.dstoff = d - timedelta(days=d.weekday() + 1)
        if self.dston <= dt.replace(tzinfo=None) < self.dstoff:
            return timedelta(hours=1)
        else:
            return timedelta(0)

    def tzname(self, dt):
        return self._name

def delta_ago(**kwargs):
    return datetime.now() - timedelta(**kwargs)

def save_config(key = ''):
    def decorated(func):
        @functools.wraps(func)
        def wrapper(self, *args, **kwargs):
            nonlocal key
            if not key:
                key = 'request_' + func.__name__.split('_')[-1]
            if key == 'all':
                saved = globals()
            else:
                saved = globals()[key]
            func(self, *args, **kwargs)
            if key == 'all':
                globals().update(saved)
            else:
                globals()[key] = saved
        return wrapper
    return decorated

class TestMessage(unittest.TestCase):
    def setUp(self):
        self.client = net.Client()
        self.client.connect(SERVER)

    @save_config('all')
    def test_message_5500(self):
        from collections import OrderedDict
        sent = OrderedDict()
        size = random.randint(2, len(all_messages))
        keys = list(all_messages.keys())
        while len(sent) < size:
            k = random.choice(keys)
            sent[k] = all_messages[k]
        sent_list = list(sent.values())
        responses = self.client.send_and_receive(*sent.values())
        for i, resp in enumerate(responses[:-1]):
            self.assertEqual(resp.msg_id, sent_list[i].msg_id)
        self.assertEqual(responses[-1].msg_id, message.MSG_TYPE_MULTI)

    @save_config('request_5502')
    def test_message_5502(self):
        sort_index = 2 #random.randint(0, len(request_5502.fields) - 1)
        request_5502.sort_field = request_5502.fields[sort_index]
        request_5502.sort_method = message.SORT_ORDER_DESC
        m = self.client.send_and_receive(request_5502)
        self.assertEqual(m.msg_id, request_5502.msg_id)
        self.assertEqual(m.pid, request_5502.pid)
        self.assertEqual(m.fields, request_5502.fields)
        data = m.data
        for i in range(len(data) - 1):
            v, next_v = data[i][sort_index], data[i+1][sort_index]
            if isinstance(v, str):
                if sys.platform[0:5] != 'win32':
                    locale.setlocale(locale.LC_COLLATE, 'zh_CN.GBK')
                else:
                    # not worked (python3.5 windows7)
                    # locale.setlocale(locale.LC_COLLATE, ".936")
                    pass
                self.assertGreaterEqual(locale.strcoll(v, next_v), 0)
            else:
                # require as little as possible for the class, so we do not
                # use assertGreaterEqual to express v >= next_v
                self.assertFalse(v < next_v)
        locale.setlocale(locale.LC_COLLATE, '')

    def test_message_5501(self):
        m = self.client.send_and_receive(request_5501)
        self.assertEqual(m.msg_id, request_5501.msg_id)
        self.assertEqual(m.request_extra, request_5501.request_extra)
        self.assertEqual(m.fields, request_5501.fields)
        self.assertEqual(m.extra.fields, request_5501.extra_fields)
        data = m.extra.data
        if len(data) > 0 and data[0].date_time > request_5501.date_time:
            self.assertTrue(m.extra.need_clear_local)
        else:
            self.assertFalse(m.extra.need_clear_local)

    def test_message_5503(self):
        m = self.client.send_and_receive(request_5503)
        self.assertEqual(m.msg_id, request_5503.msg_id)
        self.assertEqual(m.fields, request_5503.fields)

    def test_message_5504(self):
        '''
        At present we don't check trade time and this test may fail if it's
        run at non-trade time.
        '''
        request_5504.market_code = 'HK|00700'
        m = self.client.send_and_receive(request_5504)
        data = m.data
        self.assertEqual(m.msg_id, request_5504.msg_id)
        self.assertEqual(len(m.data), 2)
        self.assertEqual(data[0].nth_start_day, 0)
        self.assertEqual(data[0].nth_end_day, 0)
        self.assertEqual((data[0].start_time, data[0].end_time), (930, 1200))
        self.assertEqual((data[-1].start_time, data[-1].end_time), (1300, 1600))

        request_5504.market_code = 'NASDAQ|AAPL'

        # use the server's time to make the decision
        sm = self.client.send_and_receive(request_5505)
        now = datetime.strptime('{}{}'.format(sm.date, sm.time), '%Y%m%d%H%M%S')
        us_now = now.replace(tzinfo = FixedOffset(-5))
        # DST: Beijing time 21:30 - 4:00
        us_start, us_end = 2130, 400
        m = self.client.send_and_receive(request_5504)
        data = m.data
        self.assertEqual(len(m.data), 1)
        if not us_now.dst():
            us_start += 100
            us_end += 100
        self.assertEqual(data[0].start_time, us_start)
        self.assertEqual(data[0].end_time, us_end)
        begin = now.replace(hour = (us_start//100), minute = 30)
        end = now.replace(hour = (us_end//100), minute = 00) + timedelta(days=1)

        if 400 <= (now.hour * 100 + now.minute) <= 2359:
            self.assertEqual(data[0].nth_start_day, 0)
        else:
            self.assertEqual(data[0].nth_start_day, -1)

        if 0 <= (now.hour * 100 + now.minute) <= 400:
            self.assertEqual(data[0].nth_end_day, 0)
        else:
            self.assertEqual(data[0].nth_end_day, 1)

    def test_message_5505(self):
        m = self.client.send_and_receive(request_5505)

    def test_message_5508(self):
        m = self.client.send_and_receive(request_5508)
        self.assertEqual(m.msg_id, request_5508.msg_id)
        self.assertEqual(m.server_version, SERVER_VERSION)

    def test_message_5509(self):
        m = self.client.send_and_receive(request_5509)
        self.assertGreater(m.online_num, 0)

    def test_message_5510(self):
        m = self.client.send_and_receive(request_5510)
        today = int(datetime.now().strftime('%Y%m%d'))
        self.assertLessEqual(m.start_date, today)
        self.assertTrue(0 <= m.start_time <= 235900)

    @save_config()
    def test_message_5511(self):
        m = self.client.send_and_receive(request_5511)
        self.assertEqual(m.msg_id, request_5511.msg_id)
        self.assertEqual(m.pid, request_5511.pid)
        self.assertEqual(m.stock_id, request_5511.stock_id)

    @save_config()
    def test_message_5512(self):
        request_5512.push_type = message.PUSH_TYPE_ONCE
        m = self.client.send_and_receive(request_5512)
        self.assertEqual(m.pid, request_5512.pid)
        self.assertEqual(m.stock_id, request_5512.stock_id)
        self.assertEqual(m.fields, request_5512.fields)
        self.assertEqual(len(m.data), len(m.fields))

    @save_config()
    def test_message_5513(self):
        request_5513.push_type = message.PUSH_TYPE_ONCE
        request_5513.date_time = 0
        m = self.client.send_and_receive(request_5513)
        self.assertEqual(m.pid, request_5513.pid)
        self.assertEqual(m.stock_id, request_5513.stock_id)
        self.assertFalse(m.td.need_clear_local)
        self.assertEqual(m.td.fields, request_5513.fields)

        request_5513.date_time = int(delta_ago(days=7).strftime('%y%m%d%H%M')) # yyMMddHHmm
        m = self.client.send_and_receive(request_5513)
        if len(m.td.data) > 0 and m.td.data[0].date_time > request_5513.date_time:
            self.assertTrue(m.td.need_clear_local)
        else:
            self.assertFalse(m.td.need_clear_local)

    @save_config()
    def test_message_5514(self):
        request_5514.push_type = message.PUSH_TYPE_ONCE
        request_5514.flag = message.REQ_FLAG_NR_ROOTS
        request_5514.value = random.randint(2, 10)
        request_5514.nr_of_roots = 0

        m = self.client.send_and_receive(request_5514)
        self.assertEqual(m.msg_id, request_5514.msg_id)
        self.assertEqual(m.pid, request_5514.pid)
        self.assertEqual(m.stock_id, request_5514.stock_id)
        self.assertTrue(m.need_clear_local)

        request_5514.flag = message.REQ_FLAG_START_TIME
        request_5514.value = int(delta_ago(minutes=5).strftime('%H%M%S'))
        request_5514.nr_of_roots = random.randint(2, 10)
        m = self.client.send_and_receive(request_5514)
        self.assertLessEqual(len(m.data), request_5514.nr_of_roots)
        if len(m.data) > 0 and m.data[0].time > request_5514.value:
            self.assertTrue(m.need_clear_local)
        else:
            self.assertFalse(m.need_clear_local)

    @save_config()
    def test_message_5515(self):
        m = self.client.send_and_receive(request_5515)
        self.assertEqual(m.msg_id, request_5515.msg_id)
        self.assertEqual(m.pid, request_5515.pid)
        self.assertGreaterEqual(m.req_num, m.buy_num)
        self.assertGreaterEqual(m.req_num, m.sell_num)
        self.assertEqual(len(m.buy_queue), m.buy_num)
        self.assertEqual(len(m.sell_queue), m.sell_num)

    @save_config()
    def test_message_5516(self):
        request_5516.req_type = message.REQ_TYPE_SELF_STOCK
        request_5516.content = ['HK|00700','HK|00001','NASDAQ|AAPL']
        m = self.client.send_and_receive(request_5516)
        self.assertEqual(m.msg_id, request_5516.msg_id)
        self.assertEqual(m.pid, request_5516.pid)
        self.assertEqual(m.fields, request_5516.fields)
        self.assertLessEqual(len(m.data), len(request_5516.content))

    @save_config()
    def test_message_5517(self):
        request_5517.pid = random.randint(1, 65535)
        m = self.client.send_and_receive(request_5517)
        self.assertEqual(m.pid, request_5517.pid)

    @save_config()
    def test_message_5518(self):
        request_5518.md5 = 'AAABD'
        m = self.client.send_and_receive(request_5518)
        self.assertEqual(m.msg_id, request_5518.msg_id)
        self.assertEqual(m.pid, request_5518.pid)
        self.assertEqual(m.result, 1)

        request_5518.md5 = m.broker_data.md5
        m = self.client.send_and_receive(request_5518)
        self.assertEqual(m.result, 0)

    def tearDown(self):
        self.client.close()

def runtest(ns):
    if ns.message.keys() == all_messages.keys():
        suite = unittest.TestLoader().loadTestsFromTestCase(TestMessage)
    else:
        suite = unittest.TestSuite(map(TestMessage,
                      ('test_message_{}'.format(x) for x in ns.message.keys())))

    unittest.TextTestRunner(verbosity=ns.verbosity).run(suite)

if __name__ == '__main__':
    import sys

    ns = parse_args(sys.argv[1:], receive_push=False)

    runtest(ns)
