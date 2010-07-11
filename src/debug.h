#ifndef _paintown_debug_h
#define _paintown_debug_h

#include <ostream>

namespace Global{

void setDebug( int i );
int getDebug();
std::ostream & debug(int i, const std::string & context = "paintown");

}

#endif
