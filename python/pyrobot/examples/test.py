import arc
import config

client = arc.login.Client('arcrobot_0001', '123456')

for i in xrange(10):
    client.connect(config.loginmgr['local'])
    token = client.get_token()
    print(token)
    client.disconnect()
