#ifndef _paintown_menu_reload_exception_h
#define _paintown_menu_reload_exception_h

#include <exception>

/* This exception is thrown when a Mod is picked or changed */
class ReloadMenuException: public std::exception {
public:
	ReloadMenuException():exception(){}

	virtual ~ReloadMenuException() throw(){}
};

#endif
