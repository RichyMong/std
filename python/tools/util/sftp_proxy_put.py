import sys
import argparse
import os.path as op
import proxysftp


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='put/get file through sftp')
    parser.add_argument('-s', '--service', choices=['mds', 'gqs'])
    parser.add_argument('-i', '--host', nargs='*', required=True,
                        help="the host's IP or st73, zr86, betamds1 like form")
    parser.add_argument('-l', '--local', nargs='*', required=True, help='local path')
    parser.add_argument('-r', '--remote', help='remote path, can be a directory or file')
    ns = parser.parse_args(sys.argv[1:])

    if not ns.remote and not ns.service:
        sys.exit('Error: remote path is not set')

    for x in ns.host:
        host = proxysftp.get_fqdn_host(x)

        sftp = proxysftp.get_proxy_sftp(host)

        if ns.remote:
            for local_file in ns.local:
                sftp.put_file(host, local_file, ns.remote)
        else:
            remote_dir = sftp.get_service_deploy_dir(ns.service, host)
            sftp.put_file(host, ns.local[0], op.join(remote_dir, op.basename(ns.local[0])))
