from collections import namedtuple

# whether sequnce, stockid or market we be parameters of the notify function.
NotifySetting = namedtuple('NotifySetting', ['sequence', 'stockid', 'market'])

# 'reg'  : the 'req' class(without the CReqStk part) we use to process the message.
#          if empty a common req class will be used
# 'push' : whether this message could be pushed
MDS = namedtuple('MDS', [ 'req', 'push' ])

root = '/home/meng/windows/code/cas-vcm/multimarket/trunk/src/'

FROM_CLASS = 'CStdQtEx'

STD_CLASS_PREFIX = 'CStd'

CACHE_CLASS_PREFIX = STD_CLASS_PREFIX + 'Cache'

DATA_MODEL = 'QTEX'

REG_TYPE = 'TYPE_{}'.format(DATA_MODEL)

# whether the newly add class will be a member of another class.
is_embeded = True

# The data is stock-related
stock_data = True

embed_class = 'CStdDataUnit'

# the implementation module and class of the file
embed_impl = ('stddataunitimp', 'CStdDataUnitImp')

notify = NotifySetting(False, False, True)

mds = MDS('', True)
