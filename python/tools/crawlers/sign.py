#!/usr/bin/env python
import re
import urllib
import urlparse
import pickle
import getpass
import requests
import bs4
from datetime import datetime

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

    def __init__(self):
        super(OASession, self).__init__()

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

    def read_cookie(self):
        try:
            with open('cookie.txt', 'r') as infile:
                self.cookies = pickle.load(infile)
                return True
        except:
            pass
        return False

class Employee(object):
    sign_url = 'http://web.oa.wanmei.com/OaHomePage/Handler/signInOut.ashx'
    min_working_hours = 8

    def __init__(self):
        self.session = OASession()
        if not self.session.read_cookie():
            self.user = raw_input('Enter username: ')
            self.passwd = getpass.getpass('Enter password: ')
            self.login()

    def login(self):
        self.session.cookies['SignonDefault'] = self.user
        self.session.cookies['SM_USER'] = self.user

        resp = self.session.get('http://web.oa.wanmei.com/')

        login_url, params = extract_form(resp)
        params['USER'] = self.user
        params['PASSWORD'] = self.passwd

        login_resp = self.session.post(login_url, urllib.urlencode(params), Referer = resp.url)

        cred_url, cred_params = extract_form(login_resp)

        cred_resp = self.session.post(cred_url, urllib.urlencode(params), Referer = login_url)

        ref_url = re.search(r'top.location.href = "(.+)"', cred_resp.text).group(1)

        resp = self.session.post(urlparse.urljoin(ref_url, 'IndexFrame/IndexRedirect.aspx'), '', Refer = ref_url)

        if resp.text.lower() == 'false':
            location = "/Pages/index.htm"
        else:
            location = "/int/default.htm"

        resp = self.session.get(urlparse.urljoin(ref_url, location), Refer = ref_url)

        self.session.cookies.clear(domain = 'sso.oa.wanmei.com')

    def save(self):
        self.session.save_cookie()

    def check_sign_info(self):
        resp = self.session.post(Employee.sign_url, 'method=GetSignInfo')
        print(resp.text)
        result = re.search(r"result:'([\w:]+)'", resp.text).group(1)

        return result == 'true'

    def check_sign_out(self):
        resp = self.session.post(Employee.sign_url, 'method=DoSignOutCheck')
        print(resp.text)

    def sign_out(self):
        resp = check_sign_out(cookie)
        workTime = re.search(r"'workTime':'([\d:]+)'", resp.text).group(1)
        print(workTime)
        hour, minute = map(int, workTime.split(':'))
        if hour < Employee.min_working_hours:
            confirm = raw_input('Your working time is less than {} hours({} hours '
                                'and {} minutes). Are your sure to sign out?[y/n]'.format(
                                    Employee.min_working_hours, hour, minute))
            print(confirm)
            if confirm[0] != 'y':
                return resp
        resp = self.session.post(Employee.sign_url, 'method=DoSignOut')
        print(resp.text)


    def sign_in(self):
        resp = self.session.post(Employee.sign_url, 'method=DoSignIn')
        print(resp.text)
        result = re.search(r"actionType:'([\w]+)'", resp.text).group(1)
        if result != 'signInSuccess':
            print('failed to sign in')


if __name__ == '__main__':
    account = Employee()

    if not account.check_sign_info():
        account.sign_in()
    else:
        now = datetime.now()
        if now.hour > 18:
            account.sign_out()

    account.save()
