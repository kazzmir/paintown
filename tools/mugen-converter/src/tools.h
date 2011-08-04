#ifndef mugen_converter_tools
#define mugen_converter_tools

#include <list>
#include <string>

namespace Ast{
    class Section;
}

namespace Mugen{
    
namespace Def{
    struct Value;
    const void * parse(const std::string & filename, bool stats = false);

    class ParseException: std::exception {
    public:
        ParseException();
        virtual ~ParseException() throw();
        std::string getReason() const;
    };
}

namespace Cmd{
    struct Value;
    const void * parse(const std::string & filename, bool stats = false);

    class ParseException: std::exception {
    public:
        ParseException();
        virtual ~ParseException() throw();
        std::string getReason() const;
    };
}  

namespace Air{
    struct Value;
    const void * parse(const std::string & filename, bool stats = false);

    class ParseException: std::exception {
    public:
        ParseException();
        virtual ~ParseException() throw();
        std::string getReason() const;
    };
}

void destroy(std::list<Ast::Section*> *);

std::list<Ast::Section*> * parseAir(const std::string &);

std::list<Ast::Section*> * parseCmd(const std::string &);

std::list<Ast::Section*> * parseDef(const std::string &);

}

#endif
