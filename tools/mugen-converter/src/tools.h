#ifndef mugen_converter_tools
#define mugen_converter_tools

#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

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

/* basename, just get the filename and remove the directory part */
std::string stripDir(const std::string & str);

/* dirname, just get the directory and remove the filename part */
std::string stripFilename(const std::string & str);

/* strips out the extension of the file */
std::string stripExtension(const std::string & str);

/* equivalent of std::endl */
class StreamEnd{
private:
    friend class PythonStream;
    StreamEnd(){
    }
};

class PythonStream{
    public: 
        enum IndentType{
            indent,
            unindent,
        };
        
        PythonStream();
        ~PythonStream();
        
        template <typename T> PythonStream & operator<<(const T & var){
            stringHolder << var;
            return *this;
        }
        
        const PythonStream & operator<<(const IndentType &);
        
        // endl
        const PythonStream & operator<<(const StreamEnd &);
        
        void open(const std::string &);
        void close();
        void clear();
        
        inline void clearIndents(){
            indentAmount = 0;
        }
        
        static StreamEnd endl;
        
    private:
        std::ofstream stream;
        std::stringstream stringHolder;
        unsigned indentAmount;
};

}

#endif
