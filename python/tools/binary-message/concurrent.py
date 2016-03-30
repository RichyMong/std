import asyncio
import socket
import errno
import logging
from datetime import datetime
import emoney
import sys

package = emoney.message.Request_5512()
package.pid = 1
package.push_type = 2
package.stockid = 'HK|00700'
package.fields = [42,44,45,46]
send_bytes = package.tobytes()

failed_connection = 0
closed_by_server = 0
request_start_time = {}
request_ttl = []
CHECK_PERIOD = 2

logging.basicConfig(format = '%(asctime)s %(name)-12s %(levelname)s %(message)s',
                datefmt = '%F %T',
                level = logging.INFO)

def get_msecs():
    dt = datetime.now()
    return (dt.hour * 3600 + dt.minute * 60 + dt.second) * 1000 + dt.microsecond // 1000


def stop_loop(loop):
    if failed_connection:
        logging.info('failed connection: {}'.format(failed_connection))


    if closed_by_server:
        logging.info('closed before response: {}'.format(closed_by_server))

    logging.info('request count: {}, response count: {}'.format(
            len(request_start_time),
            len(request_ttl))
          )

    if request_ttl:
        request_ttl.sort()
        avg = sum(request_ttl)/len(request_ttl)
        logging.info(('min time: {}ms, max time: {}ms, ' +
               'average: {:.1f}ms').format(
                   request_ttl[0],
                   request_ttl[-1],
                   avg
              ))

    loop.stop()


def check_response(loop):
    if len(request_ttl) == len(request_start_time):
        stop_loop(loop)
    else:
        logging.info('request count: {}, response count: {}'.format(len(request_start_time), len(request_ttl)))

        loop.call_later(CHECK_PERIOD, check_response, loop)


def run_case(concurrent):
    loop = asyncio.get_event_loop()

    for i in range(concurrent):
        c = emoney.net.Client(loop)
        c.set_message_callback(message_callback)
        c.set_state_callback(state_callback)
        request_start_time[c.cid] = get_msecs()
        c.connect('202.104.236.88', 1862)

    loop.call_later(CHECK_PERIOD, check_response, loop)
    loop.call_later(180, stop_loop, loop)
    try:
        loop.run_forever()
    except Exception as e:
        print(e)
        loop.stop()


def message_callback(c, m):
    if m.msg_id != package.msg_id:
        logging.info('received unexpected message {}'.format(m.msg_id))
    c.close()
    request_ttl.append(get_msecs() - request_start_time[c.cid])


def state_callback(c, new_state, passive):
    if new_state == c.CLOSED and (c.state == c.CONNECTING or passive):
        if c.state == c.CONNECTING:
            global failed_connection
            failed_connection += 1
        else:
            global closed_by_server
            closed_by_server += 1

        request_start_time.pop(c.cid)

    elif new_state == c.CONNECTED:
        c.send_data(send_bytes)


if __name__ == '__main__':
    c = int(sys.argv[1]) if len(sys.argv) > 1 else 2000
    run_case(c or 2000)
