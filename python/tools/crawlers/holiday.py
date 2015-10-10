# -*- coding: UTF-8 -*-

import bs4
import requests
import datetime
import re
import os
import pickle


class Holiday(object):
    HOLIDAYS = {}

    @staticmethod
    def get_holidays_from_internet(year):
        try:
            response = requests.get('http://fangjia.911cha.com/{}.html'.format(year))
        except:
            return None

        if response.status_code != requests.codes.ok:
            return None

        bsoup = bs4.BeautifulSoup(response.text)
        non_workdays = bsoup.find_all('td', class_ = 'green zhoumo')
        non_workdays += bsoup.find_all('td', class_ = 'jiari')
        pattern = re.compile('\d+年\d+月\d+日')
        holidays = set()
        for tag in non_workdays:
            m = pattern.search(tag['title'].encode('utf-8'))
            if m:
                holidays.add(m.group(0))

        return holidays


    @staticmethod
    def get_holidays(year):
        '''
        Get holidays from local file to speed up the procedure. Though we use the
        name holidays, we include weekends too...
        '''
        file_name = 'holidays'
        if not Holiday.HOLIDAYS and os.path.exists(file_name):
            with open(file_name, 'rb') as infile:
                Holiday.HOLIDAYS = pickle.load(infile)

        if year in Holiday.HOLIDAYS:
            return Holiday.HOLIDAYS[year]

        holidays = Holiday.get_holidays_from_internet(year)
        if not holidays:
            return None

        Holiday.HOLIDAYS[year] = holidays

        with open(file_name, 'wb') as outfile:
            pickle.dump(Holiday.HOLIDAYS, outfile)

        return Holiday.HOLIDAYS[year]


    @staticmethod
    def is_workday(year, month, day):
        the_day = '{}年{}月{}日'.format(year, month, day)
        holidays = Holiday.get_holidays(year)
        if holidays:
            if the_day in holidays:
                print('{} is not a working day'.format(the_day))
                return False
            else:
                print('{} is a working day'.format(the_day))
                return True
        else:
            # if we cannot get the information, we could only decide whether this
            # day is a workday by its weekday.
            date = datetime.datetime(year, month, day)
            if date.weekday in (5, 6):
                print('{} is not a working day'.format(the_day))
                return False
            print('{} is a working day'.format(the_day))
            return True

        return True


    @staticmethod
    def is_today_workday():
        now = datetime.datetime.now()
        return Holiday.is_workday(now.year, now.month, now.day)


Holiday.is_workday(2015, 9, 1)
Holiday.is_workday(2015, 9, 2)
Holiday.is_workday(2015, 9, 3)
Holiday.is_workday(2015, 9, 4)
Holiday.is_workday(2015, 9, 5)
Holiday.is_workday(2015, 9, 6)
Holiday.is_workday(2015, 9, 7)
Holiday.is_workday(9999, 9, 7)
Holiday.is_workday(2015, 8, 30)
Holiday.is_today_workday()
