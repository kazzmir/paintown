#!/usr/bin/env python

# Packrat PEG (parsing expression grammar) generator
# http://pdos.csail.mit.edu/~baford/packrat/

# Python BNF parser:
# 1. 171397b / 45.216s = 3790.62721160651 b/s
# 2. 171397b / 36.751s = 4663.73704116895 b/s
# 3. 171397b / 8.630s = 19860.6025492468 b/s
# 4. 171397b / 10.539s = 16263.1179428788 b/s

next_var = 0
def nextVar():
    global next_var;
    next_var += 1;
    return next_var

def newResult():
    return "result_%d" % nextVar()

def newOut():
    return "out_%d" % nextVar()

def indent(s):
    space = '    '
    return s.replace('\n', '\n%s' % space)

start_code = """
class Result{
public:
    Result(const int position):
    position(position),
    isError(false){
    }

    inline const int getPosition() const {
        return position;
    }

    inline bool error(){
        return isError;
    }

    inline void nextPosition(){
        position += 1;
    }

    void setError(){
        isError = true;
    }

    void setValue(void * value){
        this->values.push_back(value);
    }

    const int matches() const {
        return this->values.size();
    }

    const std::vector<void *> & getValues() const {
        return values;
    }

    void addResult(const Result & result){
        const std::vector<void *> & his = result.getValues();
        this->values.insert(this->values.end(), his.begin(), his.end());
        this->position = result.getPosition();
    }

private:
    int position;
    bool isError;
    std::vector<void *> values;
};

class Stream{
public:
    Stream(const std::string & filename){
        stream.open(filename.c_str());
    }

    char get(const int position){
        char z;
        stream.seekg(position, std::ios_base::beg);
        stream >> z;
        return z;
    }

    void update(const Result & result){
    }

    bool hasResult(const int position){
        return false;
    }

    Result result(const int position){
        return Result(-1);
    }

private:
    std::ifstream stream;
};

Result errorResult(-1);
"""

start_python = """
class PegError(Exception):
    def __init__(self):
        Exception.__init__(self)

class Result:
    def __init__(self, position):
        self.position = position
        self.values = []

    def getPosition(self):
        return self.position

    def nextPosition(self, amount = 1):
        self.position += amount

    def setValue(self, value):
        self.values = value
    
    def matches(self):
        return len(self.values)

    def getValues(self):
        return self.values

    def addResult(self, him):
        self.values.append(him.values)
        self.position = him.position
    
    def extendResult(self, him):
        self.values.extend(him.values)
        self.position = him.position

class Stream:
    def __init__(self, filename):
        self.file = open(filename, 'r')
        self.position = 0
        self.limit = 100
        self.all = self.file.read()
        print "Read " + str(len(self.all))

    def close(self):
        self.file.close()

    def get(self, position, number = 1):
        if position + number > self.limit:
            print (position + number)
            self.limit += 5000
        if position + number >= len(self.all):
            return chr(0)
        # print "stream: %s" % self.all[position:position+number]
        return self.all[position:position+number]

    def get2(self, position):
        if position != self.position:
            self.file.seek(position)
        self.position = position + 1
        if position > self.limit:
            print position
            self.limit += 5000
        return self.file.read(1)

    def update(self, result):
        pass

    def hasResult(self, position):
        return false

    def result(self, position):
        return Result(-1)
"""

class Pattern:
    def __init__(self):
        pass

    def generate(self, result, stream, failure):
        pass

class PatternNot(Pattern):
    def __init__(self, next):
        Pattern.__init__(self)
        self.next = next

    def generate_python(self, result, stream, failure):
        my_result = newResult()
        my_fail = lambda : "raise PegError"
        data = """
%s = Result(%s);
try:
    %s
except PegError:
    %s
        """ % (my_result, result, indent(self.next.generate_python(my_result, stream, my_fail).strip()), failure())

        return data


    def generate(self, result, stream, failure):
        not_label = "not_%d" % nextVar()
        my_result = newResult()
        my_fail = lambda : "goto %s;" % not_label
        data = """
Result %s(%s);
%s
%s
%s:
        """ % (my_result, result, self.next.generate(my_result, stream, my_fail).strip(), failure(), not_label)

        return data

class PatternRule(Pattern):
    def __init__(self, rule):
        Pattern.__init__(self)
        self.rule = rule

    def generate_python(self, result, stream, failure):
        data = """
# print "Trying rule " + '%s'
%s = rule_%s(%s, %s.getPosition())
if %s == None:
    %s
""" % (self.rule, result, self.rule, stream, result, result, failure())

        return data

    def generate(self, result, stream, failure):
        data = """
%s = rule_%s(%s, %s.getPosition());
if (%s.error()){
    %s
}
""" % (result, self.rule, stream, result, result, failure())

        return data

class PatternSequence(Pattern):
    def __init__(self, patterns):
        Pattern.__init__(self)
        self.patterns = patterns

    def generate_python(self, result, stream, failure):
        data = ""
        for pattern in self.patterns:
            my_result = newResult()
            data += """
%s = Result(%s.getPosition())
%s
%s.addResult(%s);
""" % (my_result, result, pattern.generate_python(my_result, stream, failure), result, my_result)

        return data

    def generate(self, result, stream, failure):
        data = ""
        for pattern in self.patterns:
            my_result = newResult()
            data += """
{
Result %s(%s.getPosition());
%s
%s.addResult(%s);
}
""" % (my_result, result, pattern.generate(my_result, stream, failure), result, my_result)
        return data

class PatternRepeatOnce(Pattern):
    def __init__(self, next):
        Pattern.__init__(self)
        self.next = next

    def generate_python(self, result, stream, failure):
        my_fail = lambda : "raise PegError"
        my_result = newResult()
        my_result2 = newResult()
        data = """
try:
    while True:
        %s = Result(%s.getPosition());
        %s
        %s.addResult(%s);
except PegError:
    if %s.matches() == 0:
        %s
        """ % (my_result, result, indent(indent(self.next.generate_python(my_result, stream, my_fail).strip())), result, my_result, result, failure())

        return data


    def generate(self, result, stream, failure):
        loop_done = "loop_%d" % nextVar()
        my_fail = lambda : "goto %s;" % loop_done
        my_result = newResult()
        data = """
do{
    Result %s(%s.getPosition());
    %s
    %s.addResult(%s);
} while (true);
%s:
if (%s.matches() == 0){
    %s
}
        """ % (my_result, result, indent(self.next.generate(my_result, stream, my_fail).strip()), result, my_result, loop_done, result, failure())

        return data

class PatternAction(Pattern):
    def __init__(self, before, code):
        Pattern.__init__(self)
        self.before = before
        self.code = code

    def generate_python(self, result, stream, failure):
        data = """
%s
if True:
    value = None
    values = %s.getValues()
    %s
    %s.setValue(value)
""" % (self.before.generate_python(result, stream, failure).strip(), result, indent(self.code.strip()), result)

        return data

    def generate(self, result, stream, failure):
        data = """
%s
{
    void * value = 0;
    %s
    %s.setValue(value);
}
        """ % (self.before.generate(result, stream, failure).strip(), indent(self.code.strip()), result)

        return data

class PatternRepeatMany(Pattern):
    def __init__(self, next):
        Pattern.__init__(self)
        self.next = next

    def generate_python(self, result, stream, failure):
        my_fail = lambda : "raise PegError"
        my_result = newResult()
        data = """
try:
    while True:
        %s = Result(%s.getPosition());
        %s
        %s.addResult(%s);
except PegError:
    pass
        """ % (my_result, result, indent(indent(self.next.generate_python(my_result, stream, my_fail).strip())), result, my_result)

        return data



    def generate(self, result, stream, failure):
        loop_done = "loop_%d" % nextVar()
        my_fail = lambda : "goto %s;" % loop_done
        my_result = newResult()
        data = """
do{
    Result %s(%s.getPosition());
    %s
    %s.addResult(%s);
} while (true);
%s:
        """ % (my_result, result, indent(self.next.generate(my_result, stream, my_fail).strip()), result, my_result, loop_done)
        return data

class PatternMaybe(Pattern):
    def __init__(self, pattern):
        Pattern.__init__(self)
        self.pattern = pattern

    def generate_python(self, result, stream, failure):
        save = "save_%d" % nextVar()
        data = """
%s = %s.getPosition()
%s
if %s == None:
    %s = Result(%s)
    %s.setValue(None)
""" % (save, result, self.pattern.generate_python(result, stream, lambda : "pass"), result, result, save, result)
        return data

class PatternOr(Pattern):
    def __init__(self, patterns):
        Pattern.__init__(self)
        self.patterns = patterns

    def generate_python(self, result, stream, failure):
        data = ""
        fail = failure
        for pattern in self.patterns[::-1]:
            my_result = newResult()
            data = """
%s = Result(%s.getPosition())
%s
if %s != None:
    %s = %s
""" % (my_result, result, pattern.generate_python(my_result, stream, fail).strip(), my_result, result, my_result)
            fail = lambda : indent(data)
        return data

    def generate(self, result, stream, failure):
        data = ""
        success = "success_%d" % nextVar()
        for pattern in self.patterns:
            out = "or_%d" % nextVar()
            my_result = newResult()
            fail = lambda : "goto %s;" % out
            if pattern == self.patterns[-1]:
                fail = failure
            data += """
{
Result %s(%s.getPosition());
%s
%s = %s;
}
goto %s;
%s:
""" % (my_result, result, pattern.generate(my_result, stream, fail).strip(), result, my_result, success, out)
        data += "%s:\n" % success
        return data

class PatternBind(Pattern):
    def __init__(self, variable, pattern):
        Pattern.__init__(self)
        self.variable = variable
        self.pattern = pattern

    def generate_python(self, result, stream, failure):
        data = """
%s
%s = %s.getValues()
""" % (self.pattern.generate_python(result, stream, failure).strip(), self.variable, result)
        return data

class PatternRange(Pattern):
    def __init__(self, range):
        Pattern.__init__(self)
        self.range = range

    def generate_python(self, result, stream, failure):
        letter = "letter_%d" % nextVar()
        data = """
%s = %s.get(%s.getPosition())
if %s in '%s':
    %s.nextPosition()
    %s.setValue(%s)
else:
    %s
""" % (letter, stream, result, letter, self.range, result, result, letter, failure())

        return data

class PatternVerbatim(Pattern):
    def __init__(self, letters):
        Pattern.__init__(self)
        self.letters = letters

    def generate_python2(self, result, stream, failure):
        data = """
for letter in '%s':
    if letter == %s.get(%s.getPosition()):
        %s.nextPosition()
    else:
        %s
%s.setValue('%s')
""" % (self.letters, stream, result, result, failure(), result, self.letters)
        return data

    def generate_python(self, result, stream, failure):
        length = len(self.letters)
        if self.letters == "\\n" or self.letters == "\\t":
            length = 1
        data = """
if '%s' == %s.get(%s.getPosition(), %s):
    %s.nextPosition(%s)
else:
    %s
%s.setValue('%s')
""" % (self.letters, stream, result, length, result, length, failure(), result, self.letters)
        return data

    def generate(self, result, stream, failure):
        data = """
%s = "%s";
        """ % (result, self.letters)

        data = ""

        for letter in self.letters[::-1]:
            newdata = """
if (%s.get(%s.getPosition()) == '%s'){
    %s.nextPosition();
    %s
} else {
    %s
}
""" % (stream, result, letter, result, indent(data), failure())
            data = newdata

        # return data

        data = """
for (int i = 0; i < %d; i++){
    if ("%s"[i] == %s.get(%s.getPosition())){
        %s.nextPosition();
    } else {
        %s
    }
}
""" % (len(self.letters), self.letters, stream, result, result, failure())
        return data

class Rule:
    def __init__(self, name, patterns):
        self.name = name
        self.patterns = patterns

    def generate_python(self):
        def newPattern(pattern, stream, position):
            result = newResult()

            def fail():
                return "raise PegError"
            data = """
try:
    %s = Result(%s)
    %s
    %s.update(%s)
    return %s
except PegError:
    pass
            """ % (result, position, indent(pattern.generate_python(result, stream, fail).strip()), stream, result, result)
            return data

        stream = "stream"
        position = "position"
        data = """
def rule_%s(%s, %s):
    %s
    return None
""" % (self.name, stream, position, indent('\n'.join([newPattern(pattern, stream, position).strip() for pattern in self.patterns])))

        return data

    def generate(self):
        def newPattern(pattern, stream, position):
            result = newResult()
            out = newOut()
            def failure():
                return "goto %s;" % out
            data = """
Result %s(%s);
%s
%s.update(%s);
return %s;
%s:
            """ % (result, position, pattern.generate(result, stream, failure).strip(), stream, result, result, out)
            return data

        stream = "stream"
        position = "position"
        data = """
Result rule_%s(Stream & %s, const int %s){
    if (%s.hasResult(%s)){
        return %s.result(%s);
    }
    %s
    return errorResult;
}
        """ % (self.name, stream, position, stream, position, stream, position, indent('\n'.join([newPattern(pattern, stream, position).strip() for pattern in self.patterns])))

        return data
    
class Peg:
    def __init__(self, namespace, start, rules):
        self.namespace = namespace
        self.start = start
        self.rules = rules

    def generate_python(self):
        data = """
import peg

%s

%s

def parse(file):
    # print "Parsing " + file
    stream = Stream(file)
    done = rule_%s(stream, 0)
    stream.close()
    if done == None:
        print "Error parsing " + file
        return []
    else:
        return done.getValues()
""" % (start_python, '\n'.join([rule.generate_python() for rule in self.rules]), self.start)

        return data

    def generate(self):
        def prototype(rule):
            return "Result rule_%s(Stream &, const int);" % rule.name

        data = """
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

namespace %s{
    %s

    %s

    %s

std::vector<void *> main(const std::string & filename){
    Stream stream(filename);
    errorResult.setError();
    Result done = rule_%s(stream, 0);
    if (done.error()){
        std::cout << "Could not parse" << std::endl;
    }
    return done.getValues();
}

}
        """ % (self.namespace, start_code, indent('\n'.join([prototype(rule) for rule in self.rules])), '\n'.join([rule.generate() for rule in self.rules]), self.start)

        return data

def generate(peg):
    print peg.generate()

def test():
    s_code = """
printf("parsed cheese\\n");
value = (void *) 2;
"""
    rules = [
        Rule("s", [PatternNot(PatternVerbatim("hello")), PatternAction(PatternVerbatim("cheese"), s_code), PatternRepeatOnce(PatternVerbatim("once"))]),
        Rule("blah", [PatternRepeatMany(PatternRule("s"))]),
        Rule("or", [PatternOr([PatternVerbatim("joe"), PatternVerbatim("bob"), PatternVerbatim("sally")])]),
        Rule("all", [PatternSequence([PatternVerbatim("abc"), PatternVerbatim("def"), PatternVerbatim("ghi")])]),
    ]
    peg = Peg("Peg", "s", rules)
    generate(peg)

def create_peg(peg):
    # import imp
    # module = imp.new_module(peg.namespace)
    # exec peg.generate_python() in module.__dict__
    # return module.parse

    name = "peg_" + peg.namespace
    out = open(name + ".py", 'w')
    out.write(peg.generate_python())
    out.close()
    module = __import__(name, globals(), locals(), ['parse'])
    # print module
    # print dir(module)
    return module.parse

def test2():
    start_code_abc = """
std::cout << "Parsed abc!" << std::endl;
"""
    start_code_def = """
std::cout << "Parsed def!" << std::endl;
"""
    rules = [
        Rule("start", [
            PatternAction(PatternSequence([PatternRule("a"),PatternRule("b"), PatternRule("c")]), start_code_abc),
            PatternAction(PatternSequence([PatternRule("d"),PatternRule("e"), PatternRule("f")]), start_code_def),
        ]),
        Rule("a", [PatternVerbatim("a")]),
        Rule("b", [PatternVerbatim("b")]),
        Rule("c", [PatternVerbatim("c")]),

        Rule("d", [PatternVerbatim("d")]),
        Rule("e", [PatternVerbatim("e")]),
        Rule("f", [PatternVerbatim("f")]),
    ]

    peg = Peg("Peg", "start", rules)
    generate(peg)

def make_peg_parser():
    start_code_abc = """
print "parsed abc"
"""
    s_code = """
print "s code"
"""
    rules = [
        Rule("start", [
            PatternAction(PatternSequence([PatternRule("a"),PatternRule("b"), PatternRule("c")]), start_code_abc),
            ]),
        Rule("a", [PatternVerbatim("a")]),
        Rule("b", [PatternVerbatim("b")]),
        Rule("c", [PatternVerbatim("c")]),

        Rule("s", [
            PatternNot(PatternVerbatim("hello")), PatternAction(PatternVerbatim("cheese"), s_code),
            PatternRepeatOnce(PatternVerbatim("once"))]),
        Rule("blah", [PatternRepeatMany(PatternRule("s"))]),
        ]

    rules = [
        Rule("start", [
            PatternAction(PatternSequence([PatternRule("start_symbol"), PatternRule("newlines"), PatternRule("rules")]), """
start_symbol = values[0]
rules = values[2]
value = peg.Peg('xx', start_symbol, rules)
""")
            ]),
        Rule("word", [
            PatternAction(PatternRepeatOnce(PatternRule("any_char")), """
# print "all start symbol values " + str(values)
# print "values[0] " + str(values[0])
value = ''.join(values)
# print "got word " + value
""")
            ]),
        Rule("rules", [
            PatternAction(
                PatternSequence([
                    PatternVerbatim("rules:"),
                    PatternRule("newlines"),
                    PatternRepeatMany(PatternRule("rule"))
                    ]),
                """value = values[2]""")
            ]),
        Rule("rule", [
            PatternAction(PatternSequence([
                PatternRule("spaces"),
                PatternBind("name", PatternRule("word")),
                PatternRule("spaces"),
                PatternVerbatim("="),
                PatternRule("spaces"),
                PatternBind("pattern1", PatternRepeatMany(PatternRule("pattern"))),
                PatternRule("newlines"),
                PatternBind("patterns",
                    PatternRepeatMany(PatternAction(PatternSequence([
                        PatternRule("spaces"),
                        PatternVerbatim("|"),
                        PatternRule("spaces"),
                        PatternBind("pattern", PatternRepeatMany(PatternRule("pattern"))),
                        PatternRule("newlines")]),
                        """
# value = values[3]
value = pattern
"""
)))]),
                """
# name = values[1]
# pattern1 = values[5]
# patterns = values[7]
#print "pattern name is " + str(name)
#print "first pattern is " + str(pattern1)
#print "other patterns are " + str(patterns)
value = peg.Rule(name, [peg.PatternSequence(pattern) for pattern in ([pattern1] + patterns)])
""")
            ]),
        Rule("pattern", [
            PatternAction(PatternSequence([
                PatternOr([
                    PatternRule("x_word"),
                    PatternRule("string"),
                    PatternRule("sub_pattern")]),
                PatternMaybe(PatternRule("modifier")),
                PatternRule("spaces")]),
                """
# value = peg.PatternRule(values[0])
modifier = values[1]
value = values[0]
if modifier != None:
    value = modifier(value)
# print "Pattern is " + str(value)
""")
            ]),
        Rule("sub_pattern", [
            PatternAction(PatternSequence([
                PatternVerbatim("("),
                PatternRepeatOnce(PatternRule("pattern")),
                PatternVerbatim(")"),
                ]),"""
value = peg.PatternSequence(values[1])
"""),
            ]),
        Rule("string", [
            PatternAction(PatternSequence([
                PatternVerbatim("\""),
                PatternRule('word'),
                PatternVerbatim("\""),
                ]), """
value = peg.PatternVerbatim(values[1])
"""),
            ]),
        Rule("modifier", [
            PatternAction(PatternVerbatim("*"),"""
value = lambda p: peg.PatternRepeatMany(p)
"""),
            PatternAction(PatternVerbatim("?"),"""
value = lambda p: peg.PatternMaybe(p)
"""),
            PatternAction(PatternVerbatim("+"),"""
value = lambda p: peg.PatternRepeatOnce(p)
"""),
            ]),
        Rule("x_word", [
            PatternAction(PatternRule("word"), """
value = peg.PatternRule(values[0])
"""),
            ]),
        Rule("start_symbol", [
            PatternAction(PatternSequence([PatternVerbatim("start-symbol:"), PatternRepeatMany(PatternRule("space")), PatternRule("word")]), """
value = values[2]
# print 'start symbol is ' + str(value)
""")
            ]),
        Rule("spaces", [PatternRepeatMany(PatternRule("space"))]),
        # Rule("space", [PatternRange(' \t')]),
        Rule("space", [PatternVerbatim(" "), PatternVerbatim("\\t")]),
        Rule("any_char", [PatternRange('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ')]),
        # Rule("any_char", [PatternVerbatim(letter) for letter in 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ']),
        Rule("newlines", [PatternRepeatMany(PatternVerbatim("\\n"))]),
    ]

    peg = Peg("peg", "start", rules)
    # print peg.generate_python()
    parser = create_peg(peg)
    # print parser
    return parser
    answer = parser('peg.in')
    print "Got " + str(answer)
    # print answer.generate()
    # module = compile(peg.generate_python(), peg.namespace, 'exec')
    # print module

# test()
# test2()

# make_peg_parser()
if __name__ == '__main__':
    import sys
    parser = make_peg_parser()
    # out = parser('peg.in.x')
    if len(sys.argv) > 1:
        out = parser(sys.argv[1])
        print out.generate()
