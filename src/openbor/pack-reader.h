#ifndef _paintown_openbor_packreader_h
#define _paintown_openbor_packreader_h

#include <string>
#include <exception>
#include <map>

namespace Bor{

/* TODO: make these extend exceptions/exception.h */
class Eof: public std::exception {
public:
    Eof(){
    }

    virtual ~Eof() throw (){
    }
};

class PackError: public std::exception {
public:
    PackError(const std::string & reason){
    }

    virtual ~PackError() throw (){
    }
};

/* Reads Bor/Openbor packfiles */
class PackReader{
public:
    struct File{
        File(uint32_t start, uint32_t length):
            start(start),
            length(length){
            }

        File():
            start(0),
            length(0){
            }

        uint32_t start;
        uint32_t length;
    };

    PackReader(const std::string & filename);

private:
    /* Little endian magic number */
    static const uint32_t MAGIC = 0x4B434150;
    std::string filename;
    std::map<std::string, File> files;
};

}

#endif
