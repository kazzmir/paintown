#ifndef _paintown_parser_all_h
#define _paintown_parser_all_h

#include <string>
#include <exception>

/* this is bad.. peg.py should just produce a .h file we can include */
namespace Mugen{
    namespace Def{
        extern const void * parse(const std::string & filename, bool stats = false);
        extern const void * parse(const char * in, int length, bool stats = false);

        class ParseException: public std::exception {
        public:
            std::string getReason() const;
            int getLine() const;
            int getColumn() const;
            virtual ~ParseException() throw();
        };
    }

    namespace Air{
        extern const void * parse(const std::string & filename, bool stats = false);
        extern const void * parse(const char * in, int length, bool stats = false);

        class ParseException: public std::exception {
        public:
            std::string getReason() const;
            int getLine() const;
            int getColumn() const;
            virtual ~ParseException() throw();
        };
    }
    
    namespace Cmd{
        extern const void * parse(const std::string & filename, bool stats = false);
        extern const void * parse(const char * data, bool stats = false);
        extern const void * parse(const char * in, int length, bool stats = false);

        class ParseException: public std::exception {
        public:
            std::string getReason() const;
            int getLine() const;
            int getColumn() const;
            virtual ~ParseException() throw();
        };
    }
}

#endif
