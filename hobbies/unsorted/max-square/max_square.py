#!/usr/bin/env python
from __future__ import print_function

def print_matrix(matrix):
    row, column = len(numbers), len(numbers[0])
    for i in xrange(row):
        for j in xrange(column):
            print(matrix[i][j], end = ' ')
        print('')

def max_square(numbers):
    print_matrix(numbers)
    row, column = len(numbers), len(numbers[0])
    result = [[0 for i in xrange(column)] for j in xrange(row)]
    for i in xrange(row):
        result[i][0] = numbers[i][0]

    for j in xrange(column):
        result[0][j] = numbers[0][j]

    for i in xrange(1, row):
        for j in xrange(1, column):
            if numbers[i][j] == 0:
                result[i][j] = 0
            else:
                result[i][j] = min((result[i][j-1], result[i-1][j], result[i-1][j-1])) + 1

    print('result')
    print_matrix(result)
    max_s, max_i, max_j = result[0][0], 0, 0
    for i in xrange(0, row):
        for j in xrange(0, column):
            if result[i][j] > max_s:
                max_s = result[i][j]
                max_i = i
                max_j = j
    print(i, j, max_s)


row_col = raw_input()
row, col = row_col.split()
numbers = []
for i in xrange(int(row)):
    line = raw_input()
    numbers.append([int(x) for x in line.split()])

max_square(numbers)
