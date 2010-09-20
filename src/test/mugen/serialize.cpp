#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "debug.h"
#include <stdlib.h>
#include "util/file-system.h"
#include "util/timedifference.h"
#include "util/bitmap.h"
#include "util/token_exception.h"
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

static string randomFile(){
    static char temp[64];
    sprintf(temp, "/tmp/serialXXXXXX");
    close(mkstemp(temp));
    return string(temp);
}

static void load(const char * path){
    try{
        /* parse file */
        TimeDifference diff;
        diff.startTime();
        Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Cmd::parse(string(path)));
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

int main(int argc, char ** argv){
    if (argc < 2){
        load("src/test/mugen/serialize-test.cns");
    } else {
        load(argv[1]);
    }

    return 0;
}
