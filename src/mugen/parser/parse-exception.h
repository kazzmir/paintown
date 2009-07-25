#ifndef _paintown_mugen_parse_exception_h
#define _paintown_mugen_parse_exception_h

#include <exception>
#include <string>

namespace Mugen{

class ParserException: public std::exception{
public:
    ParserException(const std::string & reason);
    virtual inline const std::string & getReason() const {
        return reason;
    }

    virtual ~ParserException() throw ();
protected:
    const std::string reason;
};

}

#endif
