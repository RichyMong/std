#!/usr/bin/env python
import arc
import config

client = arc.login.Client('arcrobot_0001', '123456')
if client.connect(('10.253.89.156', 9881)):
    for game in config.GAMES:
        print(client.get_token(game))
