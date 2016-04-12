import zlib
import collections
from functools import reduce
from .import message
from .message import Attribute, BinaryObject, DependentAttribute, DependentFieldsAttribute
from .const import *
from ouou.util.stream import Reader
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

def VarFields(fields, iterfunc):
    '''
    Return a class which has attributes dependent on iterfunc. iterfunc should
    be a callable with one parameter as the object this class is embedded in.
    The fields is an iterable object of type Attribute.
    '''
    class Wrapper(list, DependentFieldsAttribute):
        @staticmethod
        def get_data_type(target):
            attributes_info = tuple(fields[x - 1] for x in iterfunc(target))
            return message.create_class('VarFieldsWrapper', attributes_info)

        @classmethod
        def fromstream(cls, reader, target, **kwargs):
            self = cls(**kwargs)
            self.target = target
            w = self.get_data_type(target).fromstream(reader)
            for v in w.attributes_info:
                self.append(getattr(w, v.name))
            return self


        def tobytes(self, target):
            return self.get_data_type(target)(self).tobytes()

        def __str__(self):
            s = str(self.get_data_type(self.target)(self))
            return s

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
            b = size_cls.pack(len(self))
            for x in self:
                '''
                Can not check if x is an instance of data_type here since
                create_class returns an unique class every time.
                '''
                if not isinstance(x, BinaryObject):
                    x = data_type(x)
                b += x.tobytes()

            return b

        def __str__(self):
            r = '共 {} 个'.format(len(self))
            for i, x in enumerate(self):
                r += '{p}{sep}{no}{sep}{p}{x}'.format(p = PRINT_PREFIX,
                         sep = 30 * '-', no = i + 1, x = x)
            return r

    return Wrapper

class PriceSeat(UInt):
    def __str__(self):
        price = self & 0X003FFFFF
        seat = (self >> 22) & 0XCFF
        return '{} : {}'.format(price, seat)

    def __str__(self):
        price = self & 0X003FFFFF
        seat = (self >> 22) & 0XCFF
        return '{:.{}f} : {}'.format(price / 1000.0, 4, seat)

def BusinessLevel(level):
    class OrderDetail(BinaryObject):
        attributes_info = (
            Attribute('price_seat', PriceSeat, '价格席位'),
            Attribute('volume', LargeInt, '成交量'),
        )

        def __str__(self):
            return '价格席位 {}, 成交量 {}'.format(self.price_seat, self.volume)

    class WrappedBusinessLevel(Array(2 * level, OrderDetail)):
        def __str__(self):
            r = ''
            for i, detail in enumerate(self):
                r += PRINT_PREFIX
                if i < level:
                    r += '\t卖{} - {}'.format(CHINSE_NUMBER[level - i], detail)
                else:
                    r += '\t买{} - {}'.format(CHINSE_NUMBER[i + 1 - level], detail)

            return r

    return WrappedBusinessLevel

class ResponseMeta(message.MessageMeta):
    '''
    The meta class for response messages. The according class is supposed to
    define a 'MSG_ID' class attribute and it will be registered to process
    response messages of this type. If there's no class registered to process
    push messages of this type, we will also register it in push messages.
    '''
    def __new__(mcs, name, bases, attrs):
        cls = super().__new__(mcs, name, bases, attrs)

        message.response_messages[cls.MSG_ID] = cls

        # Be careful not to override the push message already registered.
        if cls.MSG_ID not in message.push_messages:
            message.push_messages[cls.MSG_ID] = cls

        return cls

class PushMeta(message.MessageMeta):
    '''
    The meta class for push messages. If not provided, ResponseMessageMeta
    will register the response message class as default.
    '''
    def __new__(mcs, name, bases, attrs):
        cls = super().__new__(mcs, name, bases, attrs)

        if cls.MSG_ID in message.push_messages and type(message.push_messages) is PushMeta:
            raise RuntimeError('duplicated push message {} '.format(cls.MSG_ID))

        message.push_messages[cls.MSG_ID] = cls

        return cls

class TimeTrend(message.BinaryObject):
    '''
    Used by response 5501 and 5513.
    '''
    all_fields_info = (
        Attribute('time_data', Int, '时间'),
        Attribute('open_price', UInt, '开盘价'),
        Attribute('highest_price', UInt, '最高价'),
        Attribute('lowest_price', UInt, '最低价'),
        Attribute('close_price', UInt, '分钟收盘价'),
        Attribute('avg_price', UInt, '平均价'),
        Attribute('volume', LargeInt, '成交量'),
        Attribute('amount', LargeInt, '成交额'),
        Attribute('tick_count', UInt, '成交笔数'),
        Attribute('sellvol', LargeInt, '外盘'),
        Attribute('inventory', LargeInt, '持仓量'),
        Attribute('inventory_change', LargeInt, '持仓量变化'),
        Attribute('settlement_price', UInt, '结算价'),
        Attribute('buyvol', LargeInt, '内盘')
    )

    attributes_info = (
            Attribute('fields', ByteVector, '应答字段ID'),
            Attribute('need_clear_local', Byte, '是否需要清除本地数据'),
            Attribute('total_num', UShort, '数据总数'),
            Attribute('return_data', VarFieldsVector(UShort, all_fields_info,
                              lambda self : self.fields), '分时走势数据')
        )

class TimeSharing(message.BinaryObject):
    '''
    Used by response 5503 and 5514.
    '''
    all_fields_info = (
        Attribute('time_data', Int, '时间'),
        Attribute('open_price', UInt, '开盘价'),
        Attribute('highest_price', UInt, '最高价'),
        Attribute('lowest_price', UInt, '最低价'),
        Attribute('close_price', UInt, '分钟收盘价'),
        Attribute('avg_price', UInt, '平均价'),
        Attribute('volume', LargeInt, '成交量'),
        Attribute('amount', LargeInt, '成交额'),
        Attribute('tick_count', UInt, '成交笔数'),
        Attribute('sellvol', LargeInt, '外盘'),
        Attribute('inventory', LargeInt, '持仓量'),
        Attribute('inventory_change', LargeInt, '持仓量变化'),
        Attribute('settlement_price', UInt, '结算价'),
        Attribute('buyvol', LargeInt, '内盘')
    )

    attributes_info = (
        Attribute('fields', ByteVector, '应答字段ID'),
        Attribute('need_clear_local', Byte, '是否需要清除本地数据'),
        Attribute('total_num', UShort, '数据总数'),
        Attribute('return_data', VarFieldsVector(UShort, all_fields_info,
                           lambda self : self.fields), '返回数据'),
    )


class Response_5500(message.MultipleMessage, metaclass = ResponseMeta):
    pass

class Response_5501(message.Message, metaclass = ResponseMeta):
    quotation_response_fields = (
        Attribute('date_time', Int, '行情日期和时间'),
        Attribute('stock_id', String, '唯一标识'),
        Attribute('stock_name', GBKString, '股票名称'),
        Attribute('stock_code', String, '股票代码'),
        Attribute('market_code', UShort, '市场代码'),
        Attribute('market_type', UShort, '市场类别'),
        Attribute('trade_flag', Byte, '交易标示'),
        Attribute('close_price', UInt, '昨收价'),
        Attribute('open_price', DigitInt(3), '开盘价'),
        Attribute('highest_price', DigitInt(3), '最高价'),
        Attribute('lowest_price', DigitInt(3), '最低价'),
        Attribute('latest_price', DigitInt(3), '最新价'),
        Attribute('volume', LargeInt, '成交量'),
        Attribute('amount', LargeInt, '成交额'),
        Attribute('trade_count', UInt, '成交笔数'),
        Attribute('sellvol', LargeInt, '外盘'),
        Attribute('newvol', LargeInt, '现手'),
        Attribute('newvol_direction', Byte, '现手方向'),
        Attribute('open_interest_change', LargeInt, '持仓量变化'),
        Attribute('avg_price', DigitInt(4), '均价'),
        Attribute('open_interest', LargeInt, '持仓量'),
        Attribute('settlement_price', UInt, '今结算价'),
        Attribute('last_open_interest', LargeInt, '昨持仓量'),
        Attribute('last_settlement_price', UInt, '昨结算价'),
        Attribute('limit_up_price', UInt, '涨停价'),
        Attribute('limit_down_price', UInt, '跌停价'),
        Attribute('bid1_price', UInt, '买盘1价格'),
        Attribute('bid1_volume', UInt, '买盘1挂单量'),
        Attribute('sell1_price', UInt, '卖盘1价格'),
        Attribute('sell1_volume', UInt, '卖盘1挂单量'),
        Attribute('bid_sell_5', BusinessLevel(5), '买卖五档'),
        Attribute('bid_sell_10', BusinessLevel(10), '买卖十档'),
        Attribute('change_percent', DigitInt(2), '涨跌幅'),
        Attribute('change_value', Int, '涨跌值'),
        Attribute('price_digits', Byte, '价格类小数位'),
        Attribute('display_digits', Byte, '显示小数位'),
        Attribute('buyvol', LargeInt, '内盘'),
        Attribute('profit', DigitInt(3), '收益'),
        Attribute('pe_ratio', DigitInt(3), '市盈率'),
        Attribute('net_assets', DigitInt(3), '净资产'),
        Attribute('pb_ratio', DigitInt(3), '市净率'),
        Attribute('total_stocks', LargeInt, '总股本'),
        Attribute('hk_stocks', LargeInt, '港股本'),
        Attribute('total_value', LargeInt, '总值'),
        Attribute('hk_value', LargeInt, '港值'),
        Attribute('turnover_rate', DigitInt(4), '换手率'),
        Attribute('reverse_currency', Int, '反向汇率'),
        Attribute('hgt_flag', Byte, '沪港通标志'),
        Attribute('52_week_highest', UInt, '52周最高价'),
        Attribute('52_week_lowest', UInt, '52周最低价'),
        Attribute('amplitude', DigitInt(2), '振幅'),
    )

    attributes_info = (
        Attribute('request_extra_data', Byte, '同时请求其他数据'),
        Attribute('fields', ByteVector, '应答字段'),
        Attribute('return_data', VarFields(quotation_response_fields,
                            lambda self : self.fields), ''),
        Attribute('extra_data', VarArray(lambda self : 1 if self.request_extra_data else 0, TimeTrend),
                             '同时请求走势/分时成交/K线附加数据'),
    )

class Response_5502(message.Message, metaclass = ResponseMeta):
    attributes_info = (
        Attribute('pid', Byte, '协议标识'),
        Attribute('fields', ByteVector, '返回字段ID'),
        Attribute('total_num', UShort, '总数据个数'),
        Attribute('return_data', VarFieldsVector(UShort, Response_5501.quotation_response_fields,
                          lambda self : self.fields), '返回数据')
    )

class Response_5503(message.Message, metaclass = ResponseMeta):
    all_fields = (
        Attribute('date', UInt, '日期'),
        Attribute('time', UInt, '时间'),
        Attribute('price', UInt, '价格'),
        Attribute('volume', LargeInt, '成交量'),
        Attribute('tickcount', UInt, '成交笔数'),
        Attribute('change', LargeInt, '持仓量变化'),
        Attribute('deal_type', Byte, '交易类型'),
        Attribute('flag', Byte, '开平')
    )

    attributes_info = (
        Attribute('close_price', UInt, '昨收盘'),
        Attribute('data_pos', UInt, '数据位置'),
        Attribute('need_clear_local', Byte, '是否需要清除本地数据'),
        Attribute('fields', ByteVector, '应答字段ID'),
        Attribute('deal_data', VarFieldsVector(UShort, all_fields,
                     lambda self : self.fields,), '成交数据'),
    )

class Response_5504(message.Message, metaclass = ResponseMeta):
    data_type = message.create_class(
        'response_5504_data',
        (
            Attribute('nth_start_day', Byte, '开始相对的第几天'),
            Attribute('start_time', UShort, '开始时间'),
            Attribute('nth_end_day', Byte, '结束相对的第几天'),
            Attribute('end_time', UShort, '结束时间'),
        )
    )

    attributes_info = (
          Attribute('return_data', Vector(Byte, data_type), '返回数据'),
    )

class Response_5505(message.Message, metaclass = ResponseMeta):
    attributes_info = (
          Attribute('server_date', Int, '服务器日期'),
          Attribute('server_time', Int, '服务器时间'),
    )

class Response_5506(message.Message, metaclass = ResponseMeta):
    data_type = message.create_class(
        'Response_5506_data',
        (
            Attribute('market_id', UShort, '市场代码'),
            Attribute('security_id', String, '证券代码'),
            Attribute('security_name', GBKString, '证券名称'),
            Attribute('jian_pin', GBKString, '简拼'),
            Attribute('type', Byte, '所属类别'),
            Attribute('flag', Byte, '标示(0是新增或覆盖,1是删除)'),
            Attribute('inc_id', UInt, '增量ID'),
        )
    )

    attributes_info = (
        Attribute('result', Byte, '处理结果'),
        Attribute('server_inc_id', UInt, '服务器最新增量ID'),
        Attribute('return_data', Vector(UShort, data_type), '返回数据')
    )

class Response_5508(message.Message, metaclass = ResponseMeta):
    attributes_info = (
        Attribute('server_version', Int, '服务器版本号'),
    )

class Response_5509(message.Message, metaclass = ResponseMeta):
    attributes_info = (
        Attribute('online_num', Int, '在线数'),
    )

class Response_5510(message.Message, metaclass = ResponseMeta):
    attributes_info = (
        Attribute('start_date', Int, '启动日期'),
        Attribute('start_time', Int, '启动时间'),
    )

class Response_5511(message.Message, metaclass = ResponseMeta):
    class Data(message.BinaryObject):
        all_fields_info = (
            Attribute('date_time', Int, '行情日期和时间'),
            Attribute('stock_name', GBKString, '股票名称'),
            Attribute('stock_code', String, '股票代码'),
            Attribute('market_name', GBKString, '市场名称'),
            Attribute('market_code', UShort, '市场代码'),
            Attribute('market_type', UShort, '市场类别'),
            Attribute('trade_flag', String, '交易标示'),
            Attribute('close_price', UInt, '昨收价'),
            Attribute('open_price', UInt, '开盘价'),
            Attribute('highest_price', UInt, '最高价'),
            Attribute('lowest_price', UInt, '最低价'),
            Attribute('latest_price', Int, '最新价'),
            Attribute('volume', LargeInt, '成交量'),
            Attribute('amount', LargeInt, '成交额'),
            Attribute('trade_count', UInt, '成交笔数'),
            Attribute('sellvol', LargeInt, '外盘'),
            Attribute('newvol', LargeInt, '现手'),
            Attribute('newvol_direction', String, '现手方向'),
            Attribute('open_interest_change', LargeInt, '持仓量变化'),
            Attribute('avg_price', UInt, '均价'),
            Attribute('open_interest', LargeInt, '持仓量'),
            Attribute('settlement_price', UInt, '今结算价'),
            Attribute('last_open_interest', LargeInt, '昨持仓量'),
            Attribute('last_settlement_price', UInt, '昨结算价'),
            Attribute('limit_up_price', UInt, '涨停价'),
            Attribute('limit_down_price', UInt, '跌停价'),
            Attribute('bid1_price', UInt, '买盘1价格'),
            Attribute('bid1_volume', UInt, '买盘1挂单量'),
            Attribute('sell1_price', UInt, '卖盘1价格'),
            Attribute('sell1_volume', UInt, '卖盘1挂单量'),
            Attribute('bid_sell_5', BusinessLevel(5), '买卖五档'),
            Attribute('bid_sell_10', BusinessLevel(10), '买卖十档'),
            Attribute('change_percent', DigitInt(2), '涨跌幅'),
            Attribute('change_value', Int, '涨跌值'),
            Attribute('buyvol', LargeInt, '内盘'),
            Attribute('profit', DigitInt(3), '收益'),
            Attribute('pe_ratio', DigitInt(3), '市盈率'),
            Attribute('net_assets', DigitInt(3), '净资产'),
            Attribute('pb_ratio', DigitInt(3), '市净率'),
            Attribute('total_stocks', LargeInt, '总股本'),
            Attribute('hk_stocks', LargeInt, '港股本'),
            Attribute('total_value', LargeInt, '总值'),
            Attribute('hk_value', LargeInt, '港值'),
            Attribute('turnover_rate', DigitInt(2), '换手率'),
            Attribute('reverse_currency', Int, '反向汇率'),
            Attribute('hgt_flag', Byte, '沪港通标志'),
            Attribute('52_week_highest', UInt, '52周最高价'),
            Attribute('52_week_lowest', UInt, '52周最低价'),
            Attribute('amplitude', DigitInt(2), '振幅'),
            Attribute('volume_ratio', DigitInt(2), '量比'),
            Attribute('ps_ratio', DigitInt(2), '市销率'),
            Attribute('roe', DigitInt(2), 'ROE'),
        )

        attributes_info = (
            Attribute('index', Byte, '字段索引'),
            Attribute('name', GBKString, '字段数据名称'),
            Attribute('data_type', UShort, '字段类型'),
            Attribute('extend_data', Byte, '扩展字段类型'),
            # The type of the data is decided by the lowest 3 bits of
            # data_type. So we can only get it at runtime.
        )

        data_types = [ Int, LargeInt, LargeInt, GBKString,
                       Array(2, GBKString) ]

        def __init__(self, attrs = None, **kwargs):
            super().__init__(**kwargs)
            if attrs:
                self.index = attrs[0]
                self.name = attrs[1]
                self.data_type = attrs[2]
                self.extend_data = attrs[3]
                self.data = self.data_types[self.data_type & 0x07](attrs[4])

        def __str__(self):
            r = super().__str__()
            r += PRINT_PREFIX + '数据类型: {}'.format(self.data_type & 0x07)
            r += PRINT_PREFIX + '数据内容：{}'.format(self.data)
            r += PRINT_PREFIX + '小数位数: {}'.format((self.data_type >> 3) & 0x07)
            r += PRINT_PREFIX + '逻辑红绿白: {}'.format((self.data_type >> 6) & 0x03)
            r += PRINT_PREFIX + '数值单位: {}'.format((self.data_type >> 8) & 0x0F)
            r += PRINT_PREFIX + '单位乘除标识: {}'.format((self.data_type >> 12) & 0x01)
            r += PRINT_PREFIX + '是否百分比: {}'.format((self.data_type >> 13) & 0x01)
            r += PRINT_PREFIX + '是否有超链接: {}'.format((self.data_type >> 14) & 0x01)
            r += PRINT_PREFIX + '预留1位: {}'.format((self.data_type >> 15) & 0x01)
            return r

        def tobytes(self):
            return super().tobytes() + self.data.tobytes()

        def extra_parse(self, reader, **kwargs):
            attr = self.all_fields_info[self.index - 1]
            cls = self.data_types[self.data_type & 0x07]
            if issubclass(attr.type, cls):
                self.data = reader.read_type(attr.type)
            else:
                self.data = reader.read_type(cls)

    attributes_info = (
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('pid', UShort, '协议标识'),
        Attribute('data', Vector(Byte, Data), '返回字段')
    )

class Response_5512(message.Message, metaclass = ResponseMeta):
    all_fields_info = (
        Attribute('bid10_price', PriceSeat, '卖十价格'),
        Attribute('bid9_volume', LargeInt, '卖十量'),
        Attribute('bid9_price', PriceSeat, '卖九价格'),
        Attribute('bid8_volume', LargeInt, '卖九量'),
        Attribute('bid8_price', PriceSeat, '卖八价格'),
        Attribute('bid7_volume', LargeInt, '卖八量'),
        Attribute('bid7_price', PriceSeat, '卖七价格'),
        Attribute('bid6_volume', LargeInt, '卖七量'),
        Attribute('bid6_price', PriceSeat, '卖六价格'),
        Attribute('bid5_volume', LargeInt, '卖六量'),
        Attribute('bid5_price', PriceSeat, '卖五价格'),
        Attribute('bid4_volume', LargeInt, '卖五量'),
        Attribute('bid4_price', PriceSeat, '卖四价格'),
        Attribute('bid3_volume', LargeInt, '卖四量'),
        Attribute('bid3_price', PriceSeat, '卖三价格'),
        Attribute('bid2_volume', LargeInt, '卖三量'),
        Attribute('bid2_price', PriceSeat, '卖二价格'),
        Attribute('bid1_volume', LargeInt, '卖二量'),
        Attribute('bid1_price', PriceSeat, '卖一价格'),
        Attribute('bid0_volume', LargeInt, '卖一量'),
        Attribute('ask10_price', PriceSeat, '买十价格'),
        Attribute('ask10_volume', LargeInt, '买十量'),
        Attribute('ask9_price', PriceSeat, '买九价格'),
        Attribute('ask9_volume', LargeInt, '买九量'),
        Attribute('ask8_price', PriceSeat, '买八价格'),
        Attribute('ask8_volume', LargeInt, '买八量'),
        Attribute('ask7_price', PriceSeat, '买七价格'),
        Attribute('ask7_volume', LargeInt, '买七量'),
        Attribute('ask6_price', PriceSeat, '买六价格'),
        Attribute('ask6_volume', LargeInt, '买六量'),
        Attribute('ask5_price', PriceSeat, '买五价格'),
        Attribute('ask5_volume', LargeInt, '买五量'),
        Attribute('ask4_price', PriceSeat, '买四价格'),
        Attribute('ask4_volume', LargeInt, '买四量'),
        Attribute('ask3_price', PriceSeat, '买三价格'),
        Attribute('ask3_volume', LargeInt, '买三量'),
        Attribute('ask2_price', PriceSeat, '买二价格'),
        Attribute('ask2_volume', LargeInt, '买二量'),
        Attribute('ask1_price', PriceSeat, '买一价格'),
        Attribute('ask1_volume', LargeInt, '买一量'),
        Attribute('bid_ask_5', BusinessLevel(5), '买卖五档'),
        Attribute('bid_ask_10', BusinessLevel(10), '买卖十档'),
        Attribute('bid_ask_1', BusinessLevel(1), '买卖一档'),
        Attribute('date_time', Int, '行情日期和时间'),
        Attribute('stock_id', String, '唯一标识'),
        Attribute('stock_name', GBKString, '股票名称'),
        Attribute('stock_code', String, '股票代码'),
        Attribute('market_code', UShort, '市场代码'),
        Attribute('market_type', UShort, '市场类别'),
        Attribute('trade_flag', Byte, '交易标示'),
        Attribute('close_price', UInt, '昨收价'),
        Attribute('open_price', UInt, '开盘价'),
        Attribute('highest_price', UInt, '最高价'),
        Attribute('lowest_price', UInt, '最低价'),
        Attribute('latest_price', Int, '最新价'),
        Attribute('volume', LargeInt, '成交量'),
        Attribute('amount', LargeInt, '成交额'),
        Attribute('trade_count', UInt, '成交笔数'),
        Attribute('sellvol', LargeInt, '外盘'),
        Attribute('newvol', LargeInt, '现手'),
        Attribute('newvol_direction', Byte, '现手方向'),
        Attribute('open_interest_change', LargeInt, '持仓量变化'),
        Attribute('avg_price', Int, '均价'),
        Attribute('open_interest', LargeInt, '持仓量'),
        Attribute('settlement_price', UInt, '今结算价'),
        Attribute('last_open_interest', LargeInt, '昨持仓量'),
        Attribute('last_settlement_price', UInt, '昨结算价'),
        Attribute('limit_up_price', UInt, '涨停价'),
        Attribute('limit_down_price', UInt, '跌停价'),
        Attribute('change_price_percent', DigitInt(2), '涨跌幅'),
        Attribute('change_price', Int, '涨跌值'),
        Attribute('price_digits', Byte, '价格类小数位'),
        Attribute('display_digits', Byte, '显示小数位'),
        Attribute('buyvol', LargeInt, '内盘'),
        Attribute('profit', DigitInt(3), '收益'),
        Attribute('pe_ratio', DigitInt(3), '市盈率'),
        Attribute('net_assets', DigitInt(3), '净资产'),
        Attribute('pb_ratio', DigitInt(3), '市净率'),
        Attribute('total_share', LargeInt, '总股本'),
        Attribute('hk_share', LargeInt, '港股本'),
        Attribute('total_value', LargeInt, '总值'),
        Attribute('hk_value', LargeInt, '港值'),
        Attribute('turnover_rate', DigitInt(4), '换手率'),
        Attribute('reverse_currency', Int, '反向汇率'),
        Attribute('hgt_flag', Byte, '沪港通标志'),
        Attribute('52_week_highest', UInt, '52周最高价'),
        Attribute('52_week_lowest', UInt, '52周最低价'),
        Attribute('delta', DigitInt(3), '振幅'),
    )

    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('fields', ByteVector, '应答字段ID'),
        Attribute('return_data', VarFields(all_fields_info,
                       lambda self : self.fields), ''),
    )

class Response_5513(message.Message, metaclass = ResponseMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('timerend', TimeTrend, '分时走势'),
    )

class Response_5514(message.Message, metaclass = ResponseMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('close_price', UInt, '昨收盘'),
        Attribute('real_digit', Byte, '昨收盘实际小数位'),
        Attribute('show_digit', Byte, '昨收盘显示小数位'),
        Attribute('fields', ByteVector, '应答字段ID'),
        Attribute('need_clear_local', Byte, '是否需要清除本地数据'),
        Attribute('data_pos', UInt, '数据位置'),
        Attribute('total_num', UShort, '数据总数'),
        Attribute('deal_data', VarFieldsVector(UShort, Response_5503.all_fields,
                         lambda self : self.fields,), '成交数据'),
    )

class Response_5515(message.Message, metaclass = ResponseMeta):
    def StockQueue(countfunc):
        class QueueData(BinaryObject):
            attributes_info = (
                Attribute('flag', Char, '标记'),
                Attribute('broker_no', UShort, '经纪号'),
            )

        class Wrapper(VarArray(countfunc, QueueData)):
            def __str__(self):
                level = 1
                levels = collections.defaultdict(list)
                for i, x in enumerate(self):
                    if x.flag == 'B':
                        levels[level].append(x.broker_no)
                    else:
                        level += 1

                r = ''
                for i in sorted(levels.keys()):
                    beg = '\t第{}档: '.format(CHINSE_NUMBER[i])
                    r += PRINT_PREFIX + beg
                    for j in range(0, len(levels[i]), 8):
                        if j: r += PRINT_PREFIX + len(beg.encode()) * ' '
                        r += ' '.join(str(v) for v in levels[i][j:j+8])

                return r

        return Wrapper


    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('req_num', Byte, '请求个数'),
        Attribute('buy_num', Byte, '买盘经纪队列个数'),
        Attribute('sell_num', Byte, '卖盘经纪队列个数'),
        Attribute('buy_queue', StockQueue(lambda x : x.sell_num), '买盘经纪队列'),
        Attribute('sell_queue', StockQueue(lambda x : x.sell_num), '卖盘经纪队列'),
    )

class Response_5516(message.Message, metaclass = ResponseMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('fields', ByteVector, '返回字段ID'),
        Attribute('total_num', UShort, '总数据个数'),
        Attribute('return_data', VarFieldsVector(UShort,
                        Response_5501.quotation_response_fields,
                        lambda self : self.fields), '返回数据'),
    )

class Response_5517(message.Message, metaclass = ResponseMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
    )

class Response_5518(message.Message, metaclass = ResponseMeta):
    class BrokerData(BinaryObject):
        broker_data_type = message.create_class(
            'broker_data',
            (
                Attribute('broker_no', UShort, '经纪号'),
                Attribute('broker_name', CString(32), '中文名'),
            )
        )

        attributes_info = (
            Attribute('md5', CString(33), 'MD5'),
            Attribute('raw_size', Int, '文件原始长度'),
            Attribute('compressed_data', Vector(Int, Byte), '压缩后数据'),
        )

        def extra_parse(self, reader):
            b = bytearray(self.compressed_data)
            data = zlib.decompress(b)
            assert len(data) % 34 == 0
            num = len(data) // 34
            reader = Reader(data)
            self.broker_data = Array(num, self.broker_data_type).fromstream(reader)

        def __str__(self):
            r = 'MD5: {}'.format(self.md5)
            name_no = collections.defaultdict(list)
            for i, x in enumerate(self.broker_data):
                r += PRINT_PREFIX + '{} {}'.format(x.broker_no, x.broker_name)
            return r


    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('result', Byte, 'MD5校验结果'),
        Attribute('broker_data', VarArray(lambda self : 1 if self.result else 0,
                        BrokerData), '经纪行数据')
    )

class Push_5514(message.Message, metaclass = PushMeta):
    attributes_info = (
          Attribute('pid', UShort, '协议标识'),
          Attribute('stock_id', String, '代码唯一标识'),
          Attribute('fields', ByteVector, '应答字段ID'),
          Attribute('need_clear_local', Byte, '是否需要清除本地数据'),
          Attribute('total_num', UShort, '数据总数'),
          Attribute('return_num', UShort, '返回数据总数'),
    )

class Push_5516(message.Message, metaclass = PushMeta):
    class Data(BinaryObject):
        attributes_info = (
            Attribute('fields', ByteVector, '返回字段'),
            Attribute('fields_data', VarFields(Response_5501.quotation_response_fields,
                            lambda self : self.fields), '')
        )

    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('total_num', UShort, '总数据个数'),
        Attribute('return_data', Vector(UShort, Data), '推送数据'),
    )
