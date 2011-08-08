#ifndef mugen_converter_generator
#define mugen_converter_generator

#include "tools.h"

#include <vector>

namespace Mugen{
    
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
        PythonClass(const Content &);
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
    
/*! Reads in the characters definition file and prepares the files for conversion */
class CharacterGenerator{
    public:
        //! Pass in definition file
        CharacterGenerator(const std::string &);
        virtual ~CharacterGenerator();
        
        //! Pass in python output filename
        void output(const std::string &);
    
        //! Filename of the character definition file
        const std::string filename;
        
        //! Directory of character definition file
        const std::string directory;
        
        //! Command File
        std::string commandFile;
        
        //! Add a state file
        void addStateFile(const std::string &);
        
        //! State files
        std::vector<std::string> stateFiles;
        
        //! Air file
        std::string airFile;
        
    protected:    
        //! Handle base definition file
        void handleBaseDef(PythonClass &);
        
        //! Handle command file
        void handleCmdFile(PythonClass &);
        
        //! Handle state files
        void handleStateFiles(PythonClass &);
};
    
}

#endif