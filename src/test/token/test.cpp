#include "../../util/token.h"
#include "../../util/tokenreader.h"
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <math.h>

using namespace std;

class Failure: public exception{
public:
    Failure(int num):
    num(num){
    }

    int num;
};

static void test1_write(const string & name){
    ofstream out(name.c_str());
    out << "(foo)";
    out.close();
}

static string randomFile(){
    static char temp[64];
    sprintf(temp, "/tmp/tokenXXXXXX");
    return string(mktemp(temp));
}

static void test1(){
    try{
        string file = randomFile();
        test1_write(file);
        TokenReader reader;
        reader.readTokenFromFile(file);
    } catch (...){
        throw Failure(1);
    }
}

static void test2_write(string name){
    ofstream out(name.c_str());
    out << "(foo";
    out.close();
}

static void test2(){
    try{
        string file = randomFile();
        test2_write(file);
        TokenReader reader;
        reader.readTokenFromFile(file);
        throw Failure(2);
    } catch (const TokenException & e){
        /* good */
    }
}

static void test3_write(string file){
    ofstream out(file.c_str());
    out << "(foo1 (foo2 (foo3)) (foo2 (foo3)))";
    out.close();
}

static void test3(){
    string file = randomFile();
    test3_write(file);
    TokenReader reader;
    Token * head = reader.readTokenFromFile(file);
    vector<const Token*> tokens = head->findTokens("foo1/foo2/foo3");
    if (tokens.size() != 2){
        throw Failure(3);
    }
}

static void test4_write(string file){
    ofstream out(file.c_str());
    out << "(foo (bar cheese))";
    out.close();
}

static void test4(){
    string file = randomFile();
    test4_write(file);
    TokenReader reader;
    Token * head = reader.readTokenFromFile(file);
    string words;
    head->match("foo/bar", words);
    if (words != "cheese"){
        throw Failure(4);
    }
}

static void test5_write(string file){
    ofstream out(file.c_str());
    out << "(relative-position -.5 -.5)";
    out.close();
}

static void test5(){
    string file = randomFile();
    test5_write(file);
    TokenReader reader;
    Token * head = reader.readTokenFromFile(file);
    double n1 = 0, n2 = 0;
    double epsilon = 0.00000001;
    if (*head != "relative-position"){
        throw Failure(5);
    }
    head->view() >> n1 >> n2;
    if (fabs(n1 - (-0.5)) > epsilon){
        throw Failure(5);
    }
    if (fabs(n2 - (-0.5)) > epsilon){
        throw Failure(5);
    }
}

static void test6(){
    string data = "(foo1 (foo2 (foo3)) (foo2 (foo3)))";
    TokenReader reader;
    Token * head = reader.readTokenFromString(data);
    vector<const Token*> tokens = head->findTokens("foo1/foo2/foo3");
    if (tokens.size() != 2){
        throw Failure(6);
    }
}

static void test7_helper(string on, string off){
    std::ostringstream out;
    out << "(foo " << on << " " << off << ")";
    TokenReader reader;
    Token * head = reader.readTokenFromString(out.str());
    bool xtrue = false;
    bool xfalse = true;
    head->match("foo", xtrue, xfalse);
    if (!xtrue || xfalse){
        throw Failure(7);
    }
}

static void test7(){
    test7_helper("on", "off");
    test7_helper("1", "0");
    test7_helper("true", "false");
    test7_helper("enable", "disable");
}

int main(){
    try{
        test1();
        test2();
        test3();
        test4();
        test5();
        test6();
        test7();
        cout << "All tests passed!" << endl;
        return 0;
    } catch (const Failure & f){
        cout << "Test case " << f.num << " failed" << endl;
        return 1;
    }
}
