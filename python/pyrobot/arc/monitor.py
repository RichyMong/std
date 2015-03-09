'''
This module interacts with coremonitor from the ARC server group.
All the upper case constants are defined in const module.
'''
import sys
import socket
import select
import struct
import thread
import md5
import random
import time

from Crypto.Cipher import PKCS1_OAEP
from Crypto.PublicKey import RSA

import arc.base
from arc.util import packet
from arc.util.const import *


class Client(arc.base.Client):
    END_MSG = "END\r\n"
    def __init__(self):
        arc.base.Client.__init__(self)


    def get_stats(self, stype, block = False):
        pkt = packet.OutPacket(MSG_O2M_STATS)
        pkt.append_bytes(stype)

        self.send(pkt)

        if not block: return

        return self.wait_stats()


    def wait_stats(self):
        stats = []
        while True:
            buf = self._sock.recv(32768)
            self._pc.append(buf)

            for pkt in self._pc:
                pkt.decrypt(self._key)
                if pkt.mid == MSG_M2O_STATS_R:
                    ret = self._parse_stats(pkt)
                    if ret == Client.END_MSG:
                        return stats
                    stats.append(ret)


    def _parse_stats(self, pkt):
        stats_len = struct.unpack_from('H', pkt.data, 0)[0]
        stats = struct.unpack_from('{}s'.format(stats_len), pkt.data, 2)[0]

        return stats


    def _proc_stats_result(self, pkt):
        stats = self._parse_stats(pkt)
        if stats == Client.END_MSG:
            sys.exit(0)

        print(stats)


    def dispatch(self, pkt):
        actions = {
                    MSG_M2O_STATS_R     : self._proc_stats_result,
                  }

        if pkt.mid in actions.keys():
            return actions[pkt.mid](pkt)


def query(client, stype):
    while True:
        client.get_stats(stype)
        time.sleep(30)


if __name__ == '__main__':
    c = Client()
    c.connect(('127.0.0.1', 9600))

    stype = '' if len(sys.argv) == 1 else sys.argv[1]

    # tid = thread.start_new_thread(query, (c, stype))

    try:
        for stats in c.get_stats(stype, block = True):
            print(stats)
    except KeyboardInterrupt:
        c.disconnect()
