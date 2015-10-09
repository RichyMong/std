# -*- coding: UTF-8 -*-

import bs4
import requests
import os
import sys
import urlparse
import string
import collections
import urlparse

base_url = 'https://leetcode.com/problemset/algorithms/'

Problem = collections.namedtuple('Problem', ['number', 'title', 'url', 'content'])

def get_solution_title(sno):
    try:
        response = requests.get(base_url)
    except:
        return

    if response.status_code != requests.codes.ok:
        print(response.text)

    bsoup = bs4.BeautifulSoup(response.text)
    table = bsoup.find('table', id='problemList')
    table_body = table.find('tbody')

    rows = table_body.find_all('tr')
    for row in rows:
        # class number title accept difficulty
        records = row.find_all('td')
        href = records[2].find('a')

        cols = [col.text.strip() for col in records]
        if sno is None or sno == int(cols[1]):
            url = urlparse.urljoin('https://leetcode.com', href.get('href'))
            bsoup = bs4.BeautifulSoup(requests.get(url).text)
            content = bsoup.find_all('p')
            yield Problem(cols[1], cols[2], url, content[0].text)
            if sno is not None:
                break

if len(sys.argv) < 2:
    print('usage: {} <problem-number>'.format(sys.argv[0]))
    sys.exit(1)

file_content = string.Template(u'''// ${title} - ${content}
// ${url}
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static

};

int main()
{
    Solution::
}
''')

with open('problems.txt', 'r+') as f:
    existed_problems = {}
    for line in f:
        k, v = line.split(' ', 1)
        existed_problems[k] = v

    sno = int(sys.argv[1])
    cpp_file = 'solution_{}.cpp'.format(sno)
    create_file = True
    if os.path.exists(cpp_file):
        answer = raw_input("file '{}' already exists, overwrite?[y/n]".format(cpp_file))
        if answer[0].lower() == 'n':
            create_file = False

    if create_file:
        for problem in get_solution_title(sno):
            existed_problems[sno] = problem.title

            with open(cpp_file, 'w') as cpp_f:
                cpp_f.write(file_content.substitute(title = problem.title,
                            content = problem.content.encode('ascii', 'ignore'), url = problem.url))

        f.seek(0)
        sorted_sno = sorted(existed_problems.keys(), key = lambda x : int(x))
        for k in sorted_sno:
            f.write('{:{}} {}\n'.format(k, len(str(sorted_sno[-1])),
                existed_problems[k].strip()))

    os.system('vim {}'.format(cpp_file))
