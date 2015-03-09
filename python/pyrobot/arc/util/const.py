'''
Here are the constants used, including but not limited to message types.
All the names introduced by this module are composed of uppercase letters.
So it's quite safe to use from const import *.
'''

PKT_MAGIC                = 0x5050


PKT_HDR_LEN              = 12
PKT_HDR_FMT              = 'HHHHI'  # magic, flag, len, mid, pid
PKT_DATA_LEN_OFFSET      = 4

PKT_FLAG_REPLY           = 0x0001
PKT_FLAG_COMPRESS        = 0x0002
PKT_FLAG_ENCRYPT         = 0x0004

TOKEN_LEN                = 16

ENCIPHER_KEY_LEN         = 4

MSG_S2C_SECURITY_INIT    = 0x01
MSG_S2C_INIT_FINAL       = 0x02

MSG_C2S_SECURITY_INIT    = 0x50

COMMON_USER              = 0
FACEBOOK_USER            = 1
RECONNECT_USER           = 2

MSG_C2L_BASE             = 0x3000
MSG_C2L_LOGIN            = MSG_C2L_BASE + 1
MSG_C2L_GET_TOKEN        = MSG_C2L_BASE + 2
MSG_C2L_LOGOUT           = MSG_C2L_BASE + 3
MSG_C2L_KEEP_ALIVE       = MSG_C2L_BASE + 4
MSG_C2L_TRACE_LOG        = MSG_C2L_BASE + 5
MSG_C2L_EXIT             = MSG_C2L_BASE + 6
MSG_C2L_CHECK_PIN        = MSG_C2L_BASE + 7
MSG_C2L_REQUEST_NEW_PIN  = MSG_C2L_BASE + 8

MSG_L2C_BASE             = 0x4000
MSG_L2C_LOGIN_RESULT     = MSG_L2C_BASE + 1
MSG_L2C_GET_TOKEN_RESULT = MSG_L2C_BASE + 2
MSG_L2C_NOTIF            = MSG_L2C_BASE + 3
MSG_L2C_KICK             = MSG_L2C_BASE + 4

# monitor related
MSG_O2M_STATS            = 0x4001
MSG_M2O_STATS_R          = 0x5001
