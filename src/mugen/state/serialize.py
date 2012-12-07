#!/usr/bin/env python

import sys
sys.path.append('../parser')
import peg, state

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
    start = space:namespace (!"\n" .)* newline s* obj:struct {{ value = obj }}
    namespace = "namespace" s+ id:identifier
    struct = "struct" s* name:identifier s* "{" fields:(s* field)* s* "}" {{
        value = state.State(name)
for field in fields:
    value.addField(field)
    }}
    identifier = a:letter rest:letter_digit* {{ value = a + ''.join(rest) }}
    letter = [abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_]
    digit = [0123456789]
    number = digit+
    letter_digit = letter
                  | digit
    array = "[" s* number s* "]"
    field = obj:struct s* name:identifier array? ";"? {{ value = state.Field(obj, name) }}
          | type:type-identifier s+ name:identifier ";"? {{ value = state.Field(type, name) }}
    # Hack to use unsigned here because we don't want to specialize for c/c++
    type-modifier = "unsigned"
                  | "signed"
    # Handles c++ style namespaces
    type-identifier = modifier:type-modifier? s* name:identifier template:template? namespace:("::" type-identifier)? {{ value = state.makeType(modifier, name, template, namespace) }}
    template = "<" s* arg1:type-identifier args:(s* "," s* type-identifier)* s* ">" {{ value = [arg1] + args }}
    newline = "\n"
    s = " "
      | "\t"
      | "\n"
      | "\r"
      | comment
    comment = "//" (!"\n" .)* "\n"
            | "/*" inner-comment* "*/"
    inner-comment = comment
                  | !"*/" .
"""

def create_peg(grammar, kind = 'file'):
    # Get the parser for BNF
    peg_parser = peg.create_peg(peg.peg_bnf('peg'), 'string')
    # Then use it to parse the grammar and return a new peg
    return peg.create_peg(peg_parser(grammar), kind)

def generate_cpp(object):
    def make_init_field(field):
        return '%(name)s = %(zero)s;' % {'name': field.name,
                                         'zero': field.zero()}

    def make_definition(field):
        if isinstance(field.type_, state.State):
            return generate_cpp(field.type_)
        return '%(type)s %(name)s;' % {'name': field.name,
                                       'type': field.type_}

    def indent(lines, much):
        tabs = '    ' * much
        out = []
        for line in lines:
            out.extend(line.split('\n'))
        return '\n'.join([out[0]] + [tabs + line for line in out[1:]])

    inits = [make_init_field(field) for field in object.fields]
    definitions = [make_definition(field) for field in object.fields]
    data = """
struct %(name)s{
    %(name)s(){
        %(init-fields)s
    }

    %(definitions)s
};
""" % {'name': object.name,
       'init-fields': indent(inits, 2),
       'definitions': indent(definitions, 1)
      }
    return data

def test1():
    parser = create_peg(grammar, 'string')

    test = """foo{
  int tomato
  bool frog
}
"""
    print generate_cpp(parser(test))

if len(sys.argv) < 2:
    print "Give an argument"
    sys.exit(0)

print generate_cpp(create_peg(grammar)(sys.argv[1]))
