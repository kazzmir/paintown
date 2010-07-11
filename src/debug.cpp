#include "debug.h"
#include <iostream>

using namespace std;

static int global_debug_level = 0;

class nullstreambuf_t: public std::streambuf {
public:
    nullstreambuf_t():std::streambuf(){
    }
};

static nullstreambuf_t nullstreambuf;

class nullcout_t: public std::ostream {
public:
    nullcout_t():std::ostream(&nullstreambuf){
    }
};

static nullcout_t nullcout;

ostream & Global::debug(int i, const string & context){
    if ( global_debug_level >= i ){
        std::cout << "[" << i << ":" << context << "] ";
        return std::cout;
    }
    return nullcout;
}

void Global::setDebug( int i ){
    global_debug_level = i;
}

int Global::getDebug(){
    return global_debug_level;
}
