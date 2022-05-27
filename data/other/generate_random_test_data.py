#!/usr/bin/python
# -*- coding: utf-8 -*-

import csv
import random
import string

NUMBER_OF_ROWS = (
    #    100,
    #    1000,
    #    10000,
    #    100000,
    #    1000000,
    #    10000000
    10000,
    20000,
    30000,
    40000,
    50000,
    60000,
    70000,
    80000,
    90000,
    100000
)

TABLE_COLUMNS = (
    'id',
    'val'
)

NUMBER_OF_CHARACTERS = (
    10,
    #    100,
    #    1000
)


def generate_float_data():
    for row_number in NUMBER_OF_ROWS:
        file = open('float_{}.csv'.format(row_number), 'w')
        writer = csv.DictWriter(file, TABLE_COLUMNS)
        writer.writerow(dict(zip(TABLE_COLUMNS, TABLE_COLUMNS)))
        for i in range(0, row_number):
            writer.writerow(dict([('id', i), ('val', random.random())]))
        file.close()
        print('{} is generated.'.format(file.name))


def generate_int8_data():
    for row_number in NUMBER_OF_ROWS:
        file = open('int8_{}.csv'.format(row_number), 'w')
        writer = csv.DictWriter(file, TABLE_COLUMNS)
        writer.writerow(dict(zip(TABLE_COLUMNS, TABLE_COLUMNS)))
        for i in range(0, row_number):
            writer.writerow(dict(
                [('id', i), ('val', random.randint(-9223372036854775808, +9223372036854775807))]))
        file.close()
        print('{} is generated.'.format(file.name))


def generate_text_data():
    for row_number in NUMBER_OF_ROWS:
        for char_number in NUMBER_OF_CHARACTERS:
            file = open('text_{}_{}.csv'.format(row_number, char_number), 'w')
            writer = csv.DictWriter(file, TABLE_COLUMNS)
            writer.writerow(dict(zip(TABLE_COLUMNS, TABLE_COLUMNS)))
            for i in range(0, row_number):
                writer.writerow(dict([('id', i), ('val', ''.join(
                    (random.choice(string.ascii_lowercase) for j in range(char_number))))]))
            file.close()
            print('{} is generated.'.format(file.name))


def main():
    generate_float_data()
    generate_int8_data()
    generate_text_data()


if __name__ == '__main__':
    main()
