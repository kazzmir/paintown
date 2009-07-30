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
    return "peg_%d" % next_var

def newResult():
    return "result_%s" % nextVar()

def newOut():
    return "out_%s" % nextVar()

def indent(s):
    space = '    '
    return s.replace('\n', '\n%s' % space)

start_code = """
struct Value{
    Value():
        which(1){
    }

    Value(const Value & him){
        which = him.which;
        value = him.value;
        values = him.values;
    }

    Value(void * value):
        which(0),
        value(value){
    }

    Value & operator=(const Value & him){
        which = him.which;
        value = him.value;
        values = him.values;
        return *this;
    }

    int which; // 0 is value, 1 is values

    inline const bool isList() const {
        return which == 1;
    }

    inline const bool isData() const {
        return which == 0;
    }

    inline const void * getValue() const {
        return value;
    }

    inline void setValue(const void * value){
        which = 0;
        this->value = value;
    }

    inline const std::vector<Value> & getValues() const {
        return values;
    }

    inline void setValues(std::vector<Value> values){
        which = 1;
        values = values;
    }

    const void * value;
    std::vector<Value> values;
};

class Result{
public:
    Result():
    position(-2){
    }

    Result(const int position):
    position(position){
    }

    Result(const Result & r){
        position = r.position;
        value = r.value;
    }

    Result & operator=(const Result & r){
        position = r.position;
        value = r.value;
    }

    inline const int getPosition() const {
        return position;
    }

    inline bool error(){
        return position == -1;
    }

    inline bool calculated(){
        return position != -2;
    }

    inline void nextPosition(){
        position += 1;
    }

    void setError(){
        position = -1;
    }

    void setValue(const Value & value){
        this->value = value;
    }

    const int matches() const {
        if (value.isList()){
            return this->value.values.size();
        } else {
            return 1;
        }
    }

    const Value & getValues() const {
        return this->value;
    }

    void addResult(const Result & result){
        if (this->value.isList()){
            std::vector<Value> & mine = this->value.values;
            mine.push_back(result.getValues());
            this->position = result.getPosition();
        } else {
            std::cout << "Error: trying to add results to a single value" << std::endl; 
        }
    }

private:
    int position;
    Value value;
};

class Stream{
public:
    Stream(const std::string & filename):
    buffer(0){
        std::ifstream stream;
        stream.open(filename.c_str());
        stream.seekg(0, std::ios_base::end);
        max = stream.tellg();
        stream.seekg(0, std::ios_base::beg);
        buffer = new char[max];
        stream.read(buffer, max);
        stream.close();
    }

    char get(const int position){
        if (position >= max || position < 0){
            return '\\0';
        }

        return buffer[position];
        /*
        char z;
        stream.seekg(position, std::ios_base::beg);
        stream >> z;
        return z;
        */
    }

    bool find(const char * str, const int position){
        if (position >= max || position < 0){
            return false;
        }
        return strncmp(&buffer[position], str, max - position) == 0;
    }

    void update(const int rule, const int position, const Result & result){
        memo[rule][position] = result;
    }

    bool hasResult(const int rule, const int position){
        return memo[rule][position].calculated();
    }

    Result result(const int rule, const int position){
        return memo[rule][position];
    }

    ~Stream(){
        delete[] buffer;
    }

private:
    char * buffer;
    std::map<const int, std::map<const int, Result> > memo;
    int max;
};

Result errorResult(-1);
"""

start_python = """
class PegError(Exception):
    def __init__(self):
        Exception.__init__(self)

class NotError(Exception):
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
    
    #def extendResult(self, him):
    #    self.values.extend(him.values)
    #    self.position = him.position

class Stream:
    def __init__(self, filename):
        self.file = open(filename, 'r')
        self.position = 0
        self.limit = 100
        self.all = self.file.read()
        # print "Read " + str(len(self.all))

    def close(self):
        self.file.close()

    def get(self, position, number = 1):
        if position + number > self.limit:
            # print (position + number)
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

    def parens(self, pattern, str):
        if pattern.contains() > 1:
            return "(%s)" % str
        else:
            return str

class PatternNot(Pattern):
    def __init__(self, next):
        Pattern.__init__(self)
        self.next = next

    def ensureRules(self, find):
        self.next.ensureRules(find)

    def generate_bnf(self):
        return "!" + self.next.generate_bnf()

    def generate_python(self, result, stream, failure):
        my_result = newResult()
        my_fail = lambda : "raise NotError"
        data = """
%s = Result(%s.getPosition());
try:
    %s
    %s
except NotError:
    %s.setValue(None)
        """ % (my_result, result, indent(self.next.generate_python(my_result, stream, my_fail).strip()), failure(), result)

        return data


    def generate_cpp(self, result, stream, failure):
        not_label = "not_%s" % nextVar()
        my_result = newResult()
        my_fail = lambda : "goto %s;" % not_label
        data = """
Result %s(%s);
%s
%s
%s:
%s.setValue((void*)0);
        """ % (my_result, result, self.next.generate_cpp(my_result, stream, my_fail).strip(), failure(), not_label, result)

        return data

class PatternRule(Pattern):
    def __init__(self, rule):
        Pattern.__init__(self)
        self.rule = rule

    def contains(self):
        return 1

    def ensureRules(self, find):
        if not find(self.rule):
            print "*warning* could not find rule " + self.rule

    def generate_bnf(self):
        return self.rule

    def generate_python(self, result, stream, failure):
        data = """
# print "Trying rule " + '%s'
%s = rule_%s(%s, %s.getPosition())
if %s == None:
    %s
""" % (self.rule, result, self.rule, stream, result, result, indent(failure()))

        return data

    def generate_cpp(self, result, stream, failure):
        data = """
%s = rule_%s(%s, %s.getPosition());
if (%s.error()){
    %s
}
""" % (result, self.rule, stream, result, result, indent(failure()))

        return data

class PatternEof(Pattern):
    def __init__(self):
        Pattern.__init__(self)

    def ensureRules(self, find):
        pass

    def generate_bnf(self):
        return "<eof>"

    def generate_python(self, result, stream, failure):
        data = """
if chr(0) == %s.get(%s.getPosition()):
    %s.nextPosition()
    %s.setValue(chr(0))
else:
    %s
""" % (stream, result, result, result, indent(failure()))
        return data

    def generate_cpp(self, result, stream, failure):
        data = """
if ('\\0' == %s.get(%s.getPosition())){
    %s.nextPosition();
    %s.setValue((void *) '\\0');
} else {
    %s
}
""" % (stream, result, result, result, indent(failure()))

        return data

class PatternSequence(Pattern):
    def __init__(self, patterns):
        Pattern.__init__(self)
        self.patterns = patterns

    def contains(self):
        return len(self.patterns)

    def ensureRules(self, find):
        for pattern in self.patterns:
            pattern.ensureRules(find)

    def generate_bnf(self):
        return "%s" % " ".join([p.generate_bnf() for p in self.patterns])

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

    def generate_cpp(self, result, stream, failure):
        data = ""
        for pattern in self.patterns:
            my_result = newResult()
            data += """
{
    Result %s(%s.getPosition());
    %s
    %s.addResult(%s);
}
""" % (my_result, result, indent(pattern.generate_cpp(my_result, stream, failure).strip()), result, my_result)
        return data

class PatternRepeatOnce(Pattern):
    def __init__(self, next):
        Pattern.__init__(self)
        self.next = next

    def ensureRules(self, find):
        self.next.ensureRules(find)

    def generate_bnf(self):
        return self.parens(self.next, self.next.generate_bnf()) + "+"

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


    def generate_cpp(self, result, stream, failure):
        loop_done = "loop_%s" % nextVar()
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
        """ % (my_result, result, indent(self.next.generate_cpp(my_result, stream, my_fail).strip()), result, my_result, loop_done, result, indent(failure()))

        return data

class PatternAction(Pattern):
    def __init__(self, before, code):
        Pattern.__init__(self)
        self.before = before
        self.code = code

    def ensureRules(self, find):
        self.before.ensureRules(find)

    def generate_bnf(self):
        data = """%s {{
%s
}}""" % (self.before.generate_bnf(), self.code)
        return data

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

    def fixup_cpp(self, code):
        import re
        fix = re.compile("\$(\d+)")
        return re.sub(fix, r"values.getValues()[\1-1]", code)
        #if fix.match(code):
        #    return code.sub("$1", "values.getValues()[0]")

    def generate_cpp(self, result, stream, failure):
        data = """
%s
{
    Value value((void*) 0);
    Value values = %s.getValues();
    %s
    %s.setValue(value);
}
        """ % (self.before.generate_cpp(result, stream, failure).strip(), result, self.fixup_cpp(indent(self.code.strip())), result)

        return data

class PatternRepeatMany(Pattern):
    def __init__(self, next):
        Pattern.__init__(self)
        self.next = next

    def ensureRules(self, find):
        self.next.ensureRules(find)

    def generate_bnf(self):
        return self.parens(self.next, self.next.generate_bnf()) + "*"

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

    def generate_cpp(self, result, stream, failure):
        loop_done = "loop_%s" % nextVar()
        my_fail = lambda : "goto %s;" % loop_done
        my_result = newResult()
        data = """
do{
    Result %s(%s.getPosition());
    %s
    %s.addResult(%s);
} while (true);
%s:
        """ % (my_result, result, indent(self.next.generate_cpp(my_result, stream, my_fail).strip()), result, my_result, loop_done)
        return data

class PatternAny(Pattern):
    def __init__(self):
        Pattern.__init__(self)

    def ensureRules(self, find):
        pass

    def generate_python(self, result, stream, failure):
        data = """
%s.setValue(%s.get(%s.getPosition()))
%s.nextPosition()
""" % (result, stream, result, result)
        return data

class PatternMaybe(Pattern):
    def __init__(self, pattern):
        Pattern.__init__(self)
        self.pattern = pattern

    def ensureRules(self, find):
        self.pattern.ensureRules(find)

    def generate_bnf(self):
        return self.parens(self.pattern, self.pattern.generate_bnf()) + "?"

    def generate_python(self, result, stream, failure):
        save = "save_%s" % nextVar()
        fail = lambda : """
%s = Result(%s)
%s.setValue(None)
""" % (result, save, result)

        data = """
%s = %s.getPosition()
%s
""" % (save, result, self.pattern.generate_python(result, stream, fail))
        return data

    def generate_cpp(self, result, stream, failure):
        save = "save_%s" % nextVar()
        fail = lambda : """
%s = Result(%s);
%s.setValue((void*) 0);
""" % (result, save, result)
        data = """
int %s = %s.getPosition();
%s
""" % (save, result, self.pattern.generate_cpp(result, stream, fail))
        return data

class PatternOr(Pattern):
    def __init__(self, patterns):
        Pattern.__init__(self)
        self.patterns = patterns

    def ensureRules(self, find):
        for pattern in self.patterns:
            pattern.ensureRules(find)

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
            fail = lambda : data
        return data

    def generate_cpp(self, result, stream, failure):
        data = ""
        success = "success_%s" % nextVar()
        for pattern in self.patterns:
            out = "or_%s" % nextVar()
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
""" % (my_result, result, pattern.generate_cpp(my_result, stream, fail).strip(), result, my_result, success, out)
        data += "%s:\n" % success
        return data

class PatternBind(Pattern):
    def __init__(self, variable, pattern):
        Pattern.__init__(self)
        self.variable = variable
        self.pattern = pattern

    def ensureRules(self, find):
        self.pattern.ensureRules(find)

    def generate_cpp(self, result, stream, failure):
        data = """
%s
Value %s = %s.getValues();
""" % (self.pattern.generate_cpp(result, stream, failure).strip(), self.variable, result)
        return data

    def generate_bnf(self):
        return "%s:%s" % (self.variable, self.pattern.generate_bnf())

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

    def ensureRules(self, find):
        pass

    def generate_bnf(self):
        return "[%s]" % self.range

    def generate_cpp(self, result, stream, failure):
        letter = "letter_%s" % nextVar()
        data = """
char %s = %s.get(%s.getPosition());
if (strchr("%s", %s) != NULL){
    %s.nextPosition();
    %s.setValue((void*) %s);
} else {
    %s
}
""" % (letter, stream, result, self.range, letter, result, result, letter, indent(failure()))
        return data

    def generate_python(self, result, stream, failure):
        letter = "letter_%s" % nextVar()
        data = """
%s = %s.get(%s.getPosition())
if %s in '%s':
    %s.nextPosition()
    %s.setValue(%s)
else:
    %s
""" % (letter, stream, result, letter, self.range, result, result, letter, indent(failure()))

        return data

class PatternVerbatim(Pattern):
    def __init__(self, letters):
        Pattern.__init__(self)
        self.letters = letters

    def ensureRules(self, find):
        pass

    def contains(self):
        return 1

    def generate_bnf(self):
        return '"%s"' % self.letters

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
    %s.setValue('%s')
else:
    %s
""" % (self.letters, stream, result, length, result, length, result, self.letters, indent(failure()))
        return data

    def generate_cpp(self, result, stream, failure):
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
%s.setValue((void*) "%s");
""" % (len(self.letters), self.letters, stream, result, result, indent(indent(failure())), result, self.letters)
        return data

class Rule:
    def __init__(self, name, patterns, inline = False):
        self.name = name
        self.patterns = patterns
        self.inline = inline

    def generate_bnf(self):
        data = """
%s = %s
""" % (self.name, (('\n%s | ') % (' ' * len(self.name))).join([p.generate_bnf() for p in self.patterns]))
        if self.inline:
            return "inline " + data.strip() + "\n"
        return data.strip() + "\n"

    def ensureRules(self, find):
        for pattern in self.patterns:
            pattern.ensureRules(find)

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

    def generate_cpp(self):
        rule_number = "RULE_%s" % self.name
        def newPattern(pattern, stream, position):
            result = newResult()
            out = newOut()
            def failure():
                return "goto %s;" % out
            data = """
Result %s(%s);
%s
%s.update(%s, %s, %s);
return %s;
%s:
            """ % (result, position, pattern.generate_cpp(result, stream, failure).strip(), stream, rule_number, position, result, result, out)
            return data

        stream = "stream"
        position = "position"
        data = """
Result rule_%s(Stream & %s, const int %s){
    if (%s.hasResult(%s, %s)){
        return %s.result(%s, %s);
    }
    %s
    %s.update(%s, %s, errorResult);
    return errorResult;
}
        """ % (self.name, stream, position, stream, rule_number, position, stream, rule_number, position, indent('\n'.join([newPattern(pattern, stream, position).strip() for pattern in self.patterns])), stream, rule_number, position)

        return data
    
class Peg:
    def __init__(self, namespace, start, rules):
        self.namespace = namespace
        self.start = start
        self.rules = rules

        for rule in self.rules:
            rule.ensureRules(lambda r: r in [r2.name for r2 in self.rules])

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
        return None
    else:
        return done.getValues()
""" % (start_python, '\n'.join([rule.generate_python() for rule in self.rules]), self.start)

        return data

    def generate_bnf(self):
        data = """
start-symbol: %s
rules:
    %s
""" % (self.start, indent('\n'.join([rule.generate_bnf() for rule in self.rules]).strip()))
        return data

    def generate_cpp(self):
        def prototype(rule):
            return "Result rule_%s(Stream &, const int);" % rule.name

        r = 0
        rule_numbers = '\n'.join(["const int RULE_%s = %d;" % (x[0].name, x[1]) for x in zip(self.rules, range(0, len(self.rules)))])

        data = """
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <string.h>

namespace %s{
    %s

    %s

    %s

    %s

const void * main(const std::string & filename){
    Stream stream(filename);
    errorResult.setError();
    Result done = rule_%s(stream, 0);
    if (done.error()){
        std::cout << "Could not parse" << std::endl;
    }
    return done.getValues().getValue();
}

}
        """ % (self.namespace, start_code, indent('\n'.join([prototype(rule) for rule in self.rules])), indent(rule_numbers), '\n'.join([rule.generate_cpp() for rule in self.rules]), self.start)

        return data

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
    print peg.generate_cpp()

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
    print peg.generate_cpp()

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
            PatternAction(PatternSequence([
                PatternRule("start_symbol"),
                PatternRule("newlines"),
                PatternRule("rules"),
                PatternRule("newlines"),
                PatternEof(),
                ]), """
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
                PatternBind("inline", PatternMaybe(PatternVerbatim("inline"))),
                PatternRule("spaces"),
                PatternBind("name", PatternRule("word")),
                PatternRule("spaces"),
                PatternVerbatim("="),
                PatternRule("spaces"),
                PatternBind("pattern1", PatternRule("pattern_line")),
                PatternRule("newlines"),
                PatternBind("patterns",
                    PatternRepeatMany(PatternAction(PatternSequence([
                        PatternRule("spaces"),
                        PatternVerbatim("|"),
                        PatternRule("spaces"),
                        PatternBind("pattern", PatternRule("pattern_line")),
                        # PatternBind("pattern", PatternRepeatMany(PatternRule("pattern"))),
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
# value = peg.Rule(name, [peg.PatternSequence(pattern) for pattern in ([pattern1] + patterns)])
value = peg.Rule(name, [pattern1] + patterns, inline = (inline != None))
# print "Patterns are " + str([pattern1] + patterns)
# value = peg.Rule(name, [pattern1] + patterns)
""")
            ]),
        Rule("pattern_line",[
            PatternAction(PatternSequence([
                PatternBind("patterns", PatternRepeatMany(PatternRule("pattern"))),
                PatternRule("spaces"),
                PatternBind("code", PatternMaybe(PatternRule("code"))),
                ]), """
if code != None:
    value = code(peg.PatternSequence(patterns))
else:
    value = peg.PatternAction(peg.PatternSequence(patterns), "value = values;")
"""),
            ]),
        Rule("pattern", [
            PatternAction(PatternSequence([
                PatternBind("bind", PatternMaybe(PatternRule("bind"))),
                PatternBind("item", PatternRule("item")),
                PatternRule("spaces"),]),
                """
# value = peg.PatternRule(values[0])
if bind != None:
    item = bind(item)
value = item
# print "Pattern is " + str(value)
""")
            ]),
        Rule("code", [
            PatternAction(PatternSequence([
                PatternVerbatim("{{"),
                PatternRepeatOnce(PatternAction(PatternSequence([
                    PatternNot(PatternVerbatim("}}")),
                    PatternAny(),
                    ]),
                    """
value = values[1]
""")),
                PatternVerbatim("}}"),
                ]), """
value = lambda p: peg.PatternAction(p, ''.join(values[1]))
"""),
            ]),
        Rule("item", [
            PatternAction(PatternSequence([
                PatternBind("pnot", PatternMaybe(PatternVerbatim("!"))),
                PatternBind("pattern",
                    PatternOr([
                        PatternRule("x_word"),
                        PatternRule("eof"),
                        PatternRule("range"),
                        PatternRule("string"),
                        PatternRule("sub_pattern")])),
                    PatternBind("modifier", PatternMaybe(PatternRule("modifier")))]), """
if modifier != None:
    pattern = modifier(pattern)
if pnot != None:
    pattern = peg.PatternNot(pattern)
value = pattern
"""),
            ]),
        Rule("eof", [
            PatternAction(PatternVerbatim("<eof>"),"""value = peg.PatternEof()""")
            ]),
        Rule("range", [
            PatternAction(PatternSequence([
                PatternVerbatim("["),
                PatternRepeatMany(PatternAction(PatternSequence([
                    PatternNot(PatternVerbatim("]")),
                    PatternAny(),
                    ]),"value = values[1]")),
                PatternVerbatim("]"),
                ]),"""
value = peg.PatternRange(''.join(values[1]))
"""),
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
        Rule("bind", [
            PatternAction(PatternSequence([
                PatternBind("name", PatternRule("word")),
                PatternVerbatim(":"),
                ]),"""
value = lambda p: peg.PatternBind(name, p)
"""),
            ]),
        Rule("string", [
            PatternAction(PatternSequence([
                PatternVerbatim("\""),
                PatternRepeatMany(PatternAction(PatternSequence([
                    PatternNot(PatternVerbatim("\"")),
                    PatternAny(),
                    ]),"value = values[1]")),
                PatternVerbatim("\""),
                ]), """
value = peg.PatternVerbatim(''.join(values[1]))
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
value = peg.PatternRule(values)
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
        Rule("any_char", [PatternRange('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890')]),
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

def help():
    print "-h,--help,help : Print this help"
    print "--bnf : Generate BNF (grammar language)"
    print "--python : Generate python parser"
    print "--cpp,--c++ : Generate c++ parser"

# make_peg_parser()
if __name__ == '__main__':
    import sys
    parser = make_peg_parser()
    # out = parser('peg.in.x')
    doit = []
    file = None
    for arg in sys.argv[1:]:
        if arg == '--bnf':
            doit.append(lambda p: p.generate_bnf())
        elif arg == '--cpp' or arg == '-c++':
            doit.append(lambda p: p.generate_cpp())
        elif arg == '--python':
            doit.append(lambda p: p.generate_python())
        elif arg == '-h' or arg == '--help' or arg == 'help':
            help()
        else:
            file = arg

    if file != None:
        out = parser(file)
        # print out
        if out != None:
            if len(doit) == 0:
                print "Grammar file looks good!. Use some options to generate code"
            else:
                for generate in doit:
                    print generate(out)
        else:
            print "Uh oh, couldn't parse " + file + ". Are you sure its using BNF format?"
    else:
        help()
        print "Give a grammar file as an argument"

