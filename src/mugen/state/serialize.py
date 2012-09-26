#!/usr/bin/env python

import peg

# This script reads a specification of a datastructure with fields in it and writes
# out a class that contains those fields and a way to serialize/deserialize them
# to a stream. This is similar to google's protobuf but using a much simpler
# implementation.

# TODO: grammar of specification
grammar = """
start-symbol: start
module: state
rules:
    start = object:identifier s* "{" fields:(s* field)* s* "}"
    identifier = a:letter rest:letter_number*
    letter = [abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ]
    number = [0123456789]
    letter_number = letter
                  | number
    field = type:identifier s+ name:identifier
    s = " "
      | "\t"
      | "\n"
      | "\r"
"""

def create_peg(grammar, kind = 'file'):
    # Get the parser for BNF
    peg_parser = peg.create_peg(peg.peg_bnf('peg'), 'string')
    # Then use it to parse the grammar and return a new peg
    return peg.create_peg(peg_parser(grammar), kind)

parser = create_peg(grammar, 'string')

test = "foo{}"

parser(test)
