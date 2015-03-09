from __future__ import print_function

import sys
import os
import time
import socket
import optparse
import arc
import config
import random
import logging
import base


def run_user(user, server):
    client = arc.login.Client(user, config.password)
    langs = ['en', 'cn', 'jp', 'ru']
    lang = random.choice(langs)
    sep = '-' * 20
    logging.info('{sep}start{sep}'.format(sep = sep))
    if client.connect(config.loginmgr[server], lang = lang):
        try:
            while True:
                try:
                    buf = client.recv()
                except socket.error:
                    client.reconnect()
        except KeyboardInterrupt:
            pass
        finally:
            logging.info('{sep}end{sep}'.format(sep = sep))


if __name__ == '__main__':
    optp = base.add_args()
    opts, args = optp.parse_args()
    base.mutliple_user(opts, run_user, opts.server_type);
