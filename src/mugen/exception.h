#ifndef mugen_exception_h
#define mugen_exception_h

#include "exceptions/exception.h"
#include <string>

class MugenException: public Exception::Base {
public:
	MugenException();
	MugenException(const std::string & reason, const std::string & where = "?", int line = 0);

	virtual ~MugenException() throw();
    
        virtual Base * copy() const;

        const std::string getFullReason() const;

	inline const std::string getReason() const{
            return reason;
	}

protected:
        std::string reason;
        std::string where;
        int line;
};

#endif
