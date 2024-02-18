#!/bin/python3

## REQUIRES sexpdata https://github.com/jd-boyd/sexpdata/
## Ubuntu - apt install python3-sexpdata
import sys, io, yaml, collections.abc
from os.path import exists
from sexpdata import load, Symbol

if len(sys.argv) == 1:
    print("Usage: {0} s-expression.file [output.yaml]".format(sys.argv[0]))
    exit()
if not exists(sys.argv[1]):
    print("Could not find file {}".format(sys.argv[1]))

print("Opening file {}".format(sys.argv[1]))
file = open(sys.argv[1])

values = load(file)
#print("Have total tokens: {}".format(len(values)))
converted = { values[0] : {} } # Head of tokens

def ExtractToken(node):
    if isinstance(node, list):
        name = str(node[0])
        values = { name : [] }
        for num, expr in enumerate(node):
            if num == 0:
                continue
            values[name].append(ExtractToken(expr))
        return values
    else:
        if isinstance(node, Symbol):
            return str(node)
        return node

tokens = ExtractToken(values)

#print(tokens)

output = './output.yaml'
if len(sys.argv) == 3:
    output = sys.argv[2]

with open(output, 'w') as file:
    print("Writing yaml file: {}".format(output))
    yaml.dump(tokens, file)


