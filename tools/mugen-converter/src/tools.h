#ifndef mugen_converter_tools
#define mugen_converter_tools

#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

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
std::string stripDir(const std::string &);

/* dirname, just get the directory and remove the filename part */
std::string stripFilename(const std::string &);

/* strips out the extension of the file */
std::string stripExtension(const std::string &);

/* Returns a string in lowercase */
const std::string lowercase(const std::string &);

/* Compare strings in lowercase */
bool match(const std::string &, const std::string &);

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
            this->indentAmount = 0;
        }
        
        inline void setIndentLevel(unsigned int level){
            this->indentAmount = level;
        }
        
        inline const unsigned int getIndentLevel() const{
            return this->indentAmount;
        }
        
        static StreamEnd endl;
        
    private:
        std::ofstream stream;
        std::stringstream stringHolder;
        unsigned int indentAmount;
};

/*! Content which houses indent level and line(s) */
class Content{
    public:
        Content();
        Content(unsigned int level, const std::string &);
        Content(const Content &);
        virtual ~Content();
        
        const Content & operator=(const Content &);
        void addLine(unsigned int level, const std::string &);
        void addSpace();
        void output(PythonStream &, unsigned int indentStart = 0);
        const unsigned int getIndentLevel() const;
    protected:
        unsigned int totalLines;
        std::vector<unsigned int> indentLevel;
        std::vector<std::string> content;
};

/*! Python member def's */
class PythonDefinition{
    public:
        PythonDefinition(const Content &);
        PythonDefinition(const PythonDefinition &);
        virtual ~PythonDefinition();
        const PythonDefinition & operator=(const PythonDefinition &);
        void addContent(const Content &);
        void addSpace();
        void output(PythonStream &, unsigned int indentStart = 0);
    protected:
        Content defLine;
        std::vector<Content> content;
};
    
/*! Python Class and it's members */
class PythonClass{
    public:
        PythonClass(const Content &, const Content &);
        PythonClass(const PythonClass &);
        virtual ~PythonClass();
        
        const PythonClass & operator=(const PythonClass &);
        
        void add(const PythonDefinition &);
        void output(PythonStream &, unsigned int indentStart = 0);
        
        inline PythonDefinition & getInit(){
            return this->init;
        }
        
    protected:
        Content classLine;
        PythonDefinition init;
        std::vector<PythonDefinition> definitions;
};

}

#endif
