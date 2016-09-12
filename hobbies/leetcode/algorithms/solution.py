# -*- coding: UTF-8 -*-

import bs4
import requests
import os
import sys
import string
import collections
import json

if sys.version_info.major == 3:
    raw_input = input
    import urllib
    urljoin = urllib.parse.urljoin
else:
    import urlparse
    urljoin = urlparse.urljoin

base_url = 'https://leetcode.com/api/problems/algorithms/'

Problem = collections.namedtuple('Problem', ['number', 'title', 'url',
                                             'content', 'decl'])

def find_default_code(text, lang):
    prefix = 'aceCtrl.init('
    if not text.startswith(prefix):
        return None
    text = text[len(prefix):]
    text = text.strip()
    assert text[0] == '['
    parenths = []
    x = ''
    for i in range(1, len(text)):
        if text[i] == ']':
            if not parenths:
                x = text[:i+1]
                break
            parenths.pop(0)

        if text[i] == '[':
            parenths.append(text[i])

    x = x.replace("'", '"')
    x = x[:-2] + ']'
    obj = json.loads(x)
    for z in obj:
        if z['text'].upper() == lang.upper():
            return z['defaultCode']
    return None

def get_solution_title(sno):
    try:
        response = requests.get(base_url)
    except:
        return

    if response.status_code != requests.codes.ok:
        print(response.text)

    json_response = json.loads(response.text)
    for row in json_response['stat_status_pairs']:
        stat = row['stat']
        if sno is None or sno == int(stat['question_id']):
            title = stat['question__title_slug']
            url = 'https://leetcode.com/problems/{}'.format(title)
            bsoup = bs4.BeautifulSoup(requests.get(url).text)
            attr = bsoup.find('div', { 'ng-controller' : 'AceCtrl as aceCtrl' })
            default_code = find_default_code(attr['ng-init'], 'c++')
            content = bsoup.find_all('p')
            yield Problem(sno, title, url, content[0].text, default_code)
            if sno is not None:
                break

if len(sys.argv) < 2:
    print('usage: {} <problem-number>'.format(sys.argv[0]))
    sys.exit(1)

fc_template = string.Template(u'''${title_content}
// ${url}
#include "solution.h"

using namespace std;

${default_code}

int main()
{
    Solution::
}
''')

def make_title_content(title, content):
    prefix = '// '
    max_len = 80 - len(prefix)

    result = ''

    s = '{} - {}'.format(title, content)

    while len(s) > max_len:
        end = max_len - 1

        lspace = end
        left = end - len(prefix)
        while lspace >= left and s[lspace].isalnum():
            lspace -= 1

        rspace = max_len + 3
        while rspace > end and s[rspace].isalnum():
            rspace -= 1

        result += '// '
        if lspace >= left or rspace > end:
            space = lspace if lspace >= left else rspace
            result += s[0:space]
            s = s[space + 1:]
        else:
            result += s[0:max_len - 1] + '-'
            s = s[max_len-1:]

        result += '\n'

    if len(s):
        result += '// ' + s

    print(result)
    return result

with open('problems.txt', 'r+') as f:
    existed_problems = {}
    for line in f:
        k, v = line.split(' ', 1)
        existed_problems[int(k)] = v

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
                title_content = make_title_content(problem.title, problem.content.strip('\r\n').replace('\n\n', '\n').replace('\n', '\n// '))
                file_content = fc_template.substitute(title_content = title_content,
                            url = problem.url, default_code=problem.decl).replace('\r', '')
                cpp_f.write(file_content)

        f.seek(0)
        sorted_sno = sorted(existed_problems.keys(), key = lambda x : int(x))
        for k in sorted_sno:
            f.write('{:<{}} {}\n'.format(k, len(str(sorted_sno[-1])),
                existed_problems[k].strip()))

    os.system('vim {}'.format(cpp_file))
