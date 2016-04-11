from . import message
from .message import Attribute
from .const import *
from ouou.util import *


__all__ = [ 'MultipleMessage',
            'Request_5501', 'Request_5502', 'Request_5503', 'Request_5504',
            'Request_5505', 'Request_5506', 'Request_5508',
            'Request_5509', 'Request_5510', 'Request_5511', 'Request_5512',
            'Request_5513', 'Request_5514', 'Request_5515', 'Request_5516',
            'Request_5517', 'Request_5518',
           ]


class RequestMeta(message.MessageMeta):
    def __new__(mcs, name, bases, attrs):
        cls = super().__new__(mcs, name, bases, attrs)

        if cls.msg_id in message.request_messages:
            raise RuntimeError('duplicated message type {} '.format(cls.msg_id))

        message.request_messages[cls.msg_id] = cls

        return cls


class MultipleMessage(message.Message, metaclass = RequestMeta):
    msg_id = PKG_TYPE_MULTI

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.attributes_info = []

    def add_message(self, message):
        field = 'message_{}'.format(len(self.attributes_info) + 1)
        self.attributes_info.append((field, type(message), field))
        setattr(self, field, message)

class Request_5501(message.Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('stock_id', String, '代码唯一标示'),
        Attribute('extra_data', Byte, '同时请求其他数据'),
        Attribute('fields', Vector(Byte, Byte), '请求字段'),
        Attribute('date_time', UInt, '请求开始时间'),
        Attribute('extra_fields', Vector(Byte, Byte), '请求附加字段')
    )



class Request_5502(message.Message, metaclass = RequestMeta):
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



class Request_5503(message.Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('data_pos', Int, '数据位置'),
        Attribute('date_time', UInt, '日期和时间'),
        Attribute('request_num', UShort, '请求条数'),
        Attribute('fields', Vector(Byte, Byte), '请求字段ID'),
    )


class Request_5504(message.Message, metaclass = RequestMeta):
    attributes_info = (
          Attribute('market_code', String, '市场代码唯一标示'),
    )


class Request_5505(message.Message, metaclass = RequestMeta):
    attributes_info = ()


class Request_5506(message.Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('increment_id', UInt, '增量ID'),
        Attribute('current_count', UShort, '客户端的现有个数')
    )


class Request_5508(message.Message, metaclass = RequestMeta):
    pass


class Request_5509(message.Message, metaclass = RequestMeta):
    pass


class Request_5510(message.Message, metaclass = RequestMeta):
    pass


class Request_5511(message.Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('version', Int, '接口版本'),
        Attribute('pid', UShort, '协议标识'),
        Attribute('push_type', Byte, '推送类型'),
        Attribute('extend_data', UShort, '扩展字段'),
    )


class Request_5512(message.Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('push_type', Byte, '推送类型'),
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('fields', Vector(Byte, Byte), '请求字段ID'),
    )


class Request_5513(message.Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('push_type', Byte, '推送类型'),
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('fields', Vector(Byte, Byte), '请求字段ID'),
        Attribute('date_time', UInt, '请求开始时间')
    )


class Request_5514(message.Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('push_type', Byte, '推送类型'),
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('fields', Vector(Byte, Byte), '请求字段ID'),
        Attribute('flag', Byte, '请求标识'),
        Attribute('value', UInt, '请求最新根数或请求开始时间(HHmmss)'),
        Attribute('nr_of_roots', Int, '请求根数')
    )


class Request_5515(message.Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('push_type', Byte, '推送类型'),
        Attribute('stock_id', String, '代码唯一标识'),
        Attribute('number', Byte, '请求经纪队列个数')
    )


class Request_5516(message.Message, metaclass = RequestMeta):
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
        else:
            r += ' ' + ' '.join(x for x in self.content)

        return r


class Request_5517(message.Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
    )


class Request_5518(message.Message, metaclass = RequestMeta):
    attributes_info = (
        Attribute('pid', UShort, '协议标识'),
        Attribute('md5', Array(33, Char), 'MD5校验')
    )


def analyze_hex(content):
    blob = bytearray.fromhex(content.replace('\n\t ', ''))
    if not len(blob):
        return

    print(message.Message.allfrombytes(blob))


if __name__ == '__main__':
    # 5506
    analyze_hex('7b8815820020000100020800484b7c3030373030122a2c2d2e2f303132333435363738393a3b3c7d')
    # 5512
    analyze_hex('7b8815820020000100020800484b7c3030373030122a2c2d2e2f303132333435363738393a3b3c7d')
    # 5513
    analyze_hex('7b8915820020000100010800484b7c30303730300e0102030405060708090a0b0c0d0eaa848f5f7d')
    # 5514
    analyze_hex('7b8a1582001f000100010800484b7c303037303008010203040506070801a0860100102700007d')
    # 5515
    analyze_hex('7b8b1582000e000100010800484b7c3030373030147d')
    # 5516
    analyze_hex('7b8c15820036000100010d0100000a0008030405090a0b0c0d0103000800484b7c30303730300800484b7c30303030310b004e41534441517c4141504c7d')
    # 5518
    analyze_hex('7b8e158200230001006161610000000000000000000000000000000000000000000000000000000000007d')
