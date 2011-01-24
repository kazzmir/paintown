#include "ast/all.h"
#include <list>
#include <string>

using namespace std;

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
}

void destroy(std::list<Ast::Section*> * list){
    for (std::list<Ast::Section*>::iterator it = list->begin(); it != list->end(); it++){
        delete (*it);
    }
    delete list;
}

list<Ast::Section*> * parseAir(const string & file){
    return (std::list<Ast::Section*>*) Mugen::Air::parse(file);
}

list<Ast::Section*> * parseCmd(const string & file){
    return (std::list<Ast::Section*>*) Mugen::Cmd::parse(file);
}

list<Ast::Section*> * parseDef(const string & file){
    return (std::list<Ast::Section*>*) Mugen::Def::parse(file);
}

void exampleParseCmd(const string & file){
    try{
        class Walker: public Ast::Walker {
        public:
            Walker(){
            }

            virtual void onSection(const Ast::Section & section){
                cout << "Got a section" << endl;
            }
        };

        Walker walker;
        list<Ast::Section*> * sections = parseCmd(file);
        for (list<Ast::Section*>::iterator it = sections->begin(); it != sections->end(); it++){
            Ast::Section * section = *it;
            section->walk(walker);
        }
        destroy(sections);
        
    } catch (const Mugen::Cmd::ParseException & fail){
        cout << "Failed to parse " << file << " because " << fail.getReason() << endl;
    }
}

int main(int argc, char ** argv){
    if (argc > 1){
        exampleParseCmd(argv[1]);
    }
}
