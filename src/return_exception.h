#ifndef _paintown_return_exception_h
#define _paintown_return_exception_h

#include <exception>

using namespace std;

class ReturnException: public exception{
public:
	ReturnException():exception(){}

	virtual ~ReturnException() throw(){}
};

#endif
