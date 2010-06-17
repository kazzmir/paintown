#ifndef _paintown_exception_12345_h
#define _paintown_exception_12345_h

#include <string>
#include <exception>

namespace Exception{

/* Base class for all exceptions */
class Base: public std::exception {
public:
    Base(const std::string & file, int line);
    Base(const std::string & file, int line, const Base & nested);

    virtual ~Base() throw ();
protected:
    Base(const Base & copy);

    virtual Base * copy() const;

    std::string file;
    int line;
    Base * nested;
};


/* This exception is thrown when the user wants to return to the previous menu or
 * whatever from some menu or the game by through some abnormal means (like
 * pressing ESC). If there is an "exit" button in the menu then usually you shouldn't
 * throw this exception, just return as normal.
 */
class Return: public Base {
public:
    Return(const std::string & file, int line);
    Return(const std::string & file, int line, const Base & nested);
    virtual ~Return() throw();
protected:
    virtual Base * copy() const;
};

}

#endif
