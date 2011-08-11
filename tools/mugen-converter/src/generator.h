#ifndef mugen_converter_generator
#define mugen_converter_generator

#include "tools.h"

#include <vector>

namespace Mugen{
  
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