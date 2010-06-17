#ifndef _paintown_shutdown_exception_h
#define _paintown_shutdown_exception_h

#include <exception>

/* This exception is thrown when the user clicks the X button on window
 * or otherwise wants to immediately shutdown the program.
 * If you catch this exception to do something then you must throw it again.
 */
class ShutdownException: public std::exception {
public:
	ShutdownException():exception(){}

	virtual ~ShutdownException() throw(){}
};

#endif
