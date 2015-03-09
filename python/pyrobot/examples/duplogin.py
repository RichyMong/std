import config
import arc
import select
import logging
import sys

logging.basicConfig(format = '%(asctime)s %(name)-12s %(levelname)s %(message)s',
                    datefmt = '%F %T',
                    level = logging.DEBUG)

poll = select.poll()
lclient = arc.login.Client('arcrobot_0001', config.password)
if lclient.connect(config.loginmgr['local']):
    poll.register(lclient._sock, select.POLLIN | select.POLLHUP)
    while True:
        items = poll.poll(5 * 1000)
        for item in items:
            fd, event = item
            if fd == lclient._sock.fileno():
                if (event & select.POLLIN) == select.POLLIN:
                    lclient.recv_once()

                if (event & select.POLLHUP) == select.POLLHUP:
                    print('lost connection with the server')
                    sys.exit(0)
