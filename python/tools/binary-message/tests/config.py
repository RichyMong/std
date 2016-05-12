import re
import os
import argparse
import messages
from ouou import net, message
from collections import OrderedDict

SERVER = ('202.104.236.88', 1862)

def get_server_version(gq_src_dir = '/home/meng/windows/code/linuxserver/global_quote/globalquote/'):
    with open(os.path.expanduser(os.path.join(gq_src_dir,
            'main/handle/General.h')), encoding = 'gbk') as f:
        pattern = re.compile(r'#define +SERVER_VERSION +(\d+)')
        for line in f:
            m = pattern.match(line)
            if m:
                return int(m.group(1))
    return 20160504

SERVER_VERSION = get_server_version()

def create_parser(description=''):
    parser = argparse.ArgumentParser(description=description)
    group = parser.add_argument_group('server information')
    group.add_argument('-a', '--address', action='store', default='88',
                         help='ip address or an integer in which case the'+
                             ' address will be 202.104.236.xx, default 88')
    group.add_argument('-p', '--port', action='store', type = int, default=1862,
                        help='the port of the server, default 1862')
    group = parser.add_argument_group('stock information')
    group.add_argument('-m', '--message', nargs='*',
                        help ='message ids, delimited by space')
    group.add_argument('-s', '--stock-id', action='store',
                        help='the stock id to use, random if not set')
    parser.add_argument('-v', '--verbosity', action='count', default=2,
                        help='print verbose')
    return parser


def parse_args(args, parser = None, **kwargs):
    offline = kwargs.pop('offline', False)

    defaults = dict(message=[], stock_id=None, address='88', port=1862,
                    verbosity=2, receive_push=False)
    defaults.update(kwargs)
    ns = argparse.Namespace(**defaults)
    if not parser:
        parser = create_parser()
    parser.parse_args(args=args, namespace=ns)
    if ns.stock_id:
        messages.use_stock(ns.stock_id)

    if ns.message:
        message_ids = set(int(msg_id) for msg_id in ns.message)
    else:
        message_ids = set(messages.all_messages.keys())

    if ns.address.rfind('.') < 0:
        ns.address = '202.104.236.{}'.format(ns.address)

    global SERVER
    SERVER = (ns.address, ns.port)

    ns.message = OrderedDict()
    for mid in sorted(message_ids):
        m = messages.all_messages[mid]
        if hasattr(m, 'push_type'):
            if ns.receive_push:
                m.push_type = message.PUSH_TYPE_REGISTER
            else:
                m.push_type = message.PUSH_TYPE_ONCE
        ns.message[mid] = m

    if not offline:
        server = (ns.address, ns.port)
        if ns.stock_id:
            fetch_stock_price_digits([ns.stock_id], server)
        else:
            fetch_stock_price_digits(messages.sample_stocks, server)

    return ns

def fetch_stock_price_digits(stocks, server):
    return

    c = net.Client()
    c.connect(server)

    for stock in stocks:
        if message.get_stock_cache(stock) is None:
            req = message.Request_5512()
            req.pid = 1
            req.push_type = message.PUSH_TYPE_ONCE
            req.stock_id = stock
            req.fields = [72, 73]
            m = c.send_and_receive(req)
            if not m.empty():
                message.set_stock_cache(stock, (m.data[0], m.data[1]))
