from __future__ import print_function

import base
import arc
import sys
import os
import psutil
import socket
import optparse
import config
import random
import string
import logging
import select
import signal
import time
import multiprocessing
import atexit


MIN_ONLINE_USER = 2500

LANGS = ['en', 'cn', 'jp', 'ru']

LOG_DIR = 'logs/'

EXIT_SUCCESS = 0
EXIT_CONN_FAILED = 1

def randomword(length):
    return ''.join(random.choice(string.lowercase) for i in range(length))


def randomstring(words):
    return ' '.join(randomword(random.randint(3, 10)) for i in range(words))


def sep_str(content):
    return '{sep}{content}{sep}'.format(sep = '-' * 20, content = content)


def memory_dump(loghandler):
    rss = psutil.Process(os.getpid()).memory_percent()
    loghandler.debug('memory percent: {}'.format(rss))

class Env(object):
    server = 'local'
    ubegin = 1
    uend   = ubegin + MIN_ONLINE_USER - 1
    uprefix = config.user_prefix

    @staticmethod
    def make_name(uid):
        return config.to_user2(Env.uprefix, uid)


class Child(object):
    def __init__(self, user):
        self.user       = user
        self.last_tick  = 0
        self.keep_alive = True
        self.lclient    = None   # login client
        self.poll       = select.poll()


    def random_action(self):
        if not self.keep_alive:
            return

        val = random.randint(1, 0xFFFFFFFF)
        if (val & 0x03FF) == 0:
            logging.info('get token')
            self.lclient.get_token(random.choice(config.GAMES))

        if (val & 0x07FF) == 0:
            logging.info('close keep-alive')
            self.keep_alive = False
            return

        if (val & 0x0CFF) == 0:
            self._remove_fd(self.lclient._sock)
            logging.debug('disconnect')
            self.lclient.disconnect()
            time.sleep(random.randint(30, 60))
            self._try_reconnect()

        if (val & 0x0FFF) == 0:
            logging.debug('logout')
            self.lclient.logout()
            self.exit()


    def _add_fd(self, fd):
        self.poll.register(fd, select.POLLIN | select.POLLHUP)


    def _remove_fd(self, fd):
        try:
            self.poll.unregister(self.lclient._sock)
        except KeyError:
            pass


    def _try_reconnect(self):
        if not self.lclient.reconnect(config.loginmgr[Env.server], max_try = 10):
            sys.exit(EXIT_CONN_FAILED)


    def _process_lclient_event(self, event):
        if (event & select.POLLIN) == select.POLLIN:
            logging.info('pollin')
            try:
		self.lclient.recv_once()
            except socket.timeout:
                event |= select.POLLHUP

        if (event & select.POLLHUP) == select.POLLHUP:
            logging.info('pollhup')
            self._remove_fd(self.lclient._sock)
            logging.info('lost connection with the server')
            self._try_reconnect()
            self._add_fd(self.lclient._sock)

    def mainloop(self):
        while True:
            memory_dump(logging)
            items = self.poll.poll(10 * 1000)
            for item in items:
                fd, event = item
                if fd == self.lclient._sock.fileno():
                    self._process_lclient_event(event)

            # if we want to be timed-out by the server, we set keep_alive to
            # False and won't do any random actions.
            if self.keep_alive:
                self.lclient.keep_alive()
                self.random_action()


    def start(self):
        logging.info('{sep}start pid {pid}{sep}'.format(sep = '-' * 20, pid = os.getpid()))
        self.lclient = arc.login.Client(self.user, config.password)
        server = config.loginmgr[Env.server]
        logging.info('connecting to {}'.format(server))
        if not self.lclient.connect(server, lang = random.choice(LANGS)):
            logging.error('failed to connect to {}'.format(server))
            self._try_reconnect()
        self._add_fd(self.lclient._sock)
        self.mainloop()

        logging.info('{sep}end{sep}'.format(sep = '-' * 20))


    def exit(self):
        logging.info('{sep}end{sep}'.format(sep = '-' * 20))
        sys.exit(EXIT_SUCCESS)


def start_child_wrapper(user):
    '''
    it maybe better to make the child a daemon.
    '''
    child = Child(user)
    logging.basicConfig(filename = os.path.join(LOG_DIR, user),
                        filemode ='a',
                        format = '%(asctime)s %(name)-12s %(levelname)s %(message)s',
                        datefmt = '%F %T',
                        level = logging.DEBUG)
    try:
        child.start()
    except KeyboardInterrupt:
        child.exit()


class ReleaseCase(object):
    def __init__(self):
        '''
        pid_map --- pid -> user dict
        user_map  --- user -> pid dict
        The user_map always records the correct information. However,
        the pid_map may record out-of-date information if duplicate
        login is triggered.
        '''
        self.user_map = {}
        self.pid_map  = {}
        self.deferred = {}
        self.config_logger()

    def config_logger(self):
        if not os.path.exists(LOG_DIR):
            os.mkdir(LOG_DIR)

        self.logger = logging.getLogger('release')
        self.logger.setLevel(logging.DEBUG)
        # create file handler which logs even debug messages
        fh = logging.FileHandler(os.path.join(LOG_DIR, 'release'), mode = 'w')
        fh.setLevel(logging.DEBUG)
        # create console handler with a higher log level
        ch = logging.StreamHandler()
        # create formatter and add it to the handlers
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        fh.setFormatter(formatter)
        ch.setFormatter(formatter)
        # add the handlers to the logger
        self.logger.addHandler(fh)
        self.logger.addHandler(ch)


    def start_child(self, user):
        child = multiprocessing.Process(target = start_child_wrapper, args = (user, ))
        child.start()
        self.pid_map[child.pid] = user
        self.user_map[user] = child.pid



    def add_deferred_login(self, user):
        val = random.randint(0, 0xFFFF)
        if (val & 0x0003FFF) == 0:
            self.logger.info('{} will never login again'.format(user))
            return

        # If a user is kicked by duplicated login, user_map will save the pid
        # of the new process. In this case, we cannot make the user login again
        # later.
        if user not in self.deferred:
            self.deferred[user] = 0
            self.logger.debug('add {} to deferred login list'.format(user))


    def do_deferred_login(self):
        poped = []
        for user in self.deferred.keys():
            self.deferred[user] += 1
            # make deferred login as random as possible.
            period = random.randint(30, 90)
            if self.deferred[user] >= period:
                self.logger.info('deferred login user {}'.format(user))
                self.start_child(user)
                poped.append(user)

        if poped:
            self.logger.debug('active user count: {}'.format( len(self.user_map)))

        map(lambda x : self.deferred.pop(x), poped)


    def random_action(self):
        val = random.randint(1, 0xFFFF)

        if (val & 0x0007) == 0:
            lang = random.choice(LANGS)
            self.logger.debug('notify language {}'.format(lang))
            arc.core.notify(config.corecenter[Env.server],
                            randomword(random.randint(3, 10)),
                            randomstring(random.randint(5, 10)),
                            lang,
                            1,
                            [],
                            1)

        if (val & 0x007F) == 0:
            # simulate duplicate login
            user = random.choice(self.user_map.keys())
            self.logger.info('duplicated login {}'.format(user))
            self.start_child(user)


    def child_exit(self):
        old_size = len(self.user_map)

        while True:
            if not self.user_map or not self.pid_map:
                return

            try:
                pid, exit_code = os.waitpid(-1, os.WNOHANG)
            except OSError:
                break

            if not pid:
                break

            user = self.pid_map.pop(pid, None)
            if self.user_map.get(user, None) == pid:
                self.logger.info('user {} exited'.format(user))
                self.user_map.pop(user)
                if exit_code != EXIT_CONN_FAILED:
                    self.add_deferred_login(user)
            else:
                self.logger.info('old user {} exited'.format(user))

        if old_size != len(self.user_map):
            self.logger.debug('active user count: {}'.format(len(self.user_map)))


    def start(self):
        '''

        '''
        self.logger.info(sep_str('start'))

        for i in xrange(opts.begin, opts.end + 1):
            self.start_child(Env.make_name(i))

        self.logger.debug('start child {} - {}'.format(opts.begin, opts.end + 1))

        poll = select.poll()
        while True:
            memory_dump(self.logger)
            poll.poll(1000)
            self.do_deferred_login()
            self.child_exit()
            self.random_action()


def check_server():
    print('checking {}'.format(config.loginmgr[Env.server]))
    try:
        socket.create_connection(config.loginmgr[Env.server], timeout = 30)
        print('loginmgr is okay')
    except:
        print('Notice: loginmgr cannot be reached!')
        return False
    return True


if __name__ == '__main__':
    opts, _ = base.parse_args()
    opts.end = opts.begin + MIN_ONLINE_USER
    Env.server  = opts.server_type
    Env.ubegin  = opts.begin
    Env.uend    = opts.end
    Env.uprefix = opts.prefix
    try:
        if check_server():
            atexit.register(check_server)
            ReleaseCase().start()
    except KeyboardInterrupt:
        pass
