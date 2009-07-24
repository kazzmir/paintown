#include "parser.h"
#include "globals.h"
#include <string>
#include <fstream>
#include "util/funcs.h"

#include "parsers.h"

using namespace std;

namespace Mugen{

class Section{
public:
    Section();
    virtual ~Section();
};

Parser::Parser(){
}

void Parser::parse(const string & path){
    Mugen::parseAir(path);
    /*
    ifstream file(path.c_str());
    parse(file);
    */
}

static void removeComments(string & str){
    size_t comment = str.find_first_of(';');
    if (comment != string::npos){
        str.erase(comment);
    }
}

static void removeAll(string & str, char c){
    size_t place = str.find(c);
    while (place != string::npos){
        str.erase(place, 1);
        place = str.find(c);
    }
}

static void removeNewlines(string & str){
    removeAll(str, '\n');
    removeAll(str, '\r');
}

static void removeLeadingWhitespace(string & str){
    while (str.find(' ') == 0 || str.find('\t') == 0){
        str.erase(0, 1);
    }
}

static void removeTrailingWhitespace(string & str){
    while (str.length() > 0 && (str[str.length() - 1] == ' ' || str[str.length() - 1] == '\t')){
        str.erase(str.length() - 1);
    }
}

static string sanitize(string dirty){
    removeComments(dirty);
    removeNewlines(dirty);
    removeLeadingWhitespace(dirty);
    removeTrailingWhitespace(dirty);
    return dirty;
}

void Parser::parseLine(const string & line){
    if (line.length() == 0){
        return;
    }
    Global::debug(1) << "Parse line: " << line << endl;
}

void Parser::parse(ifstream & file){
    bool done = file.eof();
    while (!done){
        char space[1<<14];
        file.getline(space, sizeof(space));
        parseLine(sanitize(space));
        done = file.eof();
    }
}
    
Parser::~Parser(){
}

}

extern "C" int parser_get_pipe(int files[2]){
    return Util::getPipe(files);
}
