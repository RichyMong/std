#!/usr/bin/env python2
# -*- coding: UTF-8 -*-

'''
Try to sign in automatically on workdays. We decide if a day is a working
day from holiday information provided by fangjia.911cha.com.
If you do extra work on non workdays, please sing in manually.
'''
from __future__ import print_function
import sys
import os
import re
import urllib
import pickle
import getpass
import requests
import bs4
import optparse
import time
import datetime
import logging
import json


if sys.version_info[0] == '3':
    from urllib import urlparse
else:
    import urlparse

logger = logging.getLogger('auto-sign')

def print_wrapper(*args, **kwargs):
    logger.debug(*args, **kwargs)
    # if sys.platform == 'win32':
    #     logger.debug(*args, **kwargs)
    # else:
    #     print(*args, **kwargs)


def extract_form(response):
    bsoup = bs4.BeautifulSoup(response.text)
    form = bsoup.find_all('form', method = 'post')[0]
    url = urlparse.urljoin(response.url, form.attrs['action'])

    params = {}
    children = form.find_all('input', value = True)
    for child in children:
        if child['type'] == 'hidden':
            params[child['name']] = child['value']
    return (url, params)


class OASession(requests.Session):
    user_agent = 'Mozilla/5.0 (X11; Linux x86_64; rv:38.0) Gecko/20100101 Firefox/38.0'
    oa_url = 'http://web.oa.wanmei.com/'

    def __init__(self, **kwargs):
        '''
        kwargs:
            interactive - whether we will print the prompt, default True
            user - the OA user name
            passwd - the passwd of the user
        '''
        super(OASession, self).__init__()
        self.user = kwargs.get('user', None)
        self.passwd = kwargs.get('passwd', None)
        interactive = kwargs.get('interactive', False)
        if not self.check_cookie():
            if interactive:
                self.user = raw_input('Enter username: ')
                self.passwd = getpass.getpass('Enter password: ')
            elif not all((self.user, self.passwd)):
                raise RuntimeError('user name or password is missing')

            self.login()

    def post(self, url, content, **headers):
        headers['User-Agent'] = OASession.user_agent
        headers['Connection'] = 'Keep-Alive'
        headers['Accept-Charset'] = 'utf-8'
        headers['Accept-Language'] = 'zh-CN,en-US'
        headers['Accept-Encoding'] = 'deflate'
        headers['Pragma'] = 'no-cache'
        headers['Cache-control'] = 'no-cache'
        if content:
            headers['Content-Type'] = 'application/x-www-form-urlencoded'
            headers['Content-Length'] = len(content)

        return super(OASession, self).post(url, headers = headers, data = content)

    def get(self, url, **headers):
        headers['User-Agent'] = OASession.user_agent
        headers['Connection'] = 'Keep-Alive'
        headers['Accept-Charset'] = 'utf-8'
        headers['Accept-Language'] = 'zh-CN,en-US'
        headers['Accept-Encoding'] = 'deflate'
        headers['Pragma'] = 'no-cache'
        headers['Cache-control'] = 'no-cache'

        return super(OASession, self).get(url, headers = headers)

    def save_cookie(self):
        with open('cookie.txt', 'w') as outfile:
            pickle.dump(self.cookies, outfile)

    def check_cookie(self):
        try:
            with open('cookie.txt', 'r') as infile:
                self.cookies = pickle.load(infile)
                # make a dump request to validate the SMSESSION
                resp = self.get(OASession.oa_url)
                if resp.headers.get('set-cookie', None):
                    return True
                self.cookies.clear()
                return False
        except IOError:
            pass
        return False


    def login(self):
        self.cookies['SignonDefault'] = self.user
        self.cookies['SM_USER'] = self.user

        resp = self.get(OASession.oa_url)

        login_url, params = extract_form(resp)
        params['USER'] = self.user
        params['PASSWORD'] = self.passwd

        login_resp = self.post(login_url, urllib.urlencode(params), Referer = resp.url)

        cred_url, cred_params = extract_form(login_resp)

        cred_resp = self.post(cred_url, urllib.urlencode(params), Referer = login_url)

        ref_url = re.search(r'top.location.href = "(.+)"', cred_resp.text).group(1)

        resp = self.post(urlparse.urljoin(ref_url, 'IndexFrame/IndexRedirect.aspx'), '', Refer = ref_url)

        if resp.text.lower() == 'false':
            location = "/Pages/index.htm"
        else:
            location = "/int/default.htm"

        resp = self.get(urlparse.urljoin(ref_url, location), Refer = ref_url)

        self.cookies.clear(domain = 'sso.oa.wanmei.com')


class Employee(object):
    sign_url = 'http://web.oa.wanmei.com/OaHomePage/Handler/signInOut.ashx'
    min_working_hours = 8

    def __init__(self, **kwargs):
        self.session = OASession(**kwargs)

    def save(self):
        self.session.save_cookie()

    def check_sign_info(self):
        resp = self.session.post(Employee.sign_url, 'method=GetSignInfo')
        result = re.search(r"result:'([\w:]+)'", resp.text).group(1)
        action = re.search(r"actionType:'([\w:]+)'", resp.text).group(1)

        return result == 'true' and action != 'normalNoData'

    def work_time(self):
        resp = self.session.post(Employee.sign_url, 'method=DoSignOutCheck')
        print_wrapper('sign out check result: {}'.format(resp.text))
        workTime = re.search(r"'workTime':'([\d:]+)'", resp.text).group(1)
        return map(int, workTime.split(':'))

    def sign_out(self, interactive = True):
        hour, minute = self.work_time()
        if hour < Employee.min_working_hours:
            if not interactive:
                print_wrapper('working time is less than {} hours'.format(Employee.min_working_hours))
                self.notify_user('工作时间不足{}小时({}:{}), 请手动签退!'.format(Employee.min_working_hours,
                                 hour, minute))
                return
            confirm = raw_input('Your working time is less than {} hours({} hours '
                                'and {} minutes). Are your sure to sign out?[y/n]'.format(
                                    Employee.min_working_hours, hour, minute))
            if confirm[0] != 'y':
                return
        resp = self.session.post(Employee.sign_url, 'method=DoSignOut')
        print_wrapper('sign out result: {}'.format(resp.text))
        result = re.search(r"result:'([\w]+)'", resp.text).group(1)
        action = re.search(r"actionType:'([\w]+)'", resp.text).group(1)
        if result == 'true' and action == 'signOutSuccess':
            print_wrapper('sign out successfully')
            self.notify_user('签退成功!')
        else:
            self.notify_user('非常遗憾地通知您, 自动签退失败，请手动签退!')



    def sign_in(self):
        if self.check_sign_info():
            print_wrapper('already signed in')
            return
        resp = self.session.post(Employee.sign_url, 'method=DoSignIn')
        print_wrapper('sign in result: {}'.format(resp.text))
        result = re.search(r"result:'([\w]+)'", resp.text).group(1)
        action = re.search(r"actionType:'([\w]+)'", resp.text).group(1)

        if result != 'true':
            self.notify_user('非常遗憾地通知您, 自动签到失败，请手动签到!')

        return result == 'true' and action == 'signInSuccess'


    def notify_user(self, message):
        corpid = 'wxe354c66e0b57abcc'
        secrect = '3S7JkNqHnxMtOpJbPdzmb2Q2PxOy8fxOvvh1EYy6e-Qey4L3mJ0B8xLm94_LzeGf'

        response = requests.get('https://qyapi.weixin.qq.com/cgi-bin/gettoken?corpid={}&corpsecret={}'.format(corpid, secrect))
        print_wrapper(response.text)

        token = response.json()['access_token']

        data = {
            'touser': self.session.user,
            'toparty': '',
            'totag': '',
            'msgtype': 'text',
            'agentid': '0',
            'text': {
                    'content': message
                },
            'safe':'0'
        }

        response = requests.post('https://qyapi.weixin.qq.com/cgi-bin/message/send?access_token={}'.format(token),
                                 data = json.dumps(data, ensure_ascii=False) )

        print_wrapper(response.text)


def get_holidays_from_internet(year):
    try:
        response = requests.get('http://fangjia.911cha.com/{}.html'.format(year))
    except:
        return None

    if response.status_code != requests.codes.ok:
        return None

    bsoup = bs4.BeautifulSoup(response.text)
    non_workdays = bsoup.find_all('td', class_ = 'green zhoumo')
    non_workdays += bsoup.find_all('td', class_ = 'jiari')
    pattern = re.compile('\d+年\d+月\d+日')
    holidays = set()
    for tag in non_workdays:
        m = pattern.search(tag['title'].encode('utf-8'))
        if m:
            holidays.add(m.group(0))

    return holidays


def get_holidays(year):
    '''
    Get holidays from local file to speed up the procedure. Though we use the
    name holidays, we include weekends too...
    '''
    file_name = 'holidays_{}'.format(year)
    if os.path.exists(file_name):
        with open(file_name, 'rb') as infile:
            info = pickle.load(infile)
            if info['year'] == year:
                return info['holidays']

    holidays = get_holidays_from_internet(year)
    if not holidays:
        return None

    with open(file_name, 'wb') as outfile:
        pickle.dump({'year' : year, 'holidays' : holidays}, outfile)

    return holidays


def is_workday(year, month, day):
    the_day = '{}年{}月{}日'.format(year, month, day)
    holidays = get_holidays(year)
    if holidays:
        return the_day not in holidays

    # if we cannot get the information, we could only decide whether this
    # day is a workday by its weekday.
    date = datetime.datetime(year, month, day)

    return date.weekday not in (5, 6)


def is_today_workday():
    now = datetime.datetime.now()
    return is_workday(now.year, now.month, now.day)


if __name__ == '__main__':
    try:
        app_dir = os.path.dirname(os.path.abspath(__file__))
    except NameError:
        app_dir = os.path.dirname(os.path.abspath(sys.argv[0]))

    os.chdir(app_dir)

    logger.setLevel(logging.DEBUG)
    formater = logging.Formatter('%(asctime)s %(name)s %(levelname)s - %(message)s')
    fh = logging.FileHandler('auto-sign.log')
    fh.setFormatter(formater)
    fh.setLevel(logging.DEBUG)
    logger.addHandler(fh)

    if not is_today_workday():
        print_wrapper('today is not a workday')
        sys.exit(0)

    optp = optparse.OptionParser('usage: %prog [options] action')
    optp.add_option('-i', '--interactive', help = 'interactive mode',
                    action = 'store_true', dest = 'interactive', default = False)

    optp.add_option('-u', '--user', help = 'the OA user name', dest = 'user')

    optp.add_option('-p', '--passwd', help = 'the password', dest = 'passwd')

    opts, args = optp.parse_args()

    account = Employee(interactive = opts.interactive,
                       user = opts.user, passwd = opts.passwd)

    if len(args) < 1:
        now = datetime.datetime.now()
        if now.hour < 10 or (now.hour == 10 and now.minute <= 30):
            if not account.check_sign_info():
                while not account.sign_in():
                    time.sleep(5)
        else:
            account.sign_out(opts.interactive)
    else:
        if args[0] == 'in':
            account.sign_in()
        elif args[0] == 'out':
            account.sign_out(opts.interactive)
        else:
            account.work_time()

    account.save()
