from . import message
from . import stream
from ouou.util import *
from collections import OrderedDict


__all__ = [
            'Response_5501', # 'Response_5502', 'Response_5503', 'Response_5504',
            # 'Response_5505', 'Response_5506', 'Response_5507', 'Response_5508',
            # 'Response_5509', 'Response_5510', 'Response_5511',
            'Response_5512',
            'Response_5513', 'Response_5514', 'Response_5515', 'Response_5516',
            'Response_5517', 'Response_5518',
           ]


class ResponseMeta(message.BinaryObjectMeta):
    '''
    The meta class for response messages. The according class is supposed to
    define a 'msg_id' class attribute and it will be registered to process
    response messages of this type. If there's no class registered to process
    push messages of this type, we will also register it in push messages.
    '''
    def __new__(mcs, name, bases, attrs):
        cls = type.__new__(mcs, name, bases, attrs)

        message.response_messages[cls.msg_id] = cls

        # Be careful not to override the push message already registered.
        if cls.msg_id not in message.push_messages:
            message.push_messages[cls.msg_id] = cls

        return cls


class PushMeta(message.BinaryObjectMeta):
    '''
    The meta class for push messages. If not provided, ResponseMessageMeta
    will register the response message class as default.
    '''
    def __new__(mcs, name, bases, attrs):
        cls = type.__new__(mcs, name, bases, attrs)

        if cls.msg_id in message.push_messages and type(cls) is PushMessageMeta:
            raise RuntimeError('duplicated push message {} '.format(cls.msg_id))

        message.push_messages[cls.msg_id] = cls

        return cls


class Response_5501(message.Message, metaclass = ResponseMeta):
    msg_id = 5501

    fields_info = (
                      ('stockid', String),
                      ('extra_data', Byte),
                      ('fields', Vector(Byte, Byte))
                  )

    extra_fields_infog = (
                           ('start_date_time', UShort),
                           ('extra_fields', Vector(Byte, Byte))
                        )

    fields_name = [
        '行情日期和时间', '唯一标识', '股票名称', '股票代码', '市场代码',
        '市场类别', '交易标示', '昨收价', '开盘价', '最高价', '最低价',
        '最新价', '成交量', '成交额', '成交笔数', '外盘', '现手', '现手方向',
        '持仓量变化', '均价', '持仓量', '今结算价', '昨持仓量', '昨结算价',
        '涨停价', '跌停价', '买盘1价格', '买盘1挂单量', '卖盘1价格',
        '卖盘1挂单量', '买卖盘5档', '买卖盘10档', '涨跌幅', '涨跌值',
        '价格类小数位', '价格类显示小数位', '内盘', '收益', '市盈率',
        '净资产', '市净率', '总股本', '港股本', '总值', '港值', '换手率',
        '反向汇率', '沪港通标志','52周最高价', '52周最低价', '振幅',
    ]


    def tobytes(self):
        b = super().tobytes()
        if self.extra_data:
            b += self.start_date_time.tobytes()
            b += self.extra_fields.tobytes()
        return b

    @classmethod
    def frombytes(cls, buf, **kwargs):
        p = super().frombytes(buf, **kwargs)
        if p.extra_data:
            offset = p.stockid.size() + p.extra_data.size() + p.fields.size()
            self.start_date_time = Uint.frombytes(buf, offset)
            self.extra_fields = Vector(Byte, Byte).frombytes(buf, offset +
                    self.start_date_time.size())
        return p


    def __repr__(self):
        r = ('5501 - 应答单个股票可变字段' +
             '\n\t股票代码：{}' +
             '\n\t同时应答其他数据：{}' +
             '\n\t应答字段ID({})：{}').format(
                self.stockid,
                self.extra_data,
                len(self.fields), range_str(self.fields)
            )
        if self.extra_data:
            r += ('\n\t同时应答开始时间: {}' +
                  '\n\t同时应答字段ID({}): {}').format(
                    self.date_time,
                    len(self.extra_fields), range_str(self.extra_fields)
                 )
        return r


class Response_5512(message.Message, metaclass = ResponseMeta):
    msg_id = 5512

    fields_info = (
                      ('pid', UShort, '协议标识'),
                      ('stockid', String, '代码唯一标识'),
                      ('fields', Vector(Byte, Byte), '应答字段ID')
                  )

    # field id >= 41
    fields_name = [
        '卖十价格', '卖十量', '卖九价格', '卖九量', '卖八价格', '卖八量',
        '卖七价格', '卖七量', '卖六价格', '卖六量', '卖五价格', '卖五量',
        '卖四价格', '卖四量', '卖三价格', '卖三量', '卖二价格', '卖二量',
        '卖一价格', '卖一量',
        '买十价格', '买十量', '买九价格', '买九量', '买八价格', '买八量',
        '买七价格', '买七量', '买六价格', '买六量', '买五价格', '买五量',
        '买四价格', '买四量', '买三价格', '买三量', '买二价格', '买二量',
        '买一价格', '买一量',
        '买卖五档', '买卖十档', '买卖一档', '行情日期和时间', '唯一标识',
        '股票名称', '股票代码', '市场代码', '市场类别', '交易标示', '昨收价',
        '开盘价', '最高价', '最低价', '最新价', '成交量', '成交额', '成交笔数',
        '外盘', '现手', '现手方向', '持仓量变化', '均价', '持仓量', '今结算价',
        '昨持仓量', '昨结算价', '涨停价', '跌停价', '涨跌幅', '涨跌值',
        '价格类小数位', '显示小数位', '内盘', '收益', '市盈率', '净资产',
        '市净率', '总股本', '港股本', '总值', '港值', '换手率', '反向汇率',
        '沪港通标志','52周最高价', '52周最低价', '振幅',
    ]

    level_map = { 41 : 5, 42 : 10, 43 : 1 }

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.opt_fields = OrderedDict()


    @staticmethod
    def get_price(reader):
        return (reader.read_uint() & 0X003FFFFF) / 1000


    @classmethod
    def _frombytes(cls, buf, offset = 0, **kwargs):
        obj = cls(**kwargs)
        offset += obj.fieldsfrombytes(buf, offset)
        reader = stream.Reader(buf, offset)

        for x in obj.fields:
            if x < 41:
                if x & 0x1:
                    obj.opt_fields[x] = self.get_price(reader)
                else:
                    obj.opt_fields[x] = reader.read_uint()

            if x in obj.level_map:
                level = obj.level_map[x]
                sells = []
                for i in range(level):
                    sells.append((obj.get_price(reader), reader.read_large_int()))

                buys = []
                for i in range(level):
                    buys.append((obj.get_price(reader), reader.read_large_int()))
                obj.opt_fields[x] = (sells, buys)

            elif x in (44, 55, 63, 70, 71, 75, 76, 77, 78, 83, 84, 88):
                obj.opt_fields[x] = reader.read_int()
            elif x in (51, 52, 53, 54, 58, 65, 67, 68, 69, 86, 87):
                obj.opt_fields[x] = reader.read_uint()
            elif x in (45, 46, 47):
                if x == 46:
                    obj.opt_fields[x] = reader.read_string(encoding = 'gbk')
                else:
                    obj.opt_fields[x] = reader.read_string()
            elif x in (48, 49):
                obj.opt_fields[x] = reader.read_short()
            elif x in (50, 61, 72, 73, 85):
                obj.opt_fields[x] = reader.read_char()
            elif x in (56, 57, 59, 60, 62, 64, 66, 74, 79, 80, 81, 82):
                obj.opt_fields[x] = reader.read_large_int()

        return obj


    def __repr__(self):
        r = super().__repr__()
        for (x, v) in self.opt_fields.items():
            if x in self.level_map:
                level = self.level_map[x]
                r += '\n\t{}: '.format(self.fields_name[x - 1])
                for (i, vol) in enumerate(v[0]):
                    r += '\n\t\t卖{} - {}'.format(CHINSE_NUMBER[level - i], vol)

                for (i, vol) in enumerate(v[1]):
                    r += '\n\t\t买{} - {}'.format(CHINSE_NUMBER[i + 1], vol)
            else:
                r += '\n\t{}: {}'.format(self.fields_name[x - 1], v)
        return r


class Response_5513(message.Message, metaclass = ResponseMeta):
    msg_id = 5513

    fields_info = (
                      ('pid', UShort, '协议标识'),
                      ('stockid', String, '代码唯一标识'),
                      ('fields', Vector(Byte, Byte), '应答字段ID'),
                      ('need_clear_local', Byte, '是否需要清除本地数据'),
                      ('total_num', '数据总数'),
                      ('return_num', '返回数据总数'),
                  )

    fields_name = (
                '时间', '开盘价', '最高价', '最低价', '分钟收盘价', '平均价',
                '成交量', '成交额', '成交笔数', '外盘', '持仓量', '持仓量变化',
                '结算价', '内盘'
            )


    @classmethod
    def _frombytes(cls, buf, offset = 0, **kwargs):
        obj = cls(**kwargs)
        offset += obj.fieldsfrombytes(buf, offset)
        reader = stream.Reader(buf, offset)
        for i in range(self.return_num):
            for f in self.fields:
                if f in (1, 2, 3, 4, 5, 6, 9, 13):
                    v = reader.read_int()
                else:
                    v = reader.read_large_int()


    def __repr__(self):
        r = super().__repr__()


class Response_5514(message.Message, metaclass = ResponseMeta):
    msg_id = 5514

    fields_info = (
                      ('pid', UShort),
                      ('push_type', Byte),
                      ('stockid', String),
                      ('fields', Vector(Byte, Byte)),
                      ('flag', Byte),
                      ('value', UInt),
                      ('nr_of_roots', Int)
                  )

    def __repr__(self):
        return ('5514 - 推送分时成交 ' +
                '\n\t协议标识：{}' +
                '\n\t推送类型：{}' +
                '\n\t代码唯一标识：{}' +
                '\n\t应答字段ID({})：{}' +
                '\n\t应答标识: {}' +
                '\n\t应答最新根数或应答开始时间(HHmmss): {}' +
                '\n\t应答根数: {}').format(
                    self.pid,
                    self.push_type,
                    self.stockid,
                    len(self.fields), range_str(self.fields),
                    self.flag,
                    self.value,
                    self.nr_of_roots
                )


class Response_5515(message.Message, metaclass = ResponseMeta):
    msg_id = 5515

    fields_info = (
                      ('pid', UShort, '协议标识'),
                      ('stockid', String, '代码唯一标识'),
                      ('req_num', Byte, '请求个数'),
                      ('buy_num', Byte, '买盘经纪队列个数'),
                      ('sell_num', Byte, '卖盘经纪队列个数')
                  )


class Response_5516(message.Message, metaclass = ResponseMeta):
    msg_id = 5516

    fields_info = (
                      ('pid', UShort),
                      ('push_type', Byte),
                      ('sort_field', Byte),
                      ('sort_method', Byte),
                      ('start_pos', UShort),
                      ('req_num', UShort),
                      ('fields', Vector(Byte, Byte)),
                      ('req_type', Byte),
                      ('content', Vector(UShort, String)),
                  )

    def __repr__(self):
        r = ('5516 - 推送行情列表' +
                '\n\t协议标识：{}' +
                '\n\t推送类型：{}' +
                '\n\t排序字段：{}' +
                '\n\t排序方式: {}' +
                '\n\t起始位置: {}' +
                '\n\t应答个数: {}' +
                '\n\t应答字段ID({}): {}' +
                '\n\t应答类型: {}' +
                '\n\t应答数据内容({}):').format(
                    self.pid,
                    self.push_type,
                    self.sort_field,
                    self.sort_method,
                    self.start_pos,
                    self.req_num,
                    len(self.fields), range_str(self.fields),
                    self.req_type,
                    len(self.content)
                )

        if self.req_type == 4:
            for (i, s) in enumerate(self.content):
                v = int(s)
                r += ' {}|{}'.format(v >> 16, v & 0xff)
        else:
            r += ' ' + ' '.join(x for x in self.content)

        return r


class Response_5517(message.Message, metaclass = ResponseMeta):
    msg_id = 5517

    fields_info = (
                      ('pid', UShort, '协议标识'),
                  )


class Response_5518(message.Message, metaclass = ResponseMeta):
    msg_id = 5518

    fields_info = (
                      ('pid', UShort, '协议标识'),
                      ('result', Byte, 'MD5校验结果'),
                      ('md5', Array(Char, 33), 'MD5')
                  )
