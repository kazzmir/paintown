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
    start = includes:include* name:namespace (!"\n" .)* newline s* objs:structs s* <eof> {{ value = state.Program(includes, name, objs) }} 
    structs = (s* obj:struct)+
    include = "include" s+ what:include-string (!"\n" .)* s+ "\n"* {{ value = what }}
    include-string = string
                   | "<" contents:(!">" .)* ">" {{ value = "<%s>" % ''.join(contents) }}
    string = <quote> contents:(!<quote> !"\n" .)* <quote> {{ value = '"' + ''.join(contents) + '"' }}
    namespace = "namespace" s+ id:identifier
    struct = "struct" s* name:identifier s* "{" fields:(s* field)* s* "}" {{
        value = state.State(name)
for field in fields:
    value.addField(field)
    }}
    identifier = a:letter rest:letter_digit* {{ value = a + ''.join(rest) }}
    letter = [abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_]
    digit = [0123456789]
    number = all:digit+ {{ value = ''.join(all) }}
    letter_digit = letter
                  | digit
    array = "[" s* out:number s* "]" {{ value = out }}
    field = obj:struct s* name:identifier array:array? ";"? {{
    if array != None:
    value = state.Field(obj, name, array)
else:
    value = state.Field(obj, name)
        }}
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

def generate_header(object, isState, name = None, array = None):
    def make_init_field(field):
        if isinstance(field.type_, state.State) or isState(str(field.type_)):
            return None
        if field.isArray():
            raise Exception("Handle arrays of non-structs")
        if field.type_.isPOD():
            return '%(name)s = %(zero)s;' % {'name': field.name,
                                             'zero': field.zero()}
        elif not str(field.type_).startswith('std::vector') and not str(field.type_).startswith('std::map') and not str(field.type_).startswith('std::string'):
            import re
            return '%(name)s = default%(type)s();' % {'name': field.name, 'type': re.sub(':', '', str(field.type_))}
        return None

    def make_definition(field):
        array = ''
        if field.isArray():
            array = '[%s]' % field.array
        return '%(type)s %(name)s%(array)s;' % {'name': field.name,
                                                'type': str(field.type_),
                                                'array': array}

    def indent(lines, much):
        if len(lines) == 0:
            return ''
        tabs = '    ' * much
        out = []
        for line in lines:
            out.extend(line.split('\n'))
        return '\n'.join([out[0]] + [tabs + line for line in out[1:]])

    inits = []
    for field in object.fields:
        maybe_init = make_init_field(field)
        if maybe_init != None:
            inits.append(maybe_init)
    definitions = [make_definition(field) for field in object.fields]
    instance = "";
    if name != None:
        instance = " %s" % name;
    arrayUse = ''
    if array != None:
        arrayUse = '[%s]' % array

    more = ""
    for field in object.fields:
        if isinstance(field.type_, state.State):
            more += generate_header(field.type_, isState, field.name, field.array)

    data = """
%(more)s
struct %(name)s{
    %(name)s(){
        %(init-fields)s
    }

    %(definitions)s
};
Token * serialize(const %(name)s & data);
%(name)s deserialize%(name)s(const Token * data);
""" % {'name': object.name,
       'more': more,
       'maybe-instance': instance,
       'init-fields': indent(inits, 2),
       'definitions': indent(definitions, 1),
       'array': arrayUse
      }
    return data

def md5(what):
    import hashlib
    m = hashlib.md5()
    m.update(what)
    return m.hexdigest()

def generate_program_header(program):

    def isState(type_):
        for struct in program.structs:
            if struct.name == type_:
                return True
        return False

    header = "_serialize_%s_%s" % (program.namespace, md5(''.join([generate_header(s, isState) for s in program.structs])))
    includes = '\n'.join(['#include %s' % x for x in program.includes])

    all = ""
    for struct in program.structs:
        all += generate_header(struct, isState)

    data = """
#ifndef %s
#define %s

%s

class Token;

namespace %s{
%s
}

#endif
""" % (header, header, includes, program.namespace, all)
    return data

def generate_cpp(object, structs):
    def serialize_fields(object):
        fields = ""
        name = 0

        for field in object.fields:
            if field.type_.isPOD():
                fields += """    if (data.%(name)s != %(zero)s){
        *out->newToken() << "%(name)s" << data.%(name)s;
    }\n""" % {'name': field.name,
                  'zero': field.zero()}
            elif str(field.type_) == 'std::string':
                fields += """    if (data.%(name)s != ""){
        *out->newToken() << "%(name)s" << data.%(name)s;
    }\n""" % {'name': field.name}
            elif str(field.type_).startswith('std::vector'):
                name += 1
                fields += """
    Token * %(token)s = out->newToken();
    *%(token)s << "%(var)s";
    for (%(type)s::const_iterator it = data.%(var)s.begin(); it != data.%(var)s.end(); it++){
        *%(token)s << serialize(*it);
    }
    """ % {'token': 't%d' % name,
           'type': str(field.type_),
           'var': field.name}
            elif isinstance(field.type_, state.State):
                if field.array != None:
                    times = int(field.array)
                    fields += "    Token * token_%(name)s = out->newToken();\n" % {'name': field.name}
                    fields += """    *token_%(name)s << "%(name)s";\n""" % {'name': field.name} 
                    for i in xrange(0, times):
                        fields += """    *token_%(name)s->newToken() << %(index)d << serialize(data.%(name)s[%(index)d]);\n""" % {'name': field.name, 'index': i}
                else:
                    fields += """    *out->newToken() << "%(name)s" << serialize(data.%(name)s);\n""" % {'name': field.name}
            elif str(field.type_).startswith('std::map'):
                name += 1
                fields += """
    Token * %(token)s = out->newToken();
    *%(token)s << "%(var)s";
    for (%(type)s::const_iterator it = data.%(var)s.begin(); it != data.%(var)s.end(); it++){
        *%(token)s->newToken() << "e" << serialize(it->first) << serialize(it->second);
    }
    """ % {'token': 't%d' % name,
           'type': str(field.type_),
           'var': field.name}

            else:
                if field.array != None:
                    times = int(field.array)
                    for i in xrange(0, times):
                        fields += """    *out->newToken() << "%(name)s" << serialize(data.%(name)s[%(index)d]);\n""" % {'name': field.name, 'index': i}
                else:
                    fields += """    *out->newToken() << "%(name)s" << serialize(data.%(name)s);\n""" % {'name': field.name}

        return fields

    def deserialize_fields(object):
        out = ""
        name = 0
        for field in object.fields:
            path = "_/%(name)s" % {'name': field.name}
            if isinstance(field.type_, state.State):
                if field.isArray():
                    get = ""
                else:
                    get = """const Token * child;
        use->view() >> child;
        out.%(name)s = deserialize%(type)s(child);""" % {'name': field.name,
                                                                    'type': str(field.type_)}
            elif field.type_.isPOD() or str(field.type_) == 'std::string':
                get = "use->view() >> out.%(name)s;" % {'name': field.name}
            elif str(field.type_).startswith('std::vector'):
                import re
                each = ""
                if field.type_.isPOD() or field.type_.element() == 'std::string':
                    type_ = str(field.type_)
                    if not field.type_.isPOD():
                        type_ = field.type_.element()
                    each = """%(type)s element;
            view >> element;
            out.%(name)s.push_back(element);
                    """ % {'name': field.name, 'type': type_}
                else:
                    each = """out.%(name)s.push_back(deserialize%(type)s(view.next()));""" % {'type': re.sub(':', '', str(field.type_.element())),
                            'name': field.name}
                get = """for (TokenView view = use->view(); view.hasMore(); /**/){
            %(each)s
        }
""" % {'each': each}
            elif str(field.type_).startswith('std::map'):
                # TODO
                # get = "map %(name)s" % {'name': field.name}
                import re
                each = ""
                # print field.type_.element()
                typeKey = re.sub(' ', '', field.type_.element().split(',')[0].strip())
                typeValue = re.sub(' ', '', field.type_.element().split(',')[1])
                # print "map: key: '%s' value '%s'" % (typeKey, typeValue)
                if field.type_.isPOD() or field.type_.element() == 'std::string':
                    type_ = str(field.type_)
                    if not field.type_.isPOD():
                        type_ = field.type_.element()
                    each = """%(type)s element;
            view >> element;
            out.%(name)s.push_back(element);
                    """ % {'name': field.name, 'type': type_}
                else:
                    def removeTypeChars(name):
                        return re.sub('<', '',
                                re.sub('>', '',
                                  re.sub(':', '', name)))
                    each = """const Token * entry = view.next();
            %(typeKeyOriginal)s valueKey;
            deserialize_%(typeKey)s(valueKey, entry->getToken(1));
            %(typeValueOriginal)s valueValue;
            deserialize_%(typeValue)s(valueValue, entry->getToken(2));
            out.%(name)s[valueKey] = valueValue;""" % {'typeKey': removeTypeChars(typeKey),
                                     'typeKeyOriginal': str(typeKey),
                                     'typeValue': removeTypeChars(typeValue),
                                     'typeValueOriginal': str(typeValue),
                                     'name': field.name}
                get = """for (TokenView view = use->view(); view.hasMore(); /**/){
            %(each)s
        }
""" % {'each': each}

            else:
                import re
                if field.isArray():
                    get = ""
                else:
                    type_name = re.sub(':', '', str(field.type_))
                    get = "out.%(name)s = deserialize%(type)s(use);" % {'name': field.name,
                                                                        'type': type_name}
                    path = "_/%(name)s/%(type)s" % {'name': field.name,
                                                    'type': type_name}


            out += """    use = data->findToken("%(path)s");
    if (use != NULL){
        %(get)s
    }
""" % {'name': field.name,
       'path': path,
       'get': get}

        return out


    inner_structs = ""
    for field in object.fields:
        if isinstance(field.type_, state.State):
            inner_structs += generate_cpp(field.type_, structs)

    data = """
%(inner)s
Token * serialize(const %(name)s & data){
    Token * out = new Token();
    *out << "%(name)s";
%(field)s
    return out;
}

%(name)s deserialize%(name)s(const Token * data){
    %(name)s out;
    const Token * use = NULL;
%(deserialize)s
    return out;
}
""" % {'inner': inner_structs,
       'name': object.name,
       'deserialize': deserialize_fields(object),
       'field': serialize_fields(object)}
    return data

def generate_program_cpp(program):
    # FIXME: make this variable
    file = "character-state.h"
    all = ""
    for struct in program.structs:
        all += generate_cpp(struct, program.structs)
    data = """
#include "%s"

namespace %s{
%s
}
""" % (file, program.namespace, all)
    return data

def test1():
    parser = create_peg(grammar, 'string')

    test = """foo{
  int tomato
  bool frog
}
"""
    print generate_header(parser(test), lambda x: False)

if len(sys.argv) < 2:
    print "Give an argument"
    sys.exit(0)

cpp = False
for arg in sys.argv[1:]:
    if arg == '--cpp':
        cpp = True
    else:
        input = create_peg(grammar)(arg)
        if cpp:
            print generate_program_cpp(input)
        else:
            print generate_program_header(input)
