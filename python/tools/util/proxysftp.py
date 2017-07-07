import pysftp
import re
import redis
import os.path as op
from datetime import datetime

SERVER_DESC = {
    'st'   : '202.104.236',
    'zr'   : '114.80.234',
    'zp'   : '10.205.135',
    'beta' : '10.205.135',
    'gray' : {
                '156' : '112.124.114.156',
                '8'   : '112.124.113.8',
                '78'  : '112.124.113.78'
             },
}

proxy_ftps = {}

class ProxySFTP(object):
    def __init__(self, proxy_host, username, password, proxy_user,
                 use_keys=True):
        cnopts = None
        if not use_keys:
            cnopts = pysftp.CnOpts()
            cnopts.hostkeys = None

        self.sftp = pysftp.Connection(proxy_host,
                                      username=username,
                                      password=password,
                                      cnopts=cnopts)
        self.proxy_user = proxy_user
        self.last_byte = 0
        self.last_time = None

    def handle_progress(self, nr_done, nr_needed):
        if self.last_time is not None:
            d = datetime.now() - self.last_time
            speed = (nr_done - self.last_byte) * 1000000 / d.microseconds
            print('\r{} / {} {} {:.0%}'.format(nr_done, nr_needed,
                            speed, nr_done * 1.0 / nr_needed), end='')
        else:
            print('\r{} / {}'.format(nr_done, nr_needed), end='')

        self.last_time = datetime.now()
        self.last_byte = nr_done

        if nr_done == nr_needed:
            print('')

    def put_file(self, host, local_path, remote_path):
        sftp_home = self.find_host_home(host)
        if remote_path.endswith('/') or self.sftp.isdir(op.join(sftp_home, remote_path)):
            remote_path = op.join(sftp_home, remote_path, op.basename(local_path))
        else:
            remote_path = op.join(sftp_home, remote_path)

        remote_dir = op.dirname(remote_path)
        if not self.sftp.isdir(remote_dir):
            self.sftp.makedirs(remote_dir)

        self.sftp.put(local_path, remote_path, callback=self.handle_progress)
        print('{} -> {}'.format(local_path, remote_path))

    def get_file(self, host, remote_path, local_path):
        sftp_home = self.find_host_home(host)
        full_remote_path = op.join(sftp_home, remote_path)
        if not self.sftp.exists(full_remote_path):
            raise RuntimeError('{} does not exist on {}'.format(
                        full_remote_path, host))
        self.sftp.get(full_remote_path, local_path)
        print('{} -> {}'.format(full_remote_path, local_path))

    def find_host_home(self, host):
        pattern = re.compile(r'(\w+) sftp[^ ]* \({}\)'.format(host))

        for path in self.sftp.listdir():
            m = re.search(pattern, path)
            if m is not None:
                return op.join(path, self.proxy_user)
        raise RuntimeError('cannot find host {}'.format(host))

    def get_service_deploy_dir(self, service, host):
        sftp_home = self.find_host_home(host)
        if service == 'mds':
            if self.proxy_user == 'root':
                bin_dir_map = { 'multidatasvr/bin' : [ 'root', 'eastmoney/service/' ] }
            else:
                return 'home/baoleiji/mds_upgrade/'
        elif service == 'gqs':
            if self.proxy_user == 'root':
                bin_dir_map = { 'globalquote' : [ 'root', 'eastmoney/services/' ] }
            else:
                return 'home/baoleiji/gqs_upgrade/'
        else:
            raise RuntimeError('unknown service {}'.format(service))

        for bin_dir, possible_homes in bin_dir_map.items():
            for possible_home in possible_homes:
                depoly_dir = op.join(possible_home, bin_dir)
                if self.sftp.exists(op.join(sftp_home, depoly_dir)):
                    return depoly_dir
        else:
            raise RuntimeError('no deployment directory found')


def get_fqdn_host(host):
    m = re.search(r'([a-zA-Z]+)(\d{1,3}(\.\d{1,3}\.\d{1,3})?)', host)
    if m is not None:
        desc = m.group(1).lower()
        brief = m.group(2)
        if desc not in SERVER_DESC:
            raise ValueError('server location {} not found'.format(desc))
        v = SERVER_DESC[desc]
        if isinstance(v, dict):
            return v[brief]
        else:
            return '{}.{}'.format(v, m.group(2))
    else:
        return host

def get_proxy_info(proxy_name):
    rds = redis.StrictRedis(host='localhost', port=6379, db=0)
    keys = [ 'proxy.{}.{}'.format(proxy_name, field) for field in ('ip', 'user',
                                                                 'passwd', 'login_as') ]
    return [ x.decode() for x in rds.mget(keys) ]

def get_proxy_sftp(host):
    if host[:3] == '10.':
        proxy_name = 'zp'
    else:
        proxy_name = 'company'

    if proxy_name not in proxy_ftps:
        pt = ProxySFTP(*get_proxy_info(proxy_name), use_keys=(proxy_name != 'zp'))
        proxy_ftps[proxy_name] = pt
    return proxy_ftps[proxy_name]

