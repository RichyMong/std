from . import message
from ouou.util.functions import range_str
from ouou.util.etypes import *


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
        r = ('5501 - 请求单个股票可变字段' +
             '\n\t股票代码：{}' +
             '\n\t同时请求其他数据：{}' +
             '\n\t请求字段ID({})：{}').format(
                self.stockid,
                self.extra_data,
                len(self.fields), range_str(self.fields)
            )
        if self.extra_data:
            r += ('\n\t同时请求开始时间: {}' +
                  '\n\t同时请求字段ID({}): {}').format(
                    self.date_time,
                    len(self.extra_fields), range_str(self.extra_fields)
                 )
        return r


class Response_5512(message.Message, metaclass = ResponseMeta):
    msg_id = 5512

    fields_info = (
                      ('pid', UShort),
                      ('stockid', String),
                      ('fields', Vector(Byte, Byte))
                  )

    fields_name = [
        '行情日期和时间', '唯一标识', '股票名称', '股票代码', '市场代码',  # 1-5
        '市场类别', '交易标示', '昨收价', '开盘价', '最高价', '最低价', # 6 - 11
        '最新价', '成交量', '成交额', '成交笔数', '外盘', '现手', '现手方向', # 12- 18
        '持仓量变化', '均价', '持仓量', '今结算价', '昨持仓量', '昨结算价',
        '涨停价', '跌停价', '涨跌幅', '涨跌值', '价格类小数位', '显示小数位',
        '内盘', '收益', '市盈率', '净资产', '市净率', '总股本', '港股本',
        '总值', '港值', '换手率', '反向汇率', '沪港通标志','52周最高价',
        '52周最低价', '振幅',
    ]


    def __repr__(self):
        return ('5512 - 推送个股报价' +
                '\n\t协议标识：{}' +
                '\n\t代码唯一标识：{}' +
                '\n\t请求字段ID({})：{}').format(
                    self.pid,
                    self.stockid,
                    len(self.fields), range_str(self.fields)
                )

class Response_5513(message.Message, metaclass = ResponseMeta):
    msg_id = 5513

    fields_info = (
                      ('pid', UShort),
                      ('push_type', Byte),
                      ('stockid', String),
                      ('fields', Vector(Byte, Byte)),
                      ('date_time', UInt)
                  )

    def __repr__(self):
        return ('5513 - 推送分时走势' +
                '\n\t协议标识：{}' +
                '\n\t推送类型：{}' +
                '\n\t代码唯一标识：{}' +
                '\n\t请求字段ID({})：{}' +
                '\n\t请求开始时间: {}').format(
                    self.pid,
                    self.push_type,
                    self.stockid,
                    len(self.fields), range_str(self.fields),
                    self.date_time
                )


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
                '\n\t请求字段ID({})：{}' +
                '\n\t请求标识: {}' +
                '\n\t请求最新根数或请求开始时间(HHmmss): {}' +
                '\n\t请求根数: {}').format(
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
                      ('pid', UShort),
                      ('push_type', Byte),
                      ('stockid', String),
                      ('number', Byte),
                  )

    def __repr__(self):
        return ('5515 - 推送经纪队列' +
                '\n\t协议标识：{}' +
                '\n\t推送类型：{}' +
                '\n\t代码唯一标识：{}' +
                '\n\t请求经纪队列个数: {}').format(
                    self.pid,
                    self.push_type,
                    self.stockid,
                    self.number
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
                '\n\t请求个数: {}' +
                '\n\t请求字段ID({}): {}' +
                '\n\t请求类型: {}' +
                '\n\t请求数据内容({}):').format(
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
                      ('pid', UShort),
                  )

    def __repr__(self):
        return ('5517 - 心跳机制\n\t协议标识：{}').format(self.pid)


class Response_5518(message.Message, metaclass = ResponseMeta):
    msg_id = 5518

    fields_info = (
                      ('pid', UShort),
                      ('md5', Array(Char, 33))
                  )

    def __repr__(self):
        return ('5518 - 请求经纪商中文简称' +
                '\n\t协议标识：{}' +
                '\n\tMD5校验：{}').format(
                    self.pid,
                    self.md5
                )
