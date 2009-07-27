#!/usr/bin/env python

# Packrat PEG (parsing expression grammar) generator
# http://pdos.csail.mit.edu/~baford/packrat/

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

class Pattern:
    def __init__(self):
        pass

    def generate(self, result, stream, failure):
        pass

class PatternNot(Pattern):
    def __init__(self, next):
        Pattern.__init__(self)
        self.next = next

    def generate(self, result, stream, failure):
        not_label = "not_%d" % nextVar()
        my_fail = lambda : "goto %s;" % not_label
        data = """
%s
%s
%s:
        """ % (self.next.generate(result, stream, my_fail).strip(), failure(), not_label)

        return data

class PatternRule(Pattern):
    def __init__(self, rule):
        Pattern.__init__(self)
        self.rule = rule

    def generate(self, result, stream, failure):
        data = """
%s = rule_%s(%s.getPosition());
        """ % (result, self.rule, result)

        return data

class PatternRepeatOnce(Pattern):
    def __init__(self, next):
        Pattern.__init__(self)
        self.next = next

    def generate(self, result, stream, failure):
        result = newResult()
        data = """
        """

        return data

class PatternAction(Pattern):
    def __init__(self, before, code):
        Pattern.__init__(self)
        self.before = before
        self.code = code

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

    def generate(self, result, stream, failure):
        my_result = newResult()
        data = """
do{
    Result %s(%s.getPosition());
    %s
    %s.addResult(%s);
} while (%s.ok());
        """ % (my_result, result, indent(self.next.generate(my_result, stream, failure).strip()), result, my_result, result)
        return data

class PatternVerbatim(Pattern):
    def __init__(self, letters):
        Pattern.__init__(self)
        self.letters = letters

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
static Result rule_%s(Stream & %s, const int %s){
    %s
}
        """ % (self.name, stream, position, indent('\n'.join([newPattern(pattern, stream, position).strip() for pattern in self.patterns])))

        return data
    
class Peg:
    def __init__(self, start, rules):
        self.start = start
        self.rules = rules

    def generate(self):
        namespace = "Peg"
        data = """
namespace %s{
    %s

Result main(){
    return rule_%s();
}

}
        """ % (namespace, '\n'.join([rule.generate() for rule in self.rules]), self.start)

        return data

def generate(peg):
    print peg.generate()

def test():
    s_code = """
printf("parsed cheese\\n");
value = 2;
"""
    rules = [
        Rule("s", [PatternNot(PatternVerbatim("hello")), PatternAction(PatternVerbatim("cheese"), s_code)]),
        Rule("blah", [PatternRepeatMany(PatternRule("s"))])
    ]
    peg = Peg("s", rules)
    generate(peg)

test()
