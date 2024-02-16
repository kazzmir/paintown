#include "r-tech1/token.h"
#include "r-tech1/tokenreader.h"
#include "r-tech1/file-system.h"
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <math.h>

using namespace std;


#ifndef WINDOWS

/* FIXME: dont put these methods in this test file */
Filesystem::AbsolutePath Filesystem::configFile(){
    std::ostringstream str;
    /* what if HOME isn't set? */
    str << getenv("HOME") << "/.paintownrc";
    return Filesystem::AbsolutePath(str.str());
}

Filesystem::AbsolutePath Filesystem::userDirectory(){
    std::ostringstream str;
    char * home = getenv("HOME");
    if (home == NULL){
        str << "/tmp/paintown";
    } else {
        str << home << "/.paintown/";
    }
    return Filesystem::AbsolutePath(str.str());
}

#endif


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
#ifndef WINDOWS
    static char temp[64];
    sprintf(temp, "/tmp/tokenXXXXXX");
    return string(mktemp(temp));
#else 
/*
    #include <iostream>
    #include <windows.h>
    char tempPath[MAX_PATH];
    char temp[MAX_PATH];
    sprintf(temp, "tokenXXXXXX");
    DWORD result = GetTempPathA(MAX_PATH, tempPath);
    return string(GetTempFileNameA(tempPath, "tokenXXXXXX", 0, temp));
*/
    return string();
#endif
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

static void test8(){
    Storage::instance().addOverlay(Storage::instance().find(Filesystem::RelativePath("src/test/token/test.zip")), Storage::instance().find(Filesystem::RelativePath("src/test/token")));
    Util::ReferenceCount<Storage::File> file = Storage::instance().open(Storage::instance().find(Filesystem::RelativePath("src/test/token/x.txt")));
    TokenReader reader;
    Token * head = reader.readTokenFromFile(*file);
    string words;
    head->match("foo/bar", words);
    if (words != "cheese"){
        throw Failure(8);
    }
}

static void test9(){

    Token* foo2 = Token::makeSExpression(Token::makeDatum("foo"),
            Token::makeSExpression(Token::makeDatum("bar"), Token::makeDatum("bag")));

    foo2->print("");
    delete foo2;

    const std::string sxpr = "(foo (bar (bag bad) (dad bod)))";

    TokenReader tr;
    Token * token = tr.readTokenFromString(sxpr);
    token->print("");
}

static void test10(){
    const char *test9 =
    "foo: \n"
    "    bar1: {\n"
    "       bag1: true \n"
    "    } \n"
    "    bag2: \n"
    "       foo: bar \n"
    "       foo: { \n"
    "           foo3: bar2,\n"
    "           bar3: bag3\n"
    "       }";
    TokenReader reader;
    Token * head = reader.readTokenFromString(test9);

    head->print(" ");

    vector<const Token*> tokens = head->findTokens("foo/bag2/foo");
    DebugLog << tokens.size() << endl;
    if (tokens.size() != 2){
        throw Failure(10);
    }
}

static void test11(){
    std::string path = "src/test/token/test.yaml";
    TokenReader reader;
    Token * head = reader.readTokenFromFile(path);

    head->print(" ");

    vector<const Token *> tokens = head->findTokens("system/menu/values");
    if (tokens.size() != 1){
        throw Failure(11);
    }
}

#ifndef WINDOWS
int main(){
#else
#include <SDL2/SDL.h>
int main(int argv, char *args[]){
#endif
    Global::setDebug(2);
    try{
        test1();
        test2();
        test3();
        test4();
        test5();
        test6();
        test7();
        test8();
        test9();
        test10();
        test11();
        cout << "All tests passed!" << endl;
        return 0;
    } catch (const Failure & f){
        cout << "Test case " << f.num << " failed" << endl;
        return 1;
    }
}
