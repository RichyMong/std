import config
import optparse
import logging
import os

from multiprocessing import Process, Queue


def parse_args(add_extra_args = None):
    optp = optparse.OptionParser('usage: %prog [options] [targets...]')

    optp.add_option('-p', '--prefix', help = 'the prefix of user name',
                    type = 'string',  dest = 'prefix', default = config.user_prefix)
    optp.add_option('-b', '--begin', help = 'the begin of the user range',
                    type = int,  dest = 'begin', default = config.user_begin)
    optp.add_option('-e', '--end', help = 'the end of the user range',
                    type = int, dest = 'end', default = config.user_end)
    optp.add_option('-m', '--max', help = 'the max of the user id',
                    type = int, dest = 'max', default = config.user_max)
    optp.add_option('-s', '--server', help = 'set the server to connect(qa, local or staging)',
                    type = 'string', dest = 'server_type', default = config.server_type);

    if add_extra_args is not None:
        add_extra_args(optp)

    return optp.parse_args()


def user_routine(target, user, args):
    logging.basicConfig(filename ='logs/{}'.format(user),
                        filemode ='a',
                        format = '%(asctime)s %(name)-12s %(levelname)s %(message)s',
                        datefmt = '%F %T',
                        level = logging.DEBUG)
    target(user, *args)


def mutliple_user(opts, target, *args):
    '''

    '''
    log_dir = 'logs/'
    if not os.path.exists(log_dir):
        os.mkdir(log_dir)

    children = set()
    for i in xrange(opts.begin, opts.end + 1):
        child = Process(target = user_routine, args = (target, config.to_user(opts.prefix, i), args))
        child.start()
        children.add(child)
    try:
        map(lambda p: p.join(), children)
    except KeyboardInterrupt:
        pass
