import unittest
from ouou import message, util

class TestParser(unittest.TestCase):
    @staticmethod
    def parse_hex(hex_data):
        b = bytearray.fromhex(hex_data)
        return message.Message.allfrombytes(b, c2s = True)

    def test_parse_request_5512(self):
        m = self.parse_hex('7b8815820020000100020800484b7c3030373030122a2c2d2e2f303132333435363738393a3b3c7d')
        self.assertEqual(m.msg_id, 5512)
        self.assertEqual(m.pid, 1)
        self.assertEqual(m.stock_id, 'HK|00700')
        self.assertEqual(m.fields, util.range_str_to_list('42 44-60'))

    def test_parse_request_5513(self):
        m = self.parse_hex('7b8915820020000100010800484b7c30303730300e0102030405060708090a0b0c0d0eaa848f5f7d')
        self.assertEqual(m.msg_id, 5513)
        self.assertEqual(m.pid, 1)
        self.assertEqual(m.push_type, 1)
        self.assertEqual(m.stock_id, 'HK|00700')
        self.assertEqual(m.fields, util.range_str_to_list('1-14'))

    def test_parse_request_5514(self):
        m = self.parse_hex('7b8a1582001f000100010800484b7c303037303008010203040506070801a0860100102700007d')
        self.assertEqual(m.msg_id, 5514)
        self.assertEqual(m.pid, 1)
        self.assertEqual(m.push_type, 1)
        self.assertEqual(m.stock_id, 'HK|00700')
        self.assertEqual(m.fields, util.range_str_to_list('1-8'))
        self.assertEqual(m.flag, 1)
        self.assertEqual(m.value, 100000)
        self.assertEqual(m.nr_of_roots, 10000)

    def test_parse_request_5515(self):
        m = self.parse_hex('7b8b1582000e000100010800484b7c3030373030147d')
        self.assertEqual(m.msg_id, 5515)
        self.assertEqual(m.pid, 1)
        self.assertEqual(m.push_type, 1)
        self.assertEqual(m.stock_id, 'HK|00700')
        self.assertEqual(m.number, 20)

    def test_parse_request_5516(self):
        m = self.parse_hex('7b8c15820036000100010d0100000a0008030405090a0b0c0d0103000800484b7c30303730300800484b7c30303030310b004e41534441517c4141504c7d')
        self.assertEqual(m.msg_id, 5516)
        self.assertEqual(m.pid, 1)
        self.assertEqual(m.push_type, 1)
        self.assertEqual(m.sort_field, 13)
        self.assertEqual(m.sort_method, 1)
        self.assertEqual(m.start_pos, 0)
        self.assertEqual(m.req_num, 10)
        self.assertEqual(m.fields, util.range_str_to_list('3-5, 9-13'))
        self.assertEqual(m.req_type, 1)
        self.assertEqual(m.content, ['HK|00700', 'HK|00001', 'NASDAQ|AAPL'])

    def test_parse_request_5518(self):
        m = self.parse_hex('7b8e158200230001006161610000000000000000000000000000000000000000000000000000000000007d')
        self.assertEqual(m.msg_id, 5518)

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestParser)
    unittest.TextTestRunner(verbosity=2).run(suite)
