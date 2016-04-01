from . import message
from emoney.util import *


__all__ = [
            'Request_5501', # 'Request_5502', 'Request_5503', 'Request_5504',
            # 'Request_5505', 'Request_5506', 'Request_5507', 'Request_5508',
            # 'Request_5509', 'Request_5510', 'Request_5511',
            'Request_5512', 'Request_5513', 'Request_5514', 'Request_5515', 'Request_5516',
            'Request_5517', 'Request_5518',
           ]


class RequestMeta(message.BinaryObjectMeta):
    def __new__(mcs, name, bases, attrs):
        cls = super().__new__(mcs, name, bases, attrs)

        if cls.msg_id in message.request_messages:
            raise RuntimeError('duplicated message type {} '.format(cls.msg_id))

        message.request_messages[cls.msg_id] = cls

        return cls


class Request_5501(message.Message, metaclass = RequestMeta):
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


class Request_5512(message.Message, metaclass = RequestMeta):
    msg_id = 5512

    fields_info = (
                      ('pid', UShort, '协议标识'),
                      ('push_type', Byte, '推送类型'),
                      ('stockid', String, '代码唯一标识'),
                      ('fields', Vector(Byte, Byte), '请求字段ID')
                  )


class Request_5513(message.Message, metaclass = RequestMeta):
    msg_id = 5513

    fields_info = (
                      ('pid', UShort, '协议标识'),
                      ('push_type', Byte, '推送类型'),
                      ('stockid', String, '代码唯一标识'),
                      ('fields', Vector(Byte, Byte), '请求字段ID'),
                      ('date_time', UInt, '请求开始时间')
                  )


class Request_5514(message.Message, metaclass = RequestMeta):
    msg_id = 5514

    fields_info = (
                      ('pid', UShort, '协议标识'),
                      ('push_type', Byte, '推送类型'),
                      ('stockid', String, '代码唯一标识'),
                      ('fields', Vector(Byte, Byte), '请求字段ID'),
                      ('flag', Byte, '请求标识'),
                      ('value', UInt, '请求最新根数或请求开始时间(HHmmss)'),
                      ('nr_of_roots', Int, '请求根数')
                  )


class Request_5515(message.Message, metaclass = RequestMeta):
    msg_id = 5515

    fields_info = (
                      ('pid', UShort, '协议标识'),
                      ('push_type', Byte, '推送类型'),
                      ('stockid', String, '代码唯一标识'),
                      ('number', Byte, '请求经纪队列个数')
                  )


class Request_5516(message.Message, metaclass = RequestMeta):
    msg_id = 5516

    fields_info = (
                      ('pid', UShort, '协议标识'),
                      ('push_type', Byte, '推送类型'),
                      ('sort_field', Byte, '排序字段'),
                      ('sort_method', Byte, '排序方式'),
                      ('start_pos', UShort, '起始位置'),
                      ('req_num', UShort, '请求个数'),
                      ('fields', Vector(Byte, Byte), '请求字段ID'),
                      ('req_type', Byte, '请求类型'),
                      ('content', Vector(UShort, String), '请求数据内容')
                  )

    def __repr__(self):
        r = super().__repr__()
        if self.req_type == 4:
            for (i, s) in enumerate(self.content):
                v = int(s)
                r += ' {}|{}'.format(v >> 16, v & 0xff)
        else:
            r += ' ' + ' '.join(x for x in self.content)

        return r


class Request_5517(message.Message, metaclass = RequestMeta):
    msg_id = 5517

    fields_info = (
                      ('pid', UShort, '协议标识'),
                  )


class Request_5518(message.Message, metaclass = RequestMeta):
    msg_id = 5518

    fields_info = (
                      ('pid', UShort, '协议标识'),
                      ('md5', Array(Char, 33), 'MD5校验')
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
