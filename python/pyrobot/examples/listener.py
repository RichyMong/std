#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
    chatclient: The IM part of Arc robot

    If you need to write different test cases, please inherit
    the ChatClient class and add your own event handler.
"""

import sys
import logging
import getpass
from optparse import OptionParser

import arc.chat
import config

if sys.version_info >= (3, 0):
    raw_input = input

if __name__ == '__main__':
    optp = OptionParser()

    optp.add_option('-s', '--server', help = 'Server to use(qa, local or listener)',
                    type = 'string', dest = 'server', default = 'local');
    optp.add_option("-j", "--user", dest = "user", help = "User to use")
    optp.add_option("-p", "--password", dest = "password", help = "Password to use")

    opts, args = optp.parse_args()

    if hasattr(config, 'DEBUG') and config.DEBUG:
        logging.basicConfig(level = logging.DEBUG)

    if opts.user is None:
        opts.user = raw_input("Username: ")

    if opts.password is None:
        opts.password = getpass.getpass("Password: ")

    imtoken = arc.login.get_im_token(opts.user, opts.password, config.loginmgr[opts.server])
    xmpp = arc.chat.Client(opts.user, imtoken)
    logging.getLogger('sleekxmpp').setLevel(logging.ERROR)
    if xmpp.connect(config.imserver[opts.server]):
        xmpp.process(block = True)
    else:
        print("Unable to connect.")
