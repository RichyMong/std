import pysftp
import re
import redis
import os.path as op
from datetime import datetime

SERVER_DESC = {
    'zp'   : '10.205.135',
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
        self.last_byte = 0
        self.last_time = None
        self.proxy_user = proxy_user
        if proxy_user == 'root':
            self.user_home = 'root/'
        else:
            self.user_home = op.join('home', proxy_user, '')

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

    def check_restrict_home(self, path):
        if self.proxy_user != 'root' and not path.startswith(self.user_home):
            return op.join(self.user_home, path)
        return path

    def put_file(self, host, local_path, remote_path):
        remote_path = self.check_restrict_home(remote_path)
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
        remote_path = self.check_restrict_home(remote_path)
        sftp_home = self.find_host_home(host)
        full_remote_path = op.join(sftp_home, remote_path)
        if not self.sftp.exists(full_remote_path):
            raise RuntimeError('{} does not exist on {}'.format(
                        full_remote_path, host))
        self.sftp.get(full_remote_path, local_path)
        print('{} -> {}'.format(full_remote_path, local_path))

    def find_host_home(self, host):
        pattern = re.compile(r'(\w+) sftp[^ ]* \({}\)'.format(host))

        paths = self.sftp.listdir()
        for path in paths:
            m = re.search(pattern, path)
            if m is not None:
                return op.join(path, self.proxy_user)
        raise RuntimeError('cannot find host {}'.format(host))

    def get_service_deploy_dir(self, service, host):
        return 'home/baoleiji/{}_upgrade/'.format(service)

def get_loc_host(loc, brief=None):
    v = SERVER_DESC[loc]
    if isinstance(v, dict):
        if brief is None:
            return list(v.values())
        else:
            return v[brief]
    else:
        return '{}.{}'.format(v, brief)

def get_fqdn_host(host):
    m = re.search(r'([a-zA-Z]+)(\d{1,3}(\.\d{1,3}\.\d{1,3})?)', host)
    if m is not None:
        desc = m.group(1).lower()
        brief = m.group(2)
        if desc not in SERVER_DESC:
            raise ValueError('server location {} not found'.format(desc))
        return get_loc_host(desc, brief)
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
