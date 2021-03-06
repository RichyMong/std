import sys
import argparse
import os.path as op
import asyncio
import proxysftp
from collections import Iterable


def put_2_single_host(host, ns):
    sftp = proxysftp.get_proxy_sftp(host)

    if ns.service:
        remote_dir = sftp.get_service_deploy_dir(ns.service, host)
        remote_file = op.join(remote_dir, op.basename(ns.local[0]))
        sftp.put_file(host, ns.local[0], remote_file)
    else:
        if not ns.remote:
            print('Remote path is not set.', sftp.user_home, 'will be used')
        for local_file in ns.local:
            sftp.put_file(host, local_file, ns.remote or sftp.user_home)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='put/get file through sftp')
    parser.add_argument('-s', '--service', choices=['mds', 'mts', 'gqs'])
    parser.add_argument('-i', '--host', nargs='*', required=True,
                        help="the host's IP or zr86, zp115 like form")
    parser.add_argument('-l', '--local', nargs='*', required=True,
                        help='local path')
    parser.add_argument('-r', '--remote', help='''remote path, can be a
                         directory or file. If not set, user home will
                         be used''')
    ns = parser.parse_args(sys.argv[1:])

    hosts = set()
    for x in ns.host:
        host = proxysftp.get_fqdn_host(x)
        if isinstance(host, str):
            hosts.add(host)
        elif isinstance(host, Iterable):
            for x in host:
                hosts.add(x)
    loop = asyncio.get_event_loop()
    futures = [ loop.run_in_executor(None, put_2_single_host, host, ns) for host in hosts ]
    loop.run_until_complete(asyncio.gather(*futures))
    loop.close()
