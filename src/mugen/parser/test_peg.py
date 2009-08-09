#!/usr/bin/env python

class TestException(Exception):
    def __init__(self, message):
        Exception.__init__(self, message)

def erase(file):
    import os
    try:
        os.remove(file)
    except OSError:
        pass

def write(data, file):
    f = open(file, 'w')
    f.write(data)
    f.close()

def rootPath():
    return ".test"

file_count = 0
def newFile(suffix = ""):
    import os
    global file_count
    file_count += 1
    return "file%d%s" % (file_count, suffix)
    # return os.path.join(rootPath(), "file%d%s" % (file_count, suffix))

def get_peg_output(option, grammar):
    import subprocess
    peg_out = subprocess.Popen(['./peg.py', option, grammar], stdout = subprocess.PIPE)
    code = peg_out.wait()
    out, err = peg_out.communicate()
    if code != 0:
        raise TestException(out)
    return out

def do_bnf(name, grammar):
    print "[%s] Test bnf.." % name
    # peg_out = subprocess.Popen(['./peg.py', '--bnf', grammar], stdout = subprocess.PIPE)
    # out, err = peg_out.communicate()
    out = get_peg_output('--bnf', grammar)
    g2 = ".bnf2"
    write(out, g2)
    out2 = get_peg_output('--bnf', g2)
    # peg_out2 = subprocess.Popen(['./peg.py', '--bnf', g2], stdout = subprocess.PIPE)
    # out2, err2 = peg_out2.communicate()
    erase(g2)
    if out != out2:
        print "error with bnf generation!!"
        print out
        print "vs"
        print out2
        return False
    return True

def do_python(name, grammar, input):
    # import subprocess
    print "[%s] Test python.." % name
    out = get_peg_output('--python', grammar)
    # peg_out = subprocess.Popen(['./peg.py', '--python', grammar], stdout = subprocess.PIPE)
    # out, err = peg_out.communicate()
    file = 'test_python.py'
    write(out, file)
    x = __import__('test_python')
    result = x.parse(input)
    erase(file)
    erase(file + 'c')
    if result == None:
        print "Error with python parser"
        return False
    return True

def do_cpp(name, grammar, input):
    import subprocess
    print "[%s] Test c++.." % name
    out = get_peg_output("--cpp", grammar)
    cpp = '.test_cpp.cpp'
    write(out, cpp)
    driver = '.driver.cpp'
    driver_code = """
#include <string>
#include <vector>
#include <iostream>

using namespace std;

namespace Parser{
    struct Value;
    const void * main(const std::string & filename);
}

int main(int argc, char ** argv){
    if (argc >= 2){
        const void * result = Parser::main(argv[1]);
        cout << "got " << (int) result << endl << endl;
        return (int) result;
    } else {
        cout << "Give an argument" << endl;
    }
}
"""
    write(driver_code, driver)

    exe = './.cpp-test'
    subprocess.call(["g++", "-g3", cpp, driver, "-o", exe])
    subprocess.call([exe, input])

    # erase(driver)
    # erase(cpp)
    return True

def test_all(name, grammar, input):
    grammar_file = '.grammar'
    input_file = '.input'
    
    write(grammar, grammar_file)
    write(input, input_file)

    do_bnf(name, grammar_file)
    do_python(name, grammar_file, input_file)
    do_cpp(name, grammar_file, input_file)

    erase(grammar_file)
    erase(input_file)

def test_cpp(name, grammar, input):
    grammar_file = newFile()
    input_file = newFile()
    
    write(grammar, grammar_file)
    write(input, input_file)

    do_cpp(name, grammar_file, input_file)

    erase(grammar_file)
    erase(input_file)

def test1():
    grammar = """
start-symbol: start
rules:
    start = "a"* "b"* "\\n"* <eof>
"""
    input = """aaaaaaabbbbbb"""

    test_all('test1', grammar, input)

def test2():
    grammar = """
start-symbol: start
rules:
    start = "a"* &"b" "b"+ "\\n"* <eof>
"""
    input = """aaaaaaabbbbbb"""

    test_all('test2', grammar, input)

def test3():
    grammar = """
start-symbol: start
include: {{
#include <iostream>
static void got_a(){
    std::cout << "Got an 'aa'!" << std::endl;
}
}}
rules:
    start = a* b "\\n"* <eof>
    a = "aa" {{
        got_a();
    }}
    b = "b"
"""
    input = """aaaab"""

    test_cpp('test3', grammar, input)
    
import sys
# add rootPath to sys path

def test4():
    grammar = """
start-symbol: start
code: {{
static Value add(const Value & a, const Value & b){
    return Value((void*)((int) a.getValue() + (int) b.getValue()));
}

static Value sub(const Value & a, const Value & b){
    return Value((void*)((int) a.getValue() - (int) b.getValue()));
}

static Value multiply(const Value & a, const Value & b){
    return Value((void*)((int) a.getValue() * (int) b.getValue()));
}

static Value divide(const Value & a, const Value & b){
    return Value((void*)((int) a.getValue() / (int) b.getValue()));
}

}}

rules:
        start = expression sw <eof> {{ value = $1; }}
        expression = expression2 expression1_rest($1)
        expression1_rest(a) = "+" expression2 e:{{value = add(a,$2);}} expression1_rest(e)
                            | "-" expression2 e:{{value = sub(a,$2);}} expression1_rest(e)
                            | <void> {{ value = a; }}

        expression2 = expression3 expression2_rest($1)
        expression2_rest(a) = "*" expression3 e:{{value = multiply(a,$2);}} expression2_rest(e)
                            | "/" expression3 e:{{value = divide(a,$2);}} expression2_rest(e)
                            | <void> {{ value = a; }}

        expression3 = number
                    | "(" expression ")" {{ value = $2; }}

        inline number = digit+ {{
            int total = 0;
            for (std::vector<Value>::const_iterator it = $1.getValues().begin(); it != $1.getValues().end(); it++){
                const Value & v = *it;
                char letter = (char) (int) v.getValue();
                total = (total * 10) + letter - '0';
            }
            value = (void*) total;
        }}
        inline sw = "\\n"*
        inline digit = [0123456789]
"""

    input = """1+(3-2)*9/(2+2*32)-3232342+91"""
    test_cpp('test4', grammar, input)

def test5():
    grammar = """
start-symbol: start
rules:
    start = a:a b:b {{value = a;}}
    a = "a"
    b = "b"
"""

    input = "ab"

    test_all('test5', grammar, input)

tests = [test1, test2, test3, test4, test5]
import sys
if len(sys.argv) > 1:
    num = int(sys.argv[1]) - 1
    try:
        tests[num]()
    except TestException as t:
        print t
else:
    for test in tests:
        try:
            test()
        except TestException as t:
            print t
