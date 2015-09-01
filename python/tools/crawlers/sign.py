# -*- coding: UTF-8 -*-

#!/usr/bin/env python

'''
Try to sign in automatically on workdays. We decide if a day is a working
day from holiday information provided by fangjia.911cha.com.
If you do extra work on non workdays, please sing in manually.
'''
import sys
import os
import re
import urllib
import urlparse
import pickle
import getpass
import requests
import bs4
import optparse
import time
import datetime


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

    def __init__(self, interactive = False):
        super(OASession, self).__init__()
        if not self.check_cookie():
            if interactive:
                self.user = raw_input('Enter username: ')
                self.passwd = getpass.getpass('Enter password: ')
            else:
                self.user = 'mengfanke'
                self.passwd = 'wanmei'
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

    def __init__(self, interactive = False):
        self.session = OASession(interactive)

    def save(self):
        self.session.save_cookie()

    def check_sign_info(self):
        resp = self.session.post(Employee.sign_url, 'method=GetSignInfo')
        result = re.search(r"result:'([\w:]+)'", resp.text).group(1)
        action = re.search(r"actionType:'([\w:]+)'", resp.text).group(1)

        return result == 'true' and action != 'normalNoData'

    def work_time(self):
        resp = self.session.post(Employee.sign_url, 'method=DoSignOutCheck')
        print(resp.text)
        workTime = re.search(r"'workTime':'([\d:]+)'", resp.text).group(1)
        return map(int, workTime.split(':'))

    def sign_out(self, interactive = True):
        hour, minute = self.work_time()
        if hour < Employee.min_working_hours:
            if not interactive:
                return
            confirm = raw_input('Your working time is less than {} hours({} hours '
                                'and {} minutes). Are your sure to sign out?[y/n]'.format(
                                    Employee.min_working_hours, hour, minute))
            if confirm[0] != 'y':
                return
        resp = self.session.post(Employee.sign_url, 'method=DoSignOut')
        print(resp.text)
        result = re.search(r"result:'([\w]+)'", resp.text).group(1)
        action = re.search(r"actionType:'([\w]+)'", resp.text).group(1)
        if result == 'true' and action == 'signOutSuccess':
            print('sign out successfully')


    def sign_in(self):
        if self.check_sign_info():
            print('already signed in')
            return
        resp = self.session.post(Employee.sign_url, 'method=DoSignIn')
        print(resp.text)
        result = re.search(r"result:'([\w]+)'", resp.text).group(1)
        action = re.search(r"actionType:'([\w]+)'", resp.text).group(1)

        return result == 'true' and action == 'signInSuccess'


def is_workday(year, month, day):
    error = False

    try:
        response = requests.get('http://fangjia.911cha.com/{}.html'.format(year))
    except:
        error = True

    the_day = '{}年{}月{}日'.format(year, month, day)

    if error or response.status_code != requests.codes.ok:
        # if we cannot get the information, we could only decide whether this
        # day is a workday by its weekday.
        date = datetime.datetime(year, month, day)
        if date.weekday in (5, 6):
            print('{} is not a working day'.format(the_day))
            return False
        return True

    bsoup = bs4.BeautifulSoup(response.text)
    non_workdays = bsoup.find_all('td', class_ = 'green zhoumo')
    non_workdays += bsoup.find_all('td', class_ = 'jiari')
    for tag in non_workdays:
        if tag['title'].encode('utf-8').find(the_day) > 0:
            print('{} is not a working day'.format(the_day))
            return False
    return True


def is_today_workday():
    now = datetime.datetime.now()
    return is_workday(now.year, now.month, now.day)


if __name__ == '__main__':
    if not is_today_workday():
        sys.exit(0)

    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    optp = optparse.OptionParser('usage: %prog [options] action')
    optp.add_option('-q', '--quiet', help = 'non-interactive mode',
                    action = 'store_false', dest = 'interactive', default = True)
    opts, args = optp.parse_args()

    account = Employee(opts.interactive)

    if len(args) < 1:
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
