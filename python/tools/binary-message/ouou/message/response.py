from functools import reduce
from collections import OrderedDict, namedtuple
from .import message
from .message import NamedField, DependentAttribute, DependentFieldsAttribute
from .const import *
from ouou.util import *

__all__ = [
            'Response_5501', 'Response_5502', 'Response_5503', 'Response_5504',
            'Response_5505', 'Response_5506', 'Response_5508',
            'Response_5509', 'Response_5510', 'Response_5511', 'Response_5512',
            'Response_5513', 'Response_5514', 'Response_5515', 'Response_5516',
            'Response_5517', 'Response_5518',
           ]

def VarArray(sizefunc, elem_cls):
    class Wrapper(TypeList(elem_cls), DependentAttribute):
        @classmethod
        def fromstream(cls, reader, target, **kwargs):
            self = cls(**kwargs)
            for i in range(sizefunc(target)):
                self.append(reader.read_type(elem_cls))
            return self

    return Wrapper

def VarFieldsVector(size_cls, fields, iterfunc):
    class Wrapper(list, DependentFieldsAttribute):
        def get_data_type(self, target):
            if not getattr(self, 'data_type', None):
                attributes_info = tuple(fields[x - 1] for x in iterfunc(target))
                self.data_type = message.create_class('VarFieldsWrapper',
                                    attributes_info)
            return self.data_type

        @classmethod
        def fromstream(cls, reader, target, **kwargs):
            self = cls(**kwargs)
            data_type = self.get_data_type(target)
            return Vector(size_cls, data_type).fromstream(reader)

        def tobytes(self, target):
            data_type = self.get_data_type(target)
            v = Vector(size_cls, data_type)()
            for x in self:
                v.append(x)

            return v.tobytes()

        def __repr__(self):
            r = '共 {} 个'.format(len(self))
            for x in self:
                r += '\n\t' + 80 * '-' + '{}'.format(x)
            return r

    return Wrapper

def VarFields(fields):
    '''
    Fields must be an iterable object of NamedField or UnamedField
    '''
    class VariableFieldContent(message.BinaryObject):
        attributes_info = (
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

class OrderDetail(BinaryObject):
    class PriceSeat(UInt):
        def __str__(self):
            price = self & 0X003FFFFF
            seat = (self >> 22) & 0XCFF
            return '价格 - {}, 席位数 - {}'.format(price, seat)

        def __repr__(self):
            price = self & 0X003FFFFF
            seat = (self >> 22) & 0XCFF
            return '价格 - {}, 席位数 - {}'.format(price, seat)

    attributes_info = (
        ('price_seat', PriceSeat, '价格席位'),
        ('volume', LargeInt, '成交量'),
    )

def BusinessLevel(level):
    class WrappedBusinessLevel(Array(2 * level, OrderDetail)):
        def __repr__(self):
            r = ''
            for i, detail in enumerate(self):
                if i < level:
                    r += '\n\t\t卖{} - {}'.format(CHINSE_NUMBER[level - i], detail)
                else:
                    r += '\n\t\t买{} - {}'.format(CHINSE_NUMBER[i + 1 - level], detail)

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
    data_type = message.create_class(
        'TimeTrend_data',
        (
            NamedField('time_data', Int, '时间'),
            NamedField('open_price', UInt, '开盘价'),
            NamedField('highest_price', UInt, '最高价'),
            NamedField('lowest_price', UInt, '最低价'),
            NamedField('close_price', UInt, '分钟收盘价'),
            NamedField('avg_price', UInt, '平均价'),
            NamedField('volume', LargeInt, '成交量'),
            NamedField('amount', LargeInt, '成交额'),
            NamedField('tick_count', UInt, '成交笔数'),
            NamedField('sellvol', LargeInt, '外盘'),
            NamedField('inventory', LargeInt, '持仓量'),
            NamedField('inventory_change', LargeInt, '持仓量变化'),
            NamedField('settlement_price', UInt, '结算价'),
            NamedField('buyvol', LargeInt, '内盘')
        )
    )

    attributes_info = (
            NamedField('fields', ByteVector, '应答字段ID'),
            NamedField('need_clear_local', Byte, '是否需要清除本地数据'),
            NamedField('total_num', UShort, '数据总数'),
            NamedField('return_num', UShort, '返回数据总数'),
            NamedField('return_data', UShort, '分时走势数据')
        )

class TimeSharing(message.BinaryObject):
    '''
    Used by response 5503 and 5514.
    '''
    attributes_info = (
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
        self.buf = kwargs.get('reader', None)

    def __iter__(self):
        return self

    def __next__(self):
        m = message.Message.allfromstream(self.reader)
        if not m:
            raise StopIteration

        return m

    @classmethod
    def _fromstream(cls, reader, **kwargs):
        return cls(reader = reader, **kwargs)

class Response_5501(message.Message, metaclass = ResponseMeta):
    quotation_response_fields = (
        NamedField('date_time', Int, '行情日期和时间'),
        NamedField('stock_id', String, '唯一标识'),
        NamedField('stock_name', GBKString, '股票名称'),
        NamedField('stock_code', String, '股票代码'),
        NamedField('market_code', UShort, '市场代码'),
        NamedField('market_type', UShort, '市场类别'),
        NamedField('trade_flag', Byte, '交易标示'),
        NamedField('close_price', UInt, '昨收价'),
        NamedField('open_price', UInt, '开盘价'),
        NamedField('highest_price', UInt, '最高价'),
        NamedField('lowest_price', UInt, '最低价'),
        NamedField('latest_price', Int, '最新价'),
        NamedField('volume', LargeInt, '成交量'),
        NamedField('amount', LargeInt, '成交额'),
        NamedField('trade_count', UInt, '成交笔数'),
        NamedField('sellvol', LargeInt, '外盘'),
        NamedField('newvol', LargeInt, '现手'),
        NamedField('newvol_direction', Byte, '现手方向'),
        NamedField('open_interest_change', LargeInt, '持仓量变化'),
        NamedField('avg_price', Int, '均价'),
        NamedField('open_interest', LargeInt, '持仓量'),
        NamedField('settlement_price', UInt, '今结算价'),
        NamedField('last_open_interest', LargeInt, '昨持仓量'),
        NamedField('last_settlement_price', UInt, '昨结算价'),
        NamedField('limit_up_price', UInt, '涨停价'),
        NamedField('limit_down_price', UInt, '跌停价'),
        NamedField('bid1_price', UInt, '买盘1价格'),
        NamedField('bid1_volume', UInt, '买盘1挂单量'),
        NamedField('sell1_price', UInt, '卖盘1价格'),
        NamedField('sell1_volume', UInt, '卖盘1挂单量'),
        NamedField('bid_sell_5', BusinessLevel(5), '买卖五档'),
        NamedField('bid_sell_10', BusinessLevel(10), '买卖十档'),
        NamedField('change_percent', Int, '涨跌幅'),
        NamedField('change_value', Int, '涨跌值'),
        NamedField('price_digits', Byte, '价格类小数位'),
        NamedField('display_digits', Byte, '显示小数位'),
        NamedField('buyvol', LargeInt, '内盘'),
        NamedField('profit', Int, '收益'),
        NamedField('pe_ratio', Int, '市盈率'),
        NamedField('net_assets', Int, '净资产'),
        NamedField('pb_ratio', Int, '市净率'),
        NamedField('total_stocks', LargeInt, '总股本'),
        NamedField('hk_stocks', LargeInt, '港股本'),
        NamedField('total_value', LargeInt, '总值'),
        NamedField('hk_value', LargeInt, '港值'),
        NamedField('turnover_rate', Int, '换手率'),
        NamedField('reverse_currency', Int, '反向汇率'),
        NamedField('hgt_flag', Byte, '沪港通标志'),
        NamedField('52_week_highest', UInt, '52周最高价'),
        NamedField('52_week_lowest', UInt, '52周最低价'),
        NamedField('amplitude', Int, '振幅'),
    )

    attributes_info = (
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
    attributes_info = (
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
    all_fields = (
        NamedField('date', UInt, '日期'),
        NamedField('time', UInt, '时间'),
        NamedField('price', UInt, '价格'),
        NamedField('volume', LargeInt, '成交量'),
        NamedField('tickcount', UInt, '成交笔数'),
        NamedField('change', LargeInt, '持仓量变化'),
        NamedField('deal_type', Byte, '交易类型'),
        NamedField('flag', Byte, '开平')
    )

    attributes_info = (
        ('close_price', UInt, '昨收盘'),
        ('data_pos', UInt, '数据位置'),
        ('need_clear_local', Byte, '是否需要清除本地数据'),
        ('fields', ByteVector, '应答字段ID'),
        ('deal_data', VarFieldsVector(UShort, all_fields,
                     lambda self : self.fields,), '成交数据'),
    )

class Response_5504(message.Message, metaclass = ResponseMeta):
    data_type = message.create_class(
        'response_5504_data',
        (
            NamedField('nth_start_day', Byte, '开始相对的第几天'),
            NamedField('start_time', UShort, '开始时间'),
            NamedField('nth_end_day', Byte, '结束相对的第几天'),
            NamedField('end_time', UShort, '结束时间'),
        )
    )

    attributes_info = (
          NamedField('return_data', Vector(Byte, data_type), '返回数据'),
    )

class Response_5505(message.Message, metaclass = ResponseMeta):
    attributes_info = (
          NamedField('server_date', Int, '服务器日期'),
          NamedField('server_time', Int, '服务器时间'),
    )

class Response_5506(message.Message, metaclass = ResponseMeta):
    data_type = message.create_class(
        'Response_5506_data',
        (
            NamedField('market_id', UShort, '市场代码'),
            NamedField('security_id', String, '证券代码'),
            NamedField('security_name', GBKString, '证券名称'),
            NamedField('jian_pin', GBKString, '简拼'),
            NamedField('type', Byte, '所属类别'),
            NamedField('flag', Byte, '标示(0是新增或覆盖,1是删除)'),
            NamedField('inc_id', UInt, '增量ID'),
        )
    )

    attributes_info = (
        NamedField('result', Byte, '处理结果'),
        NamedField('server_inc_id', UInt, '服务器最新增量ID'),
        NamedField('return_data', Vector(UShort, data_type), '返回数据')
    )

class Response_5508(message.Message, metaclass = ResponseMeta):
    attributes_info = (
        NamedField('server_version', Int, '服务器版本号'),
    )

class Response_5509(message.Message, metaclass = ResponseMeta):
    attributes_info = (
        NamedField('online_num', Int, '在线数'),
    )

class Response_5510(message.Message, metaclass = ResponseMeta):
    attributes_info = (
        NamedField('start_date', Int, '启动日期'),
        NamedField('start_time', Int, '启动时间'),
    )

class PriceDisplay(message.BinaryObject):
    attributes_info = (
        NamedField('data_type', UShort, '字段类型'),
        NamedField('extend_data', Byte, '扩展字段类型'),
    )

    data_types = [ Int, LargeInt, LargeInt, String, Array(2, String) ]

    def __init__(self, *attrs, **kwargs):
        super().__init__(**kwargs)
        if attrs:
            self.data_type = UShort(attrs[0])
            self.extend_data = Byte(attrs[1])
            self.data = self.data_types[self.data_type & 0x07](attrs[2])

    def __repr__(self):
        r = super().__repr__()
        r += '\n\t数据类型: {}'.format(self.data_type & 0x07)
        r += '\n\t数据内容：{}'.format(self.data)
        r += '\n\t小数位数: {}'.format((self.data_type >> 3) & 0x07)
        r += '\n\t逻辑红绿白: {}'.format((self.data_type >> 6) & 0x03)
        r += '\n\t数值单位: {}'.format((self.data_type >> 8) & 0x0F)
        r += '\n\t单位乘除标识: {}'.format((self.data_type >> 12) & 0x01)
        r += '\n\t是否百分比: {}'.format((self.data_type >> 13) & 0x01)
        r += '\n\t是否有超链接: {}'.format((self.data_type >> 14) & 0x01)
        r += '\n\t预留1位: {}'.format((self.data_type >> 15) & 0x01)
        return r

    def tobytes(self):
        return super().tobytes() + self.data.tobytes()

    @classmethod
    def fromstream(cls, reader, **kwargs):
        self = super().fromstream(reader, **kwargs)

        self.data = reader.read_type(cls.data_types[self.data_type & 0x07])

        return self

class Response_5511(message.Message, metaclass = ResponseMeta):
    data_type = message.create_class(
        'Response_5511_datatype',
        (
            NamedField('index', Byte, '字段索引'),
            NamedField('name', String, '字段数据名称'),
            NamedField('data', PriceDisplay, '数据')
        )
    )

    attributes_info = (
        NamedField('stock_id', String, '代码唯一标识'),
        NamedField('pid', UShort, '协议标识'),
        NamedField('data', Vector(Byte, data_type), '返回字段')
    )

class Response_5512(message.Message, metaclass = ResponseMeta):
    attributes_info = (
        NamedField('pid', UShort, '协议标识'),
        NamedField('stock_id', String, '代码唯一标识'),
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
    attributes_info = (
        NamedField('pid', UShort, '协议标识'),
        NamedField('stock_id', String, '代码唯一标识'),
        NamedField('timerend', TimeTrend, '分时走势')
    )

class Response_5514(message.Message, metaclass = ResponseMeta):
    attributes_info = (
        NamedField('pid', UShort, '协议标识'),
        NamedField('stock_id', String, '代码唯一标识'),
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
    attributes_info = (
        NamedField('pid', UShort, '协议标识'),
        NamedField('stock_id', String, '代码唯一标识'),
        NamedField('req_num', Byte, '请求个数'),
        NamedField('buy_num', Byte, '买盘经纪队列个数'),
        NamedField('sell_num', Byte, '卖盘经纪队列个数'),
        NamedField('buy_queue', VarArray(lambda x : x.buy_num, UShort), '买盘经纪队列'),
        NamedField('sell_queue', VarArray(lambda x : x.sell_num, UShort), '卖盘经纪队列'),
    )

class Response_5516(message.Message, metaclass = ResponseMeta):
    attributes_info = (
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
    attributes_info = (
        NamedField('pid', UShort, '协议标识'),
    )

class Response_5518(message.Message, metaclass = ResponseMeta):
    attributes_info = (
                      NamedField('pid', UShort, '协议标识'),
                      NamedField('result', Byte, 'MD5校验结果'),
                      NamedField('md5', Array(33, Char), 'MD5')
                  )

class Push_5514(message.Message, metaclass = PushMeta):
    attributes_info = (
          NamedField('pid', UShort, '协议标识'),
          NamedField('stock_id', String, '代码唯一标识'),
          NamedField('fields', ByteVector, '应答字段ID'),
          NamedField('need_clear_local', Byte, '是否需要清除本地数据'),
          NamedField('total_num', UShort, '数据总数'),
          NamedField('return_num', UShort, '返回数据总数'),
    )

    unamed_fields_info = Response_5514.unamed_fields_info

class Push_5516(message.Message, metaclass = PushMeta):
    attributes_info = (
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

if __name__ == '__main__':
    r = Response_5515()
    r.pid = 1
    r.stock_id = 'HK|00700'
    r.req_num = 10
    r.buy_num = 5
    r.sell_num = 5
    r.buy_queue = [1, 2, 3, 4, 5]
    r.sell_queue = [1, 2, 3, 4, 5]
    r.tobytes()
