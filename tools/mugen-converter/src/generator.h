#ifndef mugen_converter_generator
#define mugen_converter_generator

#include "tools.h"

namespace Mugen{
    
/*! Reads in the characters definition file and prepares the files for conversion */
class CharacterGenerator{
    public:
        //! Pass in definition file
        CharacterGenerator(const std::string &);
        virtual ~CharacterGenerator();
        
        //! Pass in python output filename
        void output(const std::string &);
    protected:
        std::string filename;
};
    
}

#endif