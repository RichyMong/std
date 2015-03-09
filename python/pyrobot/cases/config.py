#!/usr/bin/env python
# -*- encoding: utf-8 -*-

'''
This file contains the configurations.
'''

DEBUG = True
TRACE = True

loginmgr = {
    'local' : ('10.253.89.156', 9881),
    'qa' : ('66.151.133.143', 9881),
    'staging' : ('66.151.133.142', 9881),
}

imserver = {
    'local' : ('10.253.89.156', 5222),
    'qa' : ('66.151.133.143', 5222),
    'staging' : ('66.151.133.142', 5222),
}

robot = {
    'passwd' : '123456'
}

'''
The values below are default values.
'''
user_prefix = 'arcrobot'
password    = '123456'
user_begin  = 1
user_end    = 100

'''
At present, the user name is in the format arcrobot_0001, ..., arcrobot_0100,
..., arcrobot_1000, so we need to know the max user id to format the user ids
less than it.
'''
user_max    = 1000

msg_cnt     = 20

server_type = 'local'
logdir      = 'logs'


def to_user(prefix, uid, umax = user_max):
    '''
    Combines the prefix and uid to make a user name.
    The returned user name is in the form of prefix_id, where id is str(uid)
    with leading '0' added when necessary to make it len(str(umax)) long.
    '''
    return '{}_{:0{width}}'.format(prefix, uid, width = len(str(umax)))

class ChatCase1:
	user_prefix = 'arcrobot'
	password    = '123456'
	user_count = 2	# 模拟用户数
	msg_count = 100		# 发送消息数，-1 时则无限发送
	server = 'local'

class LoginCase1:
	server = 'local'

