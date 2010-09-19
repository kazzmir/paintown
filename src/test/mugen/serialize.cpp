#include <string>
#include <iostream>
#include "debug.h"
#include "util/file-system.h"
#include "util/timedifference.h"
#include "util/bitmap.h"
#include "mugen/ast/all.h"
#include "mugen/parser/all.h"
#include "mugen/ast/extra.h"

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

}

static void load(const char * path){
    try{
        TimeDifference diff;
        Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Cmd::parse(string(path)));
        // Global::debug(0, "test") << diff.printTime("Success! Took") << endl;
    } catch (const Filesystem::NotFound & e){
        Global::debug(0, "test") << "Test failure! Couldn't find a file: " << e.getTrace() << endl;
    } catch (const Mugen::Cmd::ParseException & e){
        Global::debug(0, "test") << "Test failure! Parse exception: " << e.getReason() << endl;
    }
}

int main(int argc, char ** argv){
    if (argc < 2){
        load("src/test/mugen/serialize-test.cns");
    } else {
        load(argv[1]);
    }
}
