'''
This module interacts with coremonitor from the ARC server group.
All the upper case constants are defined in const module.
'''
import socket
import select
import struct
import logging
import thread
import md5
import random
import time

from Crypto.Cipher import PKCS1_OAEP
from Crypto.PublicKey import RSA

from .util import packet
from .util.const import *


logger = logging.getLogger(__name__)


class Client(object):
    def __init__(self):
        self._sock = None
        self._key  = None
        self._pc   = packet.PacketCombiner()


    def disconnect(self):
        '''
        Disconnect from the server. Set the security key to none and we will
        use a different one the next time.
        '''
        self._key       = None
        self._recvtid   = None
        self._sock.close()


    def connect(self, server):
        '''
        Connect to the server and complete the security initialization.
        For simplicity, the current thread is blocked until it's finished.

        Arguments:
            server - a tuple in the format (addr, port)
        '''
        self._sock = socket.create_connection(server)
        while True:
            buf = self._sock.recv(32768)
            self._pc.append(buf)

            for pkt in self._pc:
                if pkt.mid == MSG_S2C_SECURITY_INIT:
                    self._init_security(pkt)
                elif pkt.mid == MSG_S2C_INIT_FINAL:
                    logger.debug('connected to {}[{}]'.format(*server))
                    return


    def _init_security(self, pkt):
        for i in range(0, ENCIPHER_KEY_LEN):
            ri = random.randint(1, 127)
            if not self._key:
                self._key = struct.pack('I', ri)
            else:
                self._key += struct.pack('I', ri)
        key_data = pkt.extract_string()
        pub_key = RSA.importKey(key_data)
        cipher = PKCS1_OAEP.new(pub_key)
        crypto = cipher.encrypt(self._key)
        pkt = packet.OutPacket(MSG_C2S_SECURITY_INIT)
        pkt.append_raw(crypto)
        self._sock.sendall(pkt.buf)


    def dispatch(self):
        raise NotImplementedError


    def send(self, pkt):
        if self._key:
            pkt.encrypt(self._key)
        self._sock.sendall(pkt.buf)


    def recv(self, block = True):
        if block:
            self._recv_loop()
        else:
            self._recvtid = thread.start_new_thread(self._recv_loop, (self, ))


    def recv_once(self, check = None):
        '''
        Set check to a check function when we know which message we are
        waiting for. That is a synchronous response.
        '''
        buf = self._sock.recv(32768)
        self._pc.append(buf)
        logger.debug('received {} bytes'.format(len(buf)))
        if not len(buf):
	    raise socket.timeout

        for pkt in self._pc:
            pkt.decrypt(self._key)
            ret = self.dispatch(pkt)
            if check and check(pkt):
                return ret
        return None


    def _recv_loop(self, check = None):
        while True:
            ret = self.recv_once(check)
            if check and ret:
                return ret


    def send_and_recv(self, pkt, check):
        self.send(pkt)

        return self._recv_loop(check)
