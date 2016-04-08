from . import message
from .message import NamedField, UnamedField, UnamedFieldInfo
from . import stream
from ouou.util import *
from collections import OrderedDict, namedtuple
import ctypes


__all__ = [
            'Response_5501', 'Response_5502', 'Response_5503', 'Response_5504',
            'Response_5505', 'Response_5506', 'Response_5508',
            'Response_5509', 'Response_5510', 'Response_5511', 'Response_5512',
            'Response_5513', 'Response_5514', 'Response_5515', 'Response_5516',
            'Response_5517', 'Response_5518',
           ]


OrderDetail = namedtuple('OrderDetail', ['price_seat', 'volume'])

def VarFields(fields):
    '''
    Fields must be an iterable object of NamedField or UnamedField
    '''
    class VariableFieldContent(message.BinaryObject):
        named_fields_info = (
            NamedField('fields', ByteVector, '可变字段'),
        )

        unamed_fields_info = [
            UnamedFieldInfo(
                lambda self : 1,
                lambda self : self.fields,
                fields
            )
        ]

    return VariableFieldContent


class PriceSeat(UInt):
    def __str__(self):
        price = self & 0X003FFFFF
        seat = (self >> 22) & 0XCFF
        return '价格: {}, 席位数: {}'.format(price, seat)


    def __repr__(self):
        price = self & 0X003FFFFF
        seat = (self >> 22) & 0XCFF
        return '价格: {}, 席位数: {}'.format(price, seat)


def BusinessLevel(level):
    class WrappedBusinessLevel(object):
        def __init__(self):
            '''
            Buy and sell details are the according orders. They must have the
            same length.
            '''
            self.buy_details = []
            self.sell_details = []


        @staticmethod
        def get_order(reader):
            price_seat = PriceSeat(reader.read_uint())
            volume = reader.read_large_int()
            return OrderDetail(price_seat, volume)

        def size(self):
            return len(self.buy_details) * 8 + len(self.sell_details) * 8


        @classmethod
        def frombytes(cls, buf, offset = 0, **kwargs):
            obj = cls(**kwargs)

            reader = stream.Reader(buf, offset)
            for i in range(level):
                obj.sell_details.insert(0, obj.get_order(reader))

            for i in range(level):
                obj.buy_details.insert(0, obj.get_order(reader))
            return obj


        def __repr__(self):
            r = ''
            for i, detail in enumerate(self.sell_details):
                r += '\n\t\t卖{} - {}, 量: {}'.format(CHINSE_NUMBER[i + 1],
                               detail.price_seat, detail.volume)

            for i, detail in enumerate(self.buy_details):
                r += '\n\t\t买{} - {}, 量: {}'.format(CHINSE_NUMBER[i + 1],
                               detail.price_seat, detail.volume)

            return r
    return WrappedBusinessLevel


class ResponseMeta(message.MessageMeta):
    '''
    The meta class for response messages. The according class is supposed to
    define a 'msg_id' class attribute and it will be registered to process
    response messages of this type. If there's no class registered to process
    push messages of this type, we will also register it in push messages.
    '''
    def __new__(mcs, name, bases, attrs):
        cls = super().__new__(mcs, name, bases, attrs)

        message.response_messages[cls.msg_id] = cls

        # Be careful not to override the push message already registered.
        if cls.msg_id not in message.push_messages:
            message.push_messages[cls.msg_id] = cls

        return cls


class PushMeta(message.MessageMeta):
    '''
    The meta class for push messages. If not provided, ResponseMessageMeta
    will register the response message class as default.
    '''
    def __new__(mcs, name, bases, attrs):
        cls = super().__new__(mcs, name, bases, attrs)

        if cls.msg_id in message.push_messages and type(message.push_messages) is PushMeta:
            raise RuntimeError('duplicated push message {} '.format(cls.msg_id))

        message.push_messages[cls.msg_id] = cls

        return cls


class TimeTrend(message.BinaryObject):
    '''
    Used by response 5501 and 5513.
    '''
    named_fields_info = (
            NamedField('fields', ByteVector, '应答字段ID'),
            NamedField('need_clear_local', Byte, '是否需要清除本地数据'),
            NamedField('total_num', UShort, '数据总数'),
            NamedField('return_num', UShort, '返回数据总数'),
        )

    unamed_fields_info = [
        UnamedFieldInfo(
            lambda self : self.return_num,
            lambda self : self.fields,
            (
                UnamedField('时间', Int),
                UnamedField('开盘价', UInt),
                UnamedField('最高价', UInt),
                UnamedField('最低价', UInt),
                UnamedField('分钟收盘价', UInt),
                UnamedField('平均价', UInt),
                UnamedField('成交量', LargeInt),
                UnamedField('成交额', LargeInt),
                UnamedField('成交笔数', UInt),
                UnamedField('外盘', LargeInt),
                UnamedField('持仓量', LargeInt),
                UnamedField('持仓量变化', LargeInt),
                UnamedField('结算价', UInt),
                UnamedField('内盘', LargeInt)
            )
        )
    ]


class TimeSharing(message.BinaryObject):
    '''
    Used by response 5503 and 5514.
    '''
    named_fields_info = (
        NamedField('fields', ByteVector, '应答字段ID'),
        NamedField('need_clear_local', Byte, '是否需要清除本地数据'),
        NamedField('total_num', UShort, '数据总数'),
        NamedField('return_num', UShort, '返回数据总数'),
    )

    unamed_fields_info = [
        UnamedFieldInfo(
            lambda self : self.return_num,
            lambda self : self.fields,
            (
                UnamedField('时间', Int),
                UnamedField('开盘价', UInt),
                UnamedField('最高价', UInt),
                UnamedField('最低价', UInt),
                UnamedField('分钟收盘价', UInt),
                UnamedField('平均价', UInt),
                UnamedField('成交量', LargeInt),
                UnamedField('成交额', LargeInt),
                UnamedField('成交笔数', UInt),
                UnamedField('外盘', LargeInt),
                UnamedField('持仓量', LargeInt),
                UnamedField('持仓量变化', LargeInt),
                UnamedField('结算价', UInt),
                UnamedField('内盘', LargeInt)
            )
        )
    ]


class Response_5500(message.Message, metaclass = ResponseMeta):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.buf = None


    def __iter__(self):
        return self


    def __next__(self):
        if not message.Message.is_complete(self.buf):
            raise StopIteration

        m = message.Message.allfrombytes(self.buf)
        self.buf = self.buf[m.size():]
        return m


    @classmethod
    def _frombytes(cls, buf, **kwargs):
        p = cls()
        p.buf = buf[:p.header.msg_size]

        return p


class Response_5501(message.Message, metaclass = ResponseMeta):
    quotation_response_fields = (
        UnamedField('行情日期和时间', Int),
        UnamedField('唯一标识', String),
        UnamedField('股票名称', GBKString),
        UnamedField('股票代码', String),
        UnamedField('市场代码', UShort),
        UnamedField('市场类别', UShort),
        UnamedField('交易标示', Byte),
        UnamedField('昨收价', UInt),
        UnamedField('开盘价', UInt),
        UnamedField('最高价', UInt),
        UnamedField('最低价', UInt),
        UnamedField('最新价', Int),
        UnamedField('成交量', LargeInt),
        UnamedField('成交额', LargeInt),
        UnamedField('成交笔数', UInt),
        UnamedField('外盘', LargeInt),
        UnamedField('现手', LargeInt),
        UnamedField('现手方向', Byte),
        UnamedField('持仓量变化', LargeInt),
        UnamedField('均价', Int),
        UnamedField('持仓量', LargeInt),
        UnamedField('今结算价', UInt),
        UnamedField('昨持仓量', LargeInt),
        UnamedField('昨结算价', UInt),
        UnamedField('涨停价', UInt),
        UnamedField('跌停价', UInt),
        UnamedField('买盘1价格', UInt),
        UnamedField('买盘1挂单量', UInt),
        UnamedField('卖盘1价格', UInt),
        UnamedField('卖盘1挂单量', UInt),
        UnamedField('买卖五档', BusinessLevel(5)),
        UnamedField('买卖十档', BusinessLevel(10)),
        UnamedField('涨跌幅', Int),
        UnamedField('涨跌值', Int),
        UnamedField('价格类小数位', Byte),
        UnamedField('显示小数位', Byte),
        UnamedField('内盘', LargeInt),
        UnamedField('收益', Int),
        UnamedField('市盈率', Int),
        UnamedField('净资产', Int),
        UnamedField('市净率', Int),
        UnamedField('总股本', LargeInt),
        UnamedField('港股本', LargeInt),
        UnamedField('总值', LargeInt),
        UnamedField('港值', LargeInt),
        UnamedField('换手率', Int),
        UnamedField('反向汇率', Int),
        UnamedField('沪港通标志', Byte),
        UnamedField('52周最高价', UInt),
        UnamedField('52周最低价', UInt),
        UnamedField('振幅', Int),
    )

    named_fields_info = (
        NamedField('extra_data', Byte, '同时请求其他数据'),
        NamedField('var_fields', VarFields(quotation_response_fields), '可变字段数据')
    )

    unamed_fields_info = [
        UnamedFieldInfo(
            lambda self : 1 if self.extra_data else 0,
            None,
            (
                UnamedField('同时请求走势/分时成交/K线附加数据', TimeTrend),
            )
        )
    ]


class Response_5502(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
        NamedField('pid', Byte, '协议标识'),
        NamedField('fields', ByteVector, '返回字段ID'),
        NamedField('total_num', UShort, '总数据个数'),
        NamedField('return_num', UShort, '返回数据个数'),
    )

    unamed_fields_info = [
        UnamedFieldInfo(
            lambda self : self.return_num,
            lambda self : self.fields,
            Response_5501.quotation_response_fields
        )
    ]



class Response_5503(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
                      ('close_price', UInt, '昨收盘'),
                      ('data_pos', UInt, '数据位置'),
                      ('need_clear_local', Byte, '是否需要清除本地数据'),
                      ('fields', ByteVector, '应答字段ID'),
                      ('return_num', UShort, '返回数据总数'),
                  )


    unamed_fields_info = [
        UnamedFieldInfo(
            lambda self : self.return_num,
            lambda self : self.fields,
            (
                UnamedField('日期', UInt),
                UnamedField('时间', UInt),
                UnamedField('价格', UInt),
                UnamedField('成交量', LargeInt),
                UnamedField('成交笔数', UInt),
                UnamedField('持仓量变化', LargeInt),
                UnamedField('交易类型', Byte),
                UnamedField('开平', Byte)
            )
        )
    ]


class Response_5504(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
          ('return_num', Byte, '返回个数'),
    )


    unamed_fields_info = [
        UnamedFieldInfo(
            lambda self : self.return_num,
            None,
            (
                UnamedField('开始相对的第几天', Byte),
                UnamedField('开始时间', UShort),
                UnamedField('结束相对的第几天', Byte),
                UnamedField('结束时间', UShort),
            )
        )
    ]


class Response_5505(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
          NamedField('server_date', Int, '服务器日期'),
          NamedField('server_time', Int, '服务器时间'),
    )


class Response_5506(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
        NamedField('result', Byte, '处理结果'),
        NamedField('server_inc_id', UInt, '服务器最新增量ID'),
        NamedField('return_num', UShort, '返回個數')
    )

    unamed_fields_info = [
        UnamedFieldInfo(
            lambda self: self.return_num,
            None,
            (
                UnamedField('市场代码', UShort),
                UnamedField('证券代码', String),
                UnamedField('证券名称', GBKString),
                UnamedField('简拼', GBKString),
                UnamedField('所属类别', Byte),
                UnamedField('标示(0是新增或覆盖,1是删除)', Byte),
                UnamedField('增量ID', UInt),
            )
        )
    ]


class Response_5508(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
        NamedField('server_version', Int, '服务器版本号'),
    )


class Response_5509(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
        NamedField('online_num', Int, '在线数'),
    )


class Response_5510(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
        NamedField('start_date', Int, '启动日期'),
        NamedField('start_time', Int, '启动时间'),
    )


class PriceDisplay(message.BinaryObject):
    named_fields_info = (
        NamedField('data_type', UShort, '字段类型'),
        NamedField('extend_data', Byte, '扩展字段类型'),
    )

    data_types = [ Int, LargeInt, LargeInt, String, Array(2, String) ]

    @classmethod
    def _frombytes(cls, buf, offset = 0, **kwargs):
        p = cls(**kwargs)

        offset += p.fieldsfrombytes()

        p.data = cls.data_types[self.data_type & 0x03].frombytes(buf, offset)

        return p


    def __repr__(self):
        r = super().__repr__()
        r += '\n\t数据类型: {}'.format(self.data_type & 0x07)
        r += '\n\t小数位数: {}'.format((self.data_type >> 3) & 0x07)
        r += '\n\t逻辑红绿白: {}'.format((self.data_type >> 6) & 0x03)
        r += '\n\t数值单位: {}'.format((self.data_type >> 8) & 0x0F)
        r += '\n\t单位乘除标识: {}'.format((self.data_type >> 12) & 0x01)
        r += '\n\t是否百分比: {}'.format((self.data_type >> 13) & 0x01)
        r += '\n\t是否有超链接: {}'.format((self.data_type >> 14) & 0x01)
        r += '\n\t预留1位: {}'.format((self.data_type >> 15) & 0x01)
        return r


class Response_5511(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
        NamedField('stockid', String, '代码唯一标识'),
        NamedField('pid', UShort, '协议标识'),
        NamedField('number', Byte, '字段个数')
    )

    unamed_fields_info = [
        UnamedFieldInfo(
            lambda self : self.number,
            None,
            (
                UnamedField('字段索引', Byte),
                UnamedField('字段数据名称', String),
                UnamedField('数据', PriceDisplay)
            )
        )
    ]

class Response_5512(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
        NamedField('pid', UShort, '协议标识'),
        NamedField('stockid', String, '代码唯一标识'),
        NamedField('fields', ByteVector, '应答字段ID')
    )

    unamed_fields_info = [
        UnamedFieldInfo(
            lambda x : 1,
            lambda x : x.fields,
            (
                UnamedField('卖十价格', PriceSeat),
                UnamedField('卖十量', LargeInt),
                UnamedField('卖九价格', PriceSeat),
                UnamedField('卖九量', LargeInt),
                UnamedField('卖八价格', PriceSeat),
                UnamedField('卖八量', LargeInt),
                UnamedField('卖七价格', PriceSeat),
                UnamedField('卖七量', LargeInt),
		UnamedField('卖六价格', PriceSeat),
                UnamedField('卖六量', LargeInt),
		UnamedField('卖五价格', PriceSeat),
                UnamedField('卖五量', LargeInt),
                UnamedField('卖四价格', PriceSeat),
                UnamedField('卖四量', LargeInt),
                UnamedField('卖三价格', PriceSeat),
                UnamedField('卖三量', LargeInt),
                UnamedField('卖二价格', PriceSeat),
                UnamedField('卖二量', LargeInt),
                UnamedField('卖一价格', PriceSeat),
                UnamedField('卖一量', LargeInt),
                UnamedField('买十价格', PriceSeat),
                UnamedField('买十量', LargeInt),
                UnamedField('买九价格', PriceSeat),
                UnamedField('买九量', LargeInt),
                UnamedField('买八价格', PriceSeat),
                UnamedField('买八量', LargeInt),
                UnamedField('买七价格', PriceSeat),
                UnamedField('买七量', LargeInt),
                UnamedField('买六价格', PriceSeat),
                UnamedField('买六量', LargeInt),
                UnamedField('买五价格', PriceSeat),
                UnamedField('买五量', LargeInt),
                UnamedField('买四价格', PriceSeat),
                UnamedField('买四量', LargeInt),
                UnamedField('买三价格', PriceSeat),
                UnamedField('买三量', LargeInt),
                UnamedField('买二价格', PriceSeat),
                UnamedField('买二量', LargeInt),
                UnamedField('买一价格', PriceSeat),
                UnamedField('买一量', LargeInt),
                UnamedField('买卖五档', BusinessLevel(5)),
                UnamedField('买卖十档', BusinessLevel(10)),
                UnamedField('买卖一档', BusinessLevel(1)),
		UnamedField('行情日期和时间', Int),
		UnamedField('唯一标识', String),
                UnamedField('股票名称', GBKString),
		UnamedField('股票代码', String),
		UnamedField('市场代码', UShort),
		UnamedField('市场类别', UShort),
		UnamedField('交易标示', Byte),
		UnamedField('昨收价', UInt),
                UnamedField('开盘价', UInt),
		UnamedField('最高价', UInt),
		UnamedField('最低价', UInt),
		UnamedField('最新价', Int),
		UnamedField('成交量', LargeInt),
		UnamedField('成交额', LargeInt),
		UnamedField('成交笔数', UInt),
                UnamedField('外盘', LargeInt),
		UnamedField('现手', LargeInt),
		UnamedField('现手方向', Byte),
		UnamedField('持仓量变化', LargeInt),
		UnamedField('均价', Int),
		UnamedField('持仓量', LargeInt),
		UnamedField('今结算价', UInt),
                UnamedField('昨持仓量', LargeInt),
		UnamedField('昨结算价', UInt),
		UnamedField('涨停价', UInt),
		UnamedField('跌停价', UInt),
		UnamedField('涨跌幅', Int),
		UnamedField('涨跌值', Int),
                UnamedField('价格类小数位', Byte),
		UnamedField('显示小数位', Byte),
		UnamedField('内盘', LargeInt),
		UnamedField('收益', Int),
		UnamedField('市盈率', Int),
		UnamedField('净资产', Int),
                UnamedField('市净率', Int),
		UnamedField('总股本', LargeInt),
		UnamedField('港股本', LargeInt),
		UnamedField('总值', LargeInt),
		UnamedField('港值', LargeInt),
		UnamedField('换手率', Int),
		UnamedField('反向汇率', Int),
                UnamedField('沪港通标志', Byte),
		UnamedField('52周最高价', UInt),
		UnamedField('52周最低价', UInt),
		UnamedField('振幅', Int),
            )
        )
    ]


class Response_5513(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
        NamedField('pid', UShort, '协议标识'),
        NamedField('stockid', String, '代码唯一标识'),
        NamedField('timerend', TimeTrend, '分时走势')
    )


class Response_5514(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
        NamedField('pid', UShort, '协议标识'),
        NamedField('stockid', String, '代码唯一标识'),
        NamedField('close_price', UInt, '昨收盘'),
        NamedField('real_digit', Byte, '昨收盘实际小数位'),
        NamedField('show_digit', Byte, '昨收盘显示小数位'),
        NamedField('fields', ByteVector, '应答字段ID'),
        NamedField('need_clear_local', Byte, '是否需要清除本地数据'),
        NamedField('data_pos', UInt, '数据位置'),
        NamedField('total_num', UShort, '数据总数'),
        NamedField('return_num', UShort, '返回数据总数'),
    )


    unamed_fields_info = Response_5503.unamed_fields_info


class Response_5515(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
                      NamedField('pid', UShort, '协议标识'),
                      NamedField('stockid', String, '代码唯一标识'),
                      NamedField('req_num', Byte, '请求个数'),
                      NamedField('buy_num', Byte, '买盘经纪队列个数'),
                      NamedField('sell_num', Byte, '卖盘经纪队列个数')
                  )


class Response_5516(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
        NamedField('pid', UShort, '协议标识'),
        NamedField('fields', ByteVector, '返回字段ID'),
        NamedField('total_num', UShort, '总数据个数'),
        NamedField('return_num', UShort, '返回数据个数'),
    )

    unamed_fields_info = [
        UnamedFieldInfo(
            lambda self : self.return_num,
            lambda self : self.fields,
            Response_5501.quotation_response_fields
        )
    ]


class Response_5517(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
        NamedField('pid', UShort, '协议标识'),
    )


class Response_5518(message.Message, metaclass = ResponseMeta):
    named_fields_info = (
                      NamedField('pid', UShort, '协议标识'),
                      NamedField('result', Byte, 'MD5校验结果'),
                      NamedField('md5', Array(Char, 33), 'MD5')
                  )


class Push_5514(message.Message, metaclass = PushMeta):
    named_fields_info = (
          NamedField('pid', UShort, '协议标识'),
          NamedField('stockid', String, '代码唯一标识'),
          NamedField('fields', ByteVector, '应答字段ID'),
          NamedField('need_clear_local', Byte, '是否需要清除本地数据'),
          NamedField('total_num', UShort, '数据总数'),
          NamedField('return_num', UShort, '返回数据总数'),
    )

    unamed_fields_info = Response_5514.unamed_fields_info


class Push_5516(message.Message, metaclass = PushMeta):
    named_fields_info = (
        NamedField('pid', UShort, '协议标识'),
        NamedField('total_num', UShort, '总数据个数'),
        NamedField('return_num', UShort, '返回数据个数'),
    )

    unamed_fields_info = [
        UnamedFieldInfo(
            lambda self : self.return_num,
            None,
            Response_5501.quotation_response_fields
        )
    ]


