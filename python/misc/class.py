#!/usr/bin/env python3

class Base(object):
    def __init__(self):
        self.callback = self.do_work

    def do_work(self):
        print('Base.do_work')


class Derived(Base):
    def __init__(self):
        super().__init__()

    def do_work(self):
        print('Derived.do_work')

if __name__ == '__main__':
    Derived().callback()
