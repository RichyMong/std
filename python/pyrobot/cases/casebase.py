#!/usr/bin/env python
# -*- encoding: utf-8 -*-

# 20141127 - written by xiaotao
from __future__ import print_function
import sys
import os
import time
import arc
import arc.chat
import arc.login
import logging
from multiprocessing import Process, Queue
import config # 基本配置

class CaseBase(object):
	def __init__(self, name, desc):
		self._name = name
		self._desc = desc

	def run(self):
		logging.info("Case: [%s] running..." % (self._name))
		logging.info("Description: %s" % (self._desc))

	def done(self):
		logging.info("Case: [%s] Done" % (self._name))
		# 在此统计测试信息并记log

def Case(caseClass, **kvargs):
	case = caseClass()
	for (k, v) in kvargs.items():
		case.__setattr__(k, v)
	return case

class LoginCase1(CaseBase):

	userCount = 1
	repeatCount = 100

	def __init__(self):
		CaseBase.__init__(self, "登录测试1", "进行高强度的登录测试")

	@staticmethod
	def userLoop(self, userName):
		i = 0
		while True:
			try:
				client = arc.login.Client(userName, config.password)
				client.connect(config.loginmgr[config.LoginCase1.server])
				logging.info("%s login ok" % userName)
				client.disconnect()
				if self.repeatCount != -1:
					i += 1
				if i >= self.repeatCount:
					break
			except Excetpion, e:
				print(e)
				#raise

	def run(self):
		CaseBase.run(self)
		children = set()
		for i in range(self.userCount):
			child = Process(target = LoginCase1.userLoop,
				args = (self, config.to_user(config.user_prefix, i + 1)) )
			child.start()
			children.add(child)
		map(lambda p: p.join(), children)

	def done(self):
		CaseBase.done(self)

class ChatCase1(CaseBase):

	userCount = 2
	msgCount = 100

	def __init__(self):
		CaseBase.__init__(self, "聊天测试1",
				"""模拟用户对服务器进行聊天测试。
				userCount 是用户数;
				msgCount 是每个用户发送的消息数, -1时为无限发送;""")

	@staticmethod
	def charHandler(obj, userName):
		def wrapper(client):
			i = 0;
			while True:
				for n in range(obj.userCount):
					target = config.to_user(config.ChatCase1.user_prefix, n + 1)
					if target == userName:
						continue # skip self
					body = 'msg: [%s] => [%s]; time: %s' % (userName, target, time.strftime('%Y-%m-%d:%H:%M:%S(%s)'))
					logging.debug(body)
					client.send_message(arc.chat.make_jid(target), body)
					time.sleep(1)
				if obj.msgCount != -1:
					i += 1
				if i >= obj.msgCount:
					break
			client.disconnect()
			sys.exit(0)
		return wrapper

	def messageHandler(self, msg):
		# TODO: 发送返回消息；对消息记录进行分析， 了解消息返回的成功率
		pass

	@staticmethod
	def userLoop(self, userName):
		token = arc.login.get_im_token(userName, config.password,
								       config.loginmgr[config.ChatCase1.server])
		client = arc.chat.Client(userName, token)
		if not client.connect(config.imserver[config.ChatCase1.server]):
			logger.error('"%s" cannot connect to server' % userName)
			return
		client.add_ready_handler(ChatCase1.charHandler(self, userName))
		#self.add_event_handler("message", self.messageHandler)
		client.process(block = True)

	def run(self):
		CaseBase.run(self)
		#print( self.userCount, self.msgCount)
		children = set()
		for i in range(self.userCount):
			child = Process(target = ChatCase1.userLoop,
				args = (self, config.to_user(config.ChatCase1.user_prefix, i + 1)) )
			child.start()
			children.add(child)
		map(lambda p: p.join(), children)

	def done(self):
		CaseBase.done(self)
		# 在此统计测试信息并记log

def runCases(cases):
	for case in cases:
		case.run()
		case.done()

# unit testing
if __name__ == '__main__':
	logging.basicConfig(level=logging.DEBUG)
	case1 = ChatCase1()
	case1.run()

