#ifndef _load_exception_h
#define _load_exception_h

#include "exception.h"

/* Generic load exception class. Thrown whenever a structure is being created
 * and an error occurs.
 */

#include <string>

/* FIXME: put this in the Exception namespace */
class LoadException: public Exception::Base {
public:
    LoadException(const std::string & file, int line, const std::string & reason);
    LoadException(const std::string & file, int line, const Exception::Base & nested, const std::string & reason);
    LoadException(const LoadException & copy);

    virtual ~LoadException() throw();

    virtual void throwSelf() const {
        throw *this;
    }
    
    virtual Exception::Base * copy() const;

protected:
    virtual const std::string getReason() const;

    std::string reason;
};

#endif
