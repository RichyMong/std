'''
This file contains the configurations.
'''

DEBUG = True
TRACE = True

loginmgr = {
    'local' : ('localhost', 9881),
    'aws' : ('internal-arc-test-1556766788.us-west-2.elb.amazonaws.com', 9881),
    'auto' : ('10.253.36.9', 9881),
    'qa' : ('66.151.133.143', 9881),
    'staging' : ('66.151.133.142', 9881),
}

imserver = {
    'local' : ('localhost', 5222),
    'aws' : ('internal-arc-test-1556766788.us-west-2.elb.amazonaws.com', 5222),
    'qa' : ('66.151.133.143', 5222),
    'auto' : ('10.253.36.9', 5222),
    'staging' : ('66.151.133.142', 5222),
}

corecenter = {
    'local' : ('localhost', 9777),
    'aws' : ('172.31.30.14', 9777),
    'qa' : ('66.151.133.143', 9777),
    'auto' : ('10.253.36.9', 9777),
    'staging' : ('66.151.133.142', 9777),
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

GAMES = (
    'ArcGameSharedToken',
    'ArcIMToken',
    'pwi',
    'jd',
    'boi',
    'woi',
    'swm',
    'tl',
    'tl2',
    'sto',
    'rh',
    'rz',
    'nw',
    'eso',
    'co',
    'bl',
    'fw',
    'fire',
    'pwe',
    'dota2',
)


def to_user(prefix, uid, umax = user_max):
    '''
    Combines the prefix and uid to make a user name.
    The returned user name is in the form of prefix_id, where id is str(uid)
    with leading '0' added when necessary to make it len(str(umax)) long.
    '''
    # return '{}_{:0{width}}'.format(prefix, uid, width = len(str(umax)))
    return prefix + '_' + (len(str(user_max)) - len(str(uid))) * '0' + str(uid)

def to_user2(prefix, uid):
    return '{}_{}'.format(prefix, uid)
