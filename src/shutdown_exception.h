#ifndef _paintown_shutdown_exception_h
#define _paintown_shutdown_exception_h

#include <exception>

class ShutdownException: public std::exception {
public:
	ShutdownException():exception(){}

	virtual ~ShutdownException() throw(){}
};

#endif
