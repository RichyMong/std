from __future__ import print_function

import sys
import os
import time
import optparse
import arc
import config

from multiprocessing import Process, Queue


def do_work(targets, count, queue):
    '''
    Since we don't create a class object for this simple test, we have to
    store the informations this way...
    '''
    def wrapper(client):
        jids = [arc.chat.make_jid(x) for x in targets]
        for i in xrange(0, count):
            body = 'The number is {} now.'.format(i + 1)
            for jid in jids:
                client.send_message(jid, body)
            time.sleep(1)
        queue.put((client.user, count))
        client.disconnect()
        sys.exit(0)
    return wrapper


def run_user(user, targets, count, queue, server):
    token = arc.login.get_im_token(user, config.password, config.loginmgr[server])
    client = arc.chat.Client(user, token)
    if not client.connect(config.imserver[server]):
        sys.exit(1)

    # client.add_ready_handler(do_work(targets, count, queue))
    client.process(block = True)


def sendto(opts, args):
    '''
    Send some messages from some users to other users. The sent users are
    marked by opts.begin and opts.end, the prefix is the users' common name.
    '''
    children = set()
    targets = [config.to_user(opts.prefix, uid) for uid in args]
    queue = Queue()
    for i in xrange(opts.begin, opts.end + 1):
        user = config.to_user(opts.prefix, i)
        child = Process(target = run_user, args = (user, targets, opts.count,
                            queue, opts.server_type))
        child.start()
        children.add(child)
    map(lambda p: p.join(), children)

    print('sent to {}'.format(targets))
    total = 0
    while not queue.empty():
        user, count = queue.get()
        total += count
        print('    {} --- {}'.format(user, count))
    print('    total: {}'.format(total))


if __name__ == '__main__':
    optp = optparse.OptionParser('usage: %prog [options] [targets...]')

    optp.add_option('-p', '--prefix', help = 'the prefix of user name',
                    type = 'string',  dest = 'prefix', default = config.user_prefix)
    optp.add_option('-b', '--begin', help = 'the begin of the user range',
                    type = int,  dest = 'begin', default = config.user_begin)
    optp.add_option('-e', '--end', help = 'the end of the user range',
                    type = int, dest = 'end', default = config.user_end)
    optp.add_option('-m', '--max', help = 'the max of the user id',
                    type = int, dest = 'max', default = config.user_max)
    optp.add_option('-c', '--count', help = 'the number of messages to send',
                    type = int, dest = 'count', default = config.msg_cnt)
    optp.add_option('-s', '--server', help = 'set the server to connect(qa, local or staging)',
                    type = 'string', dest = 'server_type', default = config.server_type);

    opts, args = optp.parse_args()
    if len(args) < 1:
        print('Warning: No target is provided. {} will be used.'.format(opts.end + 1),
               file = sys.stderr)
        args.append(opts.end + 1)
    sendto(opts, args)
