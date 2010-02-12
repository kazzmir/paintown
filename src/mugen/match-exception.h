#ifndef _mugen_match_exception_h
#define _mugen_match_exception_h

#include <exception>

/*! This exception is thrown when a match has ended */
class MatchException: public std::exception {
public:
	MatchException():exception(){}

	virtual ~MatchException() throw(){}
};

#endif
