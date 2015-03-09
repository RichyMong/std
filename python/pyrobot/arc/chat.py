#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
    chat: The IM part of Arc robot

    If you need to write different test cases, please inherit
    the chat.Client class and add your own event handler.
"""

import sys
import logging
import sleekxmpp


log = logging.getLogger(__name__)
log.setLevel(logging.ERROR)

# Set the logging level of sleekxmpp so that there will be
# no boring debug messages.
logging.getLogger('sleekxmpp').setLevel(logging.ERROR)

# Python versions before 3.0 do not use UTF-8 encoding
# by default. To ensure that Unicode is handled properly
# throughout SleekXMPP, we will set the default encoding
# ourselves to UTF-8.
if sys.version_info < (3, 0):
    from sleekxmpp.util.misc_ops import setdefaultencoding
    setdefaultencoding('utf8')


DISCO_NS = 'http://jabber.org/protocol/disco#info'
OFFLINE_NS = 'http://jabber.org/protocol/offline'


def make_jid(user):
    return user + '@xiao.com'


class Client(sleekxmpp.ClientXMPP):
    """
    The base class of all other chat clients. It contains the basic functions,
    such as check offline message, ping. If you need to write test cases, you
    just need to inherit this class and override the methods like handle_online,
    handle_offline and handle_message.
    """
    keepalive_timeout = 60

    def __init__(self, user, imtoken):
        sleekxmpp.ClientXMPP.__init__(self, make_jid(user), imtoken)

        self.register_plugin('xep_0030') # Service Discovery
        self.register_plugin('xep_0013') # Flexible Offline
        self.register_plugin('xep_0004') # Data Forms
        self.register_plugin('xep_0060') # PubSub
        self.register_plugin('xep_0199') # XMPP Ping

        # The session_start event will be triggered when
        # the bot establishes its connection with the server
        # and the XML streams are ready for use. We want to
        # listen for this event so that we we can initialize
        # our roster.
        self.add_event_handler("session_start", self._handle_start)
        self.add_event_handler("message", self._handle_message)
        self.add_event_handler('disco_info', self._handle_discoinfo)
        self.add_event_handler('disco_items', self._handle_discoitems)
        self.add_event_handler('presence_available', lambda pres : self.handle_online(pres['from'].user))
        self.add_event_handler('presence_unavailable', lambda pres : self.handle_offline(pres['from'].user))

        self.user           = user
        self.offlinemsg_cnt = 0
        self.ready_handlers = []


    def connect(self, server):
        '''
        Connect to the server which is in the form of (addr, port).
        This is just a wrapper for convenience since we don't use tls all the time.
        '''
        return sleekxmpp.ClientXMPP.connect(self, server, use_tls = False)


    def send_message(self, to, body, subject = None):
        '''
        A wrapper to send chat messages to a friend.
        '''
        sleekxmpp.BaseXMPP.send_message(self, mto = to, mbody = body,
                msubject = subject, mtype = 'chat')


    def add_ready_handler(self, callback):
        self.ready_handlers.append(callback)


    @property
    def disco(self):
        return self.plugin['xep_0030']


    @property
    def offline(self):
        return self.plugin['xep_0013']


    def _trigger_ready_handler(self):
        map(lambda x : x(self), self.ready_handlers)


    def _check_offlinemsg(self, iq):
        query = iq.find('{{{}}}query'.format(DISCO_NS))
        x = query.find('{jabber:x:data}x')
        for field in x.findall('{jabber:x:data}field'):
            if field.get('var') == 'number_of_messages':
                count = int(field.find("{jabber:x:data}value").text)
                log.info('{} offline messages on the server.'.format(count))
                if count:
                    self.offlinemsg_cnt = count
                    self.offline.get_headers(jid = self.boundjid.domain, ifrom = self.boundjid)
                break
        self._trigger_ready_handler()


    def _handle_discoinfo(self, iq):
        '''
        Handle the disco info. This is similar to what the arc client does.
        We get the features on connect and all the disco info is handled here.
        If the flexinfo feature is supported, we try to get the offline message
        count.
        Arguments:
            iq   -  the received iq stanza
        '''
        node = iq['disco_info']['node']
        if not len(node):
            if OFFLINE_NS in iq['disco_info']['features']:
                log.debug('flexinfo is supported by the server.')
                self.offline.get_count(jid = self.boundjid.domain, ifrom = self.boundjid)
            else:
                log.info('flexinfo is not supported by the server.')
        elif node == OFFLINE_NS:
            self._check_offlinemsg(iq)


    def _handle_discoitems(self, iq):
        self.offline.fetch(ifrom = self.boundjid)


    def _handle_message(self, msg):
        """
        Process incoming message stanzas. Be aware that this also
        includes MUC messages and error messages. It is usually
        a good idea to check the messages's type before processing
        or sending replies.

        Arguments:
            msg -- The received message stanza. See the documentation
                   for stanza objects and the Message stanza to see
                   how it may be used.
        """
        if msg['type'] in ('chat', 'normal'):
            is_offline = False
            if msg.find('{{{}}}offline'.format(OFFLINE_NS)):
                is_offline = True
                self.offlinemsg_cnt -= 1
                if not self.offlinemsg_cnt:
                    log.info('remove all the offline messages')
                    self.offline.purge(ifrom = self.boundjid)
            self.handle_message(msg['body'], is_offline)


    def _handle_start(self, event):
        """
        Process the session_start event.

        Typical actions for the session_start event are
        requesting the roster and broadcasting an initial
        presence stanza.

        Arguments:
            event -- An empty dictionary. The session_start
                     event does not provide any additional
                     data.
        """
        self.get_roster(block = True)      # block here to make life easy
        self.send_presence(pfrom = self.boundjid, ppriority = 0, ptype = 'available')
        self.disco.get_info(self.boundjid.domain, cached = True, block = False)
        self.schedule("keepalive", self.keepalive_timeout, self._handle_keepalive)


    def _handle_keepalive(self):
        self['xep_0199'].ping(self.boundjid.domain, timeout=10)
        self.schedule("keepalive", self.keepalive_timeout, self._handle_keepalive)


    def handle_online(self, who):
        '''
        who - the use name part of the jid
        '''
        log.debug('{} is online'.format(who))


    def handle_offline(self, who):
        '''
        who - the use name part of the jid
        '''
        log.debug('{} is offline'.format(who))


    def handle_message(self, body, offline):
        log.debug('msg {} sending from {}'.format(msg['body'], msg['from']))


__all__ = ['make_jid', 'Client']
