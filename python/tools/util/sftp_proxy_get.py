import sys
import argparse
import os
import os.path as op
import proxysftp


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='put/get file through sftp')
    parser.add_argument('-s', '--service', choices=['mds', 'gqs'])
    parser.add_argument('-i', '--host', required=True,
                        help="the host's IP or st73, zr86 like form")
    parser.add_argument('-l', '--local', nargs='*', help='local path to save the file')
    parser.add_argument('-r', '--remote', nargs='*', required=True, help='remote path, can be a directory or file')
    parser.add_argument('-p', '--proxy', help='proxy type, company/zp',
                        choices=['company', 'zp'], default='company')
    ns = parser.parse_args(sys.argv[1:])

    if not ns.remote and not ns.service:
        sys.exit('Error: remote path is not set')

    host = proxysftp.get_fqdn_host(ns.host)
    sftp = proxysftp.get_proxy_sftp(host)
    if not ns.local:
        ns.local = [ os.getcwd() ]

    if len(ns.remote) > 1:
        if not (op.isdir(ns.local[0]) or len(ns.local) == len(ns.remote)):
            sys.exit('Error: local path should be a directory for multiple remote files')

    if len(ns.local) < len(ns.remote):
        local_dir = ns.local[0]
        for r in ns.remote:
            sftp.get_file(host, r, op.join(local_dir, op.basename(r)))
    else:
        for r, l in zip(ns.remote, ns.local):
            if op.isdir(l):
                sftp.get_file(host, r, op.join(l, op.basename(r)))
            else:
                sftp.get_file(host, r, l)
