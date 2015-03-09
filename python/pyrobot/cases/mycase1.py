#!/usr/bin/env python
# -*- encoding: utf-8 -*-

from casebase import *

# 定义好需要跑的case
cases = [
	Case( LoginCase1 ),  # 使用默认设置, 1 个用户， 登录 100 次
	Case( ChatCase1, userCount = 3, msgCount = 200 )
]

logging.basicConfig(level = logging.DEBUG)
# 执行 case
runCases(cases)

