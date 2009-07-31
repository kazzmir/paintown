#!/usr/bin/env python

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

def do_bnf(name, grammar):
    import subprocess
    print "[%s] Test bnf.." % name
    peg_out = subprocess.Popen(['./peg.py', '--bnf', grammar], stdout = subprocess.PIPE)
    out, err = peg_out.communicate()
    g2 = ".bnf2"
    write(out, g2)
    peg_out2 = subprocess.Popen(['./peg.py', '--bnf', g2], stdout = subprocess.PIPE)
    out2, err2 = peg_out2.communicate()
    erase(g2)
    if out != out2:
        print "error with bnf generation!!"
        print out
        print "vs"
        print out2
        return False
    return True

def do_python(name, grammar, input):
    import subprocess
    print "[%s] Test python.." % name
    peg_out = subprocess.Popen(['./peg.py', '--python', grammar], stdout = subprocess.PIPE)
    out, err = peg_out.communicate()
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
    peg_out = subprocess.Popen(['./peg.py', '--cpp', grammar], stdout = subprocess.PIPE)
    out, err = peg_out.communicate()
    cpp = '.test_cpp.cpp'
    write(out, cpp)
    driver = '.driver.cpp'
    driver_code = """
#include <string>
#include <vector>
#include <iostream>

using namespace std;

namespace xx{
    struct Value;
    const void * main(const std::string & filename);
}

int main(int argc, char ** argv){
    if (argc >= 2){
        const void * result = xx::main(argv[1]);
        cout << "got " << (int) result << endl << endl;
    } else {
        cout << "Give an argument" << endl;
    }
}
"""
    write(driver_code, driver)

    exe = './.cpp-test'
    subprocess.call(["g++", cpp, driver, "-o", exe])
    subprocess.call([exe, input])

    erase(driver)
    erase(cpp)
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

test1()
test2()
test3()
