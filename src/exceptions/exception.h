#ifndef _paintown_exception_12345_h
#define _paintown_exception_12345_h

#include <string>
#include <exception>

namespace Exception{

class Base: public std::exception {
public:
    Base(const std::string & file, int line);
    Base(const std::string & file, int line, const Base & nested);

    virtual ~Base() throw ();
protected:
    Base(const Base & copy);
    Base(const std::string & file, int line, Base * nested);

    virtual Base * copy() const;

    std::string file;
    int line;
    Base * nested;
};

}

#endif
