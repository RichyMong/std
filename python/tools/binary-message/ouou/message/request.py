from . import message
from .message import Attribute, Message, MessageMeta
from .const import *
from ..util import *

__all__ = [
            'Request_5501', 'Request_5502', 'Request_5503', 'Request_5504',
            'Request_5505', 'Request_5506', 'Request_5508',
            'Request_5509', 'Request_5510', 'Request_5511', 'Request_5512',
            'Request_5513', 'Request_5514', 'Request_5515', 'Request_5516',
            'Request_5517', 'Request_5518',
           ]

class RequestMeta(MessageMeta):
    def __new__(mcs, name, bases, attrs):
        cls = super().__new__(mcs, name, bases, attrs)

        if cls.MSG_ID in message.request_messages:
            raise RuntimeError('duplicated message type {} '.format(cls.MSG_ID))

        message.request_messages[cls.MSG_ID] = cls

        return cls

class Request_5500(message.MultipleMessage, metaclass = RequestMeta):
    pass

class Request_5501(Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('stock_id', String, '代码唯一标示'),
        Attribute('extra_data', Byte, '同时请求其他数据'),
        Attribute('fields', Vector(Byte, Byte), '请求字段'),
        Attribute('date_time', UInt, '请求开始时间'),
        Attribute('extra_fields', Vector(Byte, Byte), '请求附加字段')
    )

class Request_5502(Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('pid', Byte, '协议标识'),
        Attribute('sort_field', UShort, '排序字段'),
        Attribute('sort_method', Byte, '排序方式'),
        Attribute('start_pos', UShort, '起始位置'),
        Attribute('req_num', UShort, '请求个数'),
        Attribute('fields', ByteVector, '请求字段ID'),
        Attribute('req_type', Byte, '请求类型'),
        Attribute('content', Vector(UShort, String), '请求数据内容')
    )

    def __str__(self):
        r = super().__str__()
        if self.req_type == 4:
            for (i, s) in enumerate(self.content):
                v = int(s)
                r += ' {}|{}'.format(v >> 16, v & 0xff)
        else:
            r += ' ' + ' '.join(x for x in self.content)

        return r

class Request_5503(Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('data_pos', Int, '数据位置'),
        Attribute('date_time', UInt, '日期和时间'),
        Attribute('request_num', UShort, '请求条数'),
        Attribute('fields', Vector(Byte, Byte), '请求字段ID'),
    )

class Request_5504(Message, metaclass = RequestMeta):
    attributes_info = (
          Attribute('market_code', String, '市场代码唯一标示'),
    )

class Request_5505(Message, metaclass = RequestMeta):
    attributes_info = ()

class Request_5506(Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('increment_id', UInt, '增量ID'),
        Attribute('current_count', UShort, '客户端的现有个数')
    )

class Request_5508(Message, metaclass = RequestMeta):
    pass

class Request_5509(Message, metaclass = RequestMeta):
    pass

class Request_5510(Message, metaclass = RequestMeta):
    pass

class Request_5511(Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('version', Int, '接口版本'),
        Attribute('pid', UShort, '协议标识'),
        Attribute('push_type', Byte, '推送类型'),
        Attribute('extend_data', UShort, '扩展字段'),
    )

    def tobytes(self):
        # TODO: 5511 doesn't support push message now
        self.push_type = PUSH_TYPE_ONCE
        return super().tobytes()

class Request_5512(Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('push_type', Byte, '推送类型'),
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('fields', Vector(Byte, Byte), '请求字段ID'),
    )

class Request_5513(Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('push_type', Byte, '推送类型'),
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('fields', Vector(Byte, Byte), '请求字段ID'),
        Attribute('date_time', UInt, '请求开始时间')
    )

class Request_5514(Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('push_type', Byte, '推送类型'),
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('fields', Vector(Byte, Byte), '请求字段ID'),
        Attribute('flag', Byte, '请求标识'),
        Attribute('value', UInt, '请求最新根数或请求开始时间(HHmmss)'),
        Attribute('nr_of_roots', Int, '请求根数')
    )

class Request_5515(Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('push_type', Byte, '推送类型'),
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('number', Byte, '请求经纪队列个数')
    )

class Request_5516(Message, metaclass = RequestMeta):
    # The fields are almost the same as Request_5502 except pid and push_type.
    # However, sort_field is Byte here.
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('push_type', Byte, '推送类型'),
        Attribute('sort_field', Byte, '排序字段'),
        Attribute('sort_method', Byte, '排序方式'),
        Attribute('start_pos', UShort, '起始位置'),
        Attribute('req_num', UShort, '请求个数'),
        Attribute('fields', Vector(Byte, Byte), '请求字段ID'),
        Attribute('req_type', Byte, '请求类型'),
        Attribute('content', Vector(UShort, String), '请求数据内容')
    )

    def __str__(self):
        r = super().__str__()
        if self.req_type == 4:
            for (i, s) in enumerate(self.content):
                v = int(s)
                r += ' {}|{}'.format(v >> 16, v & 0xff)

        return r

class Request_5517(Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
    )

class Request_5518(Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('md5', CString(33), 'MD5校验')
    )

def analyze_hex(content):
    blob = bytearray.fromhex(content.replace('\n\t ', ''))
    if not len(blob):
        return

    print(Message.allfrombytes(blob))
