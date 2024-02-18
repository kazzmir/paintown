#ifndef _rtech1_tokenreader_h
#define _rtech1_tokenreader_h

#include <string>
#include <vector>
#include "token_exception.h"

class Token;

namespace Storage{
    class File;
}

class TokenReader{
public:
    TokenReader();

    virtual Token * readToken(const std::string & path);
    virtual Token * readToken(const char * path);
    virtual Token * readTokenFromString(const std::string & stuff);
    virtual Token * readTokenFromFile(const std::string & path);
    virtual Token * readTokenFromFile(Storage::File & file);

    virtual ~TokenReader();

protected:
    /* deprecated */
    TokenReader( const std::string & s );
    TokenReader( const char * filename );
    virtual Token * readToken();

    virtual void readTokens(Storage::File & stream);
    virtual void readTokensFromYaml(Storage::File & file);
    /*
    std::ifstream ifile;
    std::string myfile;
    */
    std::vector<Token *> my_tokens;
};

#endif
