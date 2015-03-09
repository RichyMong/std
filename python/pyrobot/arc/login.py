'''
This module interacts with loginmgr from the ARC server group.
All the upper case constants are defined in const module.
'''
import sys
import socket
import struct
import logging
import md5
import random
import time
import arc.base

from Crypto.Cipher import PKCS1_OAEP
from Crypto.PublicKey import RSA

from .util import packet
from .util import tea
from .util.const import *


logger = logging.getLogger(__name__)


class Client(arc.base.Client):
    ERROR_MSG = { 1 : 'server busy', 2 : 'internal error', 3 : 'wrong password',
                  4 : 'server upgrade', 5 : 'temporarily banned', 6 : 'region banned' }

    def __init__(self, username, passwd, use_email = False):
        arc.base.Client.__init__(self)
        self._username  = username
        self._passwd    = passwd
        self._use_email = use_email
        self._online    = False
        self._cookie    = ''
        self._lang      = 'en'


    def disconnect(self):
        '''
        Disconnect from the server. Set the security key to none and we will
        use a different one the next time.
        '''
        self._key       = None
        self._online    = False
        self._sock.close()


    def connect(self, server, lang = 'en', env_desc = 'arcversion:V99.99.99.9999 OS:Centos', reconnect = False):
        arc.base.Client.connect(self, server)

        self._lang = lang

        pkt = packet.OutPacket(MSG_C2L_LOGIN)
        if reconnect:
            pkt.append_bytes(self._username, self._cookie, lang, env_desc)
            pkt.append_uint16(RECONNECT_USER)
        else:
            pkt.append_bytes(self._username, self._passwd, lang, env_desc)
            pkt.append_uint16(COMMON_USER)

        return self.send_and_recv(pkt, lambda pkt : pkt.mid == MSG_L2C_LOGIN_RESULT)


    def reconnect(self, server, max_try = 0):
        try_cnt = 0
        while True:
            self.disconnect()
            time.sleep(3)
            if self.connect(server, lang = self._lang, reconnect = True):
                return True
            try_cnt += 1
            if max_try and try_cnt > max_try:
                return False


    def logout(self):
        pkt = packet.OutPacket(MSG_C2L_EXIT)
        self.send(pkt)
        self.disconnect()


    def keep_alive(self):
        if not self._sock:
            logger.error('keep alive: not connected')
            return None

        pkt = packet.OutPacket(MSG_C2L_KEEP_ALIVE)
        self.send(pkt)


    def get_token(self, game = ''):
        if not self._online:
            logger.error('get_token: not connected')
            return None

        pkt = packet.OutPacket(MSG_C2L_GET_TOKEN)
        pkt.append_uint32(self._use_email)
        if len(game): pkt.append_bytes(game)

        try:
            token = self.send_and_recv(pkt, lambda pkt : pkt.mid == MSG_L2C_GET_TOKEN_RESULT)
        except socket.timeout:
           logger.error('get token timeout')
           return None

        if not token or len(token) != TOKEN_LEN:
            raise ValueError('invalid token: {}'.format(token))

        return token


    def get_im_token(self):
        token = self.get_token()

        m = md5.new()
        m.update(token + 'PWRD7766CORE')

        return m.hexdigest()


    def dispatch(self, pkt):
        actions = {
                    MSG_L2C_LOGIN_RESULT     : self._proc_login_result,
                    MSG_L2C_GET_TOKEN_RESULT : self._proc_token_result,
                    MSG_L2C_NOTIF            : self._proc_notification,
                    MSG_L2C_KICK             : self._proc_kicked,
                  }

        if pkt.mid in actions.keys():
            return actions[pkt.mid](pkt)
        else:
            logger.warning('unknown message id {}'.format(mid))


    def _proc_login_result(self, pkt):
        flag, result = pkt.extract_chars(2)
        logger.debug('login result: flag[{}], result[{}]'.format(flag, result))
        if (not flag == 1) and (result in Client.ERROR_MSG):
            raise ValueError('failed to login: {}'.format(Client.ERROR_MSG[result]))
        if not self._cookie:
            _, _, _, self._cookie = pkt.extract_strings(4)
        self._online = True

        return True


    def _proc_token_result(self, pkt):
        token_len = struct.unpack_from('H', pkt.data, 0)[0]
        token = struct.unpack_from('{}s'.format(token_len), pkt.data, 2)[0]
        logger.debug('got token {}'.format(token))
        return token


    def _proc_notification(self, pkt):
        title, content = pkt.extract_strings(2)
        target, show   = pkt.extract_shorts(2)
        logger.info('received notification - title[{}], content[{}], target[{}], '
                    'show[{}]'.format(title, content, target, show))


    def _proc_kicked(self, pkt):
        logger.debug('was kicked')
        self.disconnect()
        sys.exit(0)


    @property
    def online(self):
        return self._online


def get_im_token(username, passwd, server):
    '''
    Just a wrapper to get the im token only once. If you need to get it more
    than once, it's more efficient to instantiate login.Client and then call
    login.Client.get_token instead.
    '''
    client = Client(username, passwd)
    client.connect(server)
    token = client.get_im_token()
    client.disconnect()

    return token


__all__ = ['Client', 'get_im_token']
