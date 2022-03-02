import csv
import random

ROW_COUNT = 1000
FIELDS = ['id', 'value']

WRITER = csv.DictWriter(open("{}_float.csv".format(ROW_COUNT), "w"), fieldnames=FIELDS)

WRITER.writerow(dict(zip(FIELDS, FIELDS)))
for i in range(0, ROW_COUNT):
  WRITER.writerow(dict([('id', i), ('value', random.random())]))
