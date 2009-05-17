#ifndef _paintown_return_exception_h
#define _paintown_return_exception_h

#include <exception>

class ReturnException: public std::exception {
public:
	ReturnException():exception(){}

	virtual ~ReturnException() throw(){}
};

#endif
