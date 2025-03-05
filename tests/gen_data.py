import csv
import random
import string
import argparse

NUMBER_OF_ARRAY_ELEMENTS = 3
NUMBER_OF_CHARACTERS = 8
DELIMITER = ','
TYPES = ['float_array', 'float', 'int8_array', 'int64', 'text_array', 'text']
TREES = ['mtree', 'cube']

def generate_float_data(path, number_of_rows):
    values = []
    for _ in range(number_of_rows):
        values.append(random.random())

    for tree in TREES:
        filename = f'float_{number_of_rows}_{tree}.csv'
        with open(f'{path}/{filename}', 'w') as file:
            for value in values:
                if tree == 'cube':
                    file.write(f'({value})\n')
                else:
                    file.write(f'{value}\n')
            print(f'{filename} is generated.')

def generate_float_array_data(path, number_of_rows):
    values = []
    for _ in range(number_of_rows):
        values.append([random.random() for _ in range(NUMBER_OF_ARRAY_ELEMENTS)])

    for tree in TREES:
        filename = f'float_array_{number_of_rows}_{tree}.csv'
        with open(f'{path}/{filename}', 'w') as file:
            for value in values:
                if tree == 'cube':
                    file.write(f'({','.join(str(x) for x in value)})\n')
                else:
                    file.write(f'{','.join(str(x) for x in value)}\n')
            print(f'{filename} is generated.')

def generate_int8_data(path, number_of_rows):
    values = []
    for _ in range(number_of_rows):
        values.append(random.randint(-9223372036854775808, 9223372036854775807))

    for tree in TREES:
        filename = f'int8_{number_of_rows}_{tree}.csv'
        with open(f'{path}/{filename}', 'w') as file:
            for value in values:
                if tree == 'cube':
                    file.write(f'({value})\n')
                else:
                    file.write(f'{value}\n')
            print(f'{filename} is generated.')

def generate_int8_array_data(path, number_of_rows):
    values = []
    for _ in range(number_of_rows):
        values.append([random.randint(-9223372036854775808, 9223372036854775807) for _ in range(NUMBER_OF_ARRAY_ELEMENTS)])

    for tree in TREES:
        filename = f'int8_array_{number_of_rows}_{tree}.csv'
        with open(f'{path}/{filename}', 'w') as file:
            for value in values:
                if tree == 'cube':
                    file.write(f'({','.join(str(x) for x in value)})\n')
                else:
                    file.write(f'{','.join(str(x) for x in value)}\n')
            print(f'{filename} is generated.')

def generate_text_data(path, number_of_rows):
    values = []
    for _ in range(number_of_rows):
        values.append(''.join(random.choice(string.ascii_lowercase) for _ in range(NUMBER_OF_CHARACTERS)))

    for tree in TREES:
        filename = f'text_{number_of_rows}_{tree}.csv'
        with open(f'{path}/{filename}', 'w') as file:
            for value in values:
                if tree == 'cube':
                    file.write(f'({value})\n')
                else:
                    file.write(f'{value}\n')
            print(f'{filename} is generated.')

def generate_text_array_data(path, number_of_rows):
    values = []
    for _ in range(number_of_rows):
        values.append([''.join(random.choice(string.ascii_lowercase) for _ in range(NUMBER_OF_CHARACTERS)) for _ in range(NUMBER_OF_ARRAY_ELEMENTS)])

    for tree in TREES:
        filename = f'text_array_{number_of_rows}_{tree}.csv'
        with open(f'{path}/{filename}', 'w') as file:
            for value in values:
                if tree == 'cube':
                    file.write(f'({','.join(x for x in value)})\n')
                else:
                    file.write(f'{','.join(x for x in value)}\n')
            print(f'{filename} is generated.')

def main():
    parser = argparse.ArgumentParser(description='Generate various types of data files.')
    parser.add_argument('-t', '--type', choices=TYPES, required=True, help='specify data type')
    parser.add_argument('-n', '--number_of_rows', type=int, help='specify the number of rows')
    parser.add_argument('-p', '--path', type=str, help='specify where to save the files')
    
    args = parser.parse_args()

    number_of_rows = 10
    if args.number_of_rows != None:
        number_of_rows = args.number_of_rows
    
    if args.type == 'float':
        generate_float_data(args.path, number_of_rows)
    elif args.type == 'float_array':
        generate_float_array_data(args.path, number_of_rows)
    elif args.type == 'int64':
        generate_int8_data(args.path, number_of_rows)
    elif args.type == 'int8_array':
        generate_int8_array_data(args.path, number_of_rows)
    elif args.type == 'text':
        generate_text_data(args.path, number_of_rows)
    elif args.type == 'text_array':
        generate_text_array_data(args.path, number_of_rows)

if __name__ == '__main__':
    main()