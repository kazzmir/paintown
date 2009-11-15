#ifndef _paintown_parser_all_h
#define _paintown_parser_all_h

#include <string>

/* this is bad.. peg.py should just produce a .h file we can include */
namespace Mugen{
    namespace Def{
        extern const void * main(const std::string & filename);
    }
}

#endif
