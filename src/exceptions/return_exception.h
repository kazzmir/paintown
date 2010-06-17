#ifndef _paintown_return_exception_h
#define _paintown_return_exception_h

#include "exception.h"

#if 0
#include <exception>

/* This exception is thrown when the user wants to return to the previous menu or
 * whatever from some menu or the game by through some abnormal means (like
 * pressing ESC). If there is an "exit" button in the menu then usually you shouldn't
 * throw this exception, just return as normal.
 */
class ReturnException: public std::exception {
public:
	ReturnException():exception(){}

	virtual ~ReturnException() throw(){}
};
#endif

#endif
