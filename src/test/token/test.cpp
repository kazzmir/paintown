#include "../../util/token.h"
#include "../../util/tokenreader.h"
#include <exception>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>

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
        TokenReader reader(file);
        reader.readToken();
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
        TokenReader reader(file);
        reader.readToken();
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
    TokenReader reader(file);
    Token * head = reader.readToken();
    vector<Token*> tokens = head->findTokens("foo1/foo2/foo3");
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
    TokenReader reader(file);
    Token * head = reader.readToken();
    string words;
    head->match("foo/bar", words);
    if (words != "cheese"){
        throw Failure(4);
    }
}

int main(){
    try{
        test1();
        test2();
        test3();
        test4();
        cout << "All tests passed!" << endl;
    } catch (const Failure & f){
        cout << "Test case " << f.num << " failed" << endl;
    }
}
