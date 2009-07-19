#ifndef _paintown_mugen_parser_h
#define _paintown_mugen_parser_h

#include <string>
#include <fstream>

namespace Mugen{

class Parser{
public:
    Parser();
    void parse(const std::string & path);
    void parse(std::ifstream & path);
    virtual ~Parser();
protected:
    void parseLine(const std::string & line);
};

}

#endif
