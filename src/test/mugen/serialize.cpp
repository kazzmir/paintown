#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "util/debug.h"
#include <stdlib.h>
#include "util/file-system.h"
#include "util/timedifference.h"
#include "util/bitmap.h"
#include "util/token_exception.h"
#include "mugen/ast/all.h"
#include "mugen/parser/all.h"
#include "mugen/ast/extra.h"
#include "mugen/parse-cache.h"

using namespace std;

namespace Util{

string trim(const std::string & str){
    string s;
    size_t startpos = str.find_first_not_of(" \t");
    size_t endpos = str.find_last_not_of(" \t");
    // if all spaces or empty return an empty string  
    if ((string::npos == startpos) ||
        (string::npos == endpos)){
        return "";
    } else {
        return str.substr(startpos, endpos-startpos+1);
    }
    return str;
}

static int upperCase(int c){
    return toupper(c);
}

string upperCaseAll(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), upperCase);
    return str;
}

Filesystem::AbsolutePath getDataPath2(){
    return Filesystem::AbsolutePath("/");
}

}

static string randomFile(){
    static char temp[64];
    sprintf(temp, "/tmp/serialXXXXXX");
    close(mkstemp(temp));
    return string(temp);
}

typedef const void * (*parser)(const string & path, bool what);

static void timeit(const char * path, parser what){
    Mugen::ParseCache cache;
    what(path, false); // ignore first time
    for (int count = 1; count < 1000; count *= 10){
        TimeDifference diff;
        diff.startTime();
        for (int i = 0; i < count; i++){
            what(path, false);
        }
        diff.endTime();
        Global::debug(0) << diff.printTime("total time") << std::endl;
        Global::debug(0) << diff.getTime() / (double) count / (1000) << " ms per copy" << std::endl;
    }
}

static void load(const char * path, parser what){
    try{
        /* parse file */
        TimeDifference diff;
        diff.startTime();
        Ast::AstParse parsed((list<Ast::Section*>*) what(string(path), false));
        diff.endTime();
        Global::debug(0, "test") << diff.printTime("parse") << endl;
        diff.startTime();

        /* serialize and write it out to a file */
        Token * serial = parsed.serialize();
        diff.endTime();
        Global::debug(0, "test") << diff.printTime("serialize") << endl;
        string file = randomFile();
        ofstream out(file.c_str());
        // out << setprecision(6) << fixed;
        serial->toStringCompact(out);
        out << endl;
        out.close();
        TokenReader reader;
        diff.startTime();

        /* read file and deserialize input */
        Token * deserial = reader.readTokenFromFile(file.c_str());
        diff.endTime();
        Global::debug(0, "test") << diff.printTime("token") << endl;

        diff.startTime();
        Ast::AstParse deparsed(deserial);
        diff.endTime();
        Global::debug(0, "test") << diff.printTime("deserialize") << endl;

        /* compare original parse with deserial parse */
        diff.startTime();
        if (parsed != deparsed){
            diff.endTime();
            Global::debug(0, "test") << diff.printTime("equality") << endl;
            Global::debug(0) << "Fail!" << endl;
        } else {
            diff.endTime();
            Global::debug(0, "test") << diff.printTime("equality") << endl;
            Global::debug(0) << "Pass!" << endl;
        }

        /* cleanup */
        delete serial;
        remove(file.c_str());

        // timeit(path, what);
    } catch (const Filesystem::NotFound & e){
        Global::debug(0, "test") << "Test failure! Couldn't find a file: " << e.getTrace() << endl;
    } catch (const Mugen::Cmd::ParseException & e){
        Global::debug(0, "test") << "Test failure! Parse exception: " << e.getReason() << endl;
    } catch (const Ast::Exception & e){
        Global::debug(0, "test") << "Test failure! Ast exception: " << e.getTrace() << endl;
    } catch (const TokenException & e){
        Global::debug(0, "test") << "Test failure! Token exception: " << e.getTrace() << endl;
    }
}

bool isCmd(const string & str){
    return str.find(".cmd") != string::npos ||
           str.find(".cns") != string::npos;
}

bool isDef(const string & str){
    return str.find(".def") != string::npos;
}

bool isAir(const string & str){
    return str.find(".air") != string::npos;
}

int main(int argc, char ** argv){
    if (argc < 2){
        load("src/test/mugen/serialize-test.cns", Mugen::Cmd::parse);
    } else {
        if (isCmd(argv[1])){
            load(argv[1], Mugen::Cmd::parse);
        } else if (isDef(argv[1])){
            load(argv[1], Mugen::Def::parse);
        } else if (isAir(argv[1])){
            load(argv[1], Mugen::Air::parse);
        } else {
            Global::debug(0) << "Don't know how to parse " << argv[1] << endl;
        }
    }

    return 0;
}
