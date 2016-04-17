﻿from ouou.util import Char

MSG_FLAG_COMPRESSED   = 0x02
MSG_FLAG_REQ_COMPRESS = 0x04
MSG_FLAG_SUB          = 0x08
MSG_FLAG_PUSH         = 0x10

MSG_TYPE_MULTI        = 5500

PUSH_TYPE_UNREGISTER  = 0
PUSH_TYPE_REGISTER    = 1
PUSH_TYPE_ONCE        = 2

SORT_ORDER_ASEC = 0
SORT_ORDER_DESC = 1

REQ_TYPE_MARKET = 0
REQ_TYPE_SELF_STOCK = 1
REQ_TYPE_MODULE = 2
REQ_TYPE_GLOBAL_QUOTE = 3
REQ_TYPE_MARKET_MOULE = 4

REQ_FLAG_NR_ROOTS = 0
REQ_FLAG_START_TIME = 1

MSG_DELIM = '{'
MSG_TAIL_BYTES = Char.pack('}')

INDENT = 4 * ' '
PRINT_PREFIX = '\n' + INDENT

CHINSE_NUMBER = [ '零', '一', '二', '三', '四', '五', '六', '七', '八', '九', '十']

__all__ = [x for x in locals() if not x.startswith('__')]