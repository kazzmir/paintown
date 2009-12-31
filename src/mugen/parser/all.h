#ifndef _paintown_parser_all_h
#define _paintown_parser_all_h

#include <string>
#include <exception>

/* this is bad.. peg.py should just produce a .h file we can include */
namespace Mugen{
    namespace Def{
        extern const void * main(const std::string & filename, bool stats = false);
    }

    namespace Air{
        extern const void * main(const std::string & filename, bool stats = false);
    }
    
    namespace Cmd{
        extern const void * main(const std::string & filename, bool stats = false);

        class ParseException: std::exception {
        public:
            std::string getReason() const;
            virtual ~ParseException() throw();
        };
    }
}

#endif
