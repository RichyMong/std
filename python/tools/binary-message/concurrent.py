import asyncio
import socket
import errno
import logging
import collections
from datetime import datetime
import ouou
from ouou import util
import sys

msg_5512 = ouou.message.Request_5512()
msg_5512.pid = 1
msg_5512.push_type = 1
msg_5512.stockid = 'HK|00700'
msg_5512.fields = [42,44,45,46]

msg_5513 = ouou.message.Request_5512()
msg_5513.pid = 2
msg_5513.push_type = 1
msg_5513.stockid = 'HK|00700'
msg_5513.fields = [42,44,45,46]
msg_5513.date_time =1603311550

messges = { 5512 : msg_5512, 5513 : msg_5513 }

failed_connection = 0
closed_by_server = 0
request_start_time = {}
round_trip = collections.defaultdict(list)
CHECK_PERIOD = 2

clients = set()

logging.basicConfig(format = '%(asctime)s %(name)-12s %(levelname)s %(message)s',
                datefmt = '%F %T',
                level = logging.INFO)

def print_result():
    if failed_connection:
        logging.info('failed connection: {}'.format(failed_connection))


    if closed_by_server:
        logging.info('closed before response: {}'.format(closed_by_server))

    for x, v in round_trip.items():
        if len(v) == len(request_start_time):
            logging.info('request {}, count: {}, response count: {}'.format(
                    x, len(request_start_time), len(v))
            )

            v.sort()
            avg = sum(v)/len(v)
            logging.info(('min time: {}ms, max time: {}ms, ' +
                   'average: {:.1f}ms').format(
                       v[0], v[-1], avg
                  ))


def check_response(loop):
    if len(request_start_time):
        if len(round_trip) == len(request_start_time):
            print_result()
        else:
            logging.info('request count: {}, response count: {}'.format(len(request_start_time), len(round_trip)))

    if not clients:
        loop.stop()
    else:
        loop.call_later(CHECK_PERIOD, check_response, loop)


def run_case(concurrent):
    loop = asyncio.get_event_loop()

    for i in range(concurrent):
        c = ouou.net.Client(loop)
        c.set_message_callback(message_callback)
        c.set_state_callback(state_callback)
        request_start_time[c.cid] = util.get_milliseconds()
        clients.add(c.cid)
        c.connect('202.104.236.88', 1862)

    loop.call_later(CHECK_PERIOD, check_response, loop)
    # loop.call_later(180, stop_loop, loop)
    try:
        loop.run_forever()
    except Exception as e:
        print(e)
        loop.stop()


def message_callback(c, m):
    if not m.is_push_pkg():
        round_trip[m.msg_id].append(util.get_milliseconds() - request_start_time[c.cid])

    print(m)
    if not c.is_waiting_for_msg() or not any(
            (c.is_waiting_for_response(), ouou.util.is_trade_time())):
        c.close()


def state_callback(c, new_state):
    if new_state == c.CLOSED:
        if c.state != c.CLOSING:
            print('{} closed by remote'.format(c))
            if c.state == c.CONNECTING:
                global failed_connection
                failed_connection += 1
            else:
                global closed_by_server
                closed_by_server += 1

        request_start_time.pop(c.cid)
        clients.remove(c.cid)

    elif new_state == c.CONNECTED:
        for m in messges.values():
            c.send_message(m)


if __name__ == '__main__':
    c = int(sys.argv[1]) if len(sys.argv) > 1 else 2000
    run_case(c or 2000)
