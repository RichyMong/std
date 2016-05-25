from multiprocessing import Process
import os

class Task(object):
    def __init__(self, v):
        self.snid = v

    def loop(self):
        print(self.snid)

t = Task(20)
p = Process(target = t.loop, args = ())
p.start()
os.wait()
