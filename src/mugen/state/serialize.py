#!/usr/bin/env python

import peg

# This script reads a specification of a datastructure with fields in it and writes
# out a class that contains those fields and a way to serialize/deserialize them
# to a stream. This is similar to google's protobuf but using a much simpler
# implementation.

grammar = """
start-symbol: start
module: state
code: {{
import state    
}}
rules:
    start = name:identifier s* "{" fields:(s* field)* s* "}" {{
        value = state.State(name)
for field in fields:
    value.addField(field)
    }}
    identifier = a:letter rest:letter_number* {{ value = a + ''.join(rest) }}
    letter = [abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ]
    number = [0123456789]
    letter_number = letter
                  | number
    field = type:identifier s+ name:identifier {{ value = state.Field(type, name) }}
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

test = """foo{
  int tomato
  bool frog
}
"""

def generate_cpp(object):
    def make_init_field(field):
        return '%(name)s = %(zero)s;' % {'name': field.name,
                                         'zero': field.zero()}

    def indent(lines, much):
        tabs = '    ' * much
        return '\n'.join([lines[0]] + [tabs + line for line in lines[1:]])

    inits = [make_init_field(field) for field in object.fields]
    data = """
struct %(name)s{
    %(name)s(){
        %(init-fields)s
    }
};
""" % {'name': object.name,
       'init-fields': indent(inits, 2)}
    print data

generate_cpp(parser(test))
