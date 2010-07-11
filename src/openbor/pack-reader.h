#ifndef _paintown_openbor_packreader_h
#define _paintown_openbor_packreader_h

#include <string>
#include <map>
#include <stdint.h>
#include "exceptions/exception.h"
#include "util/file-system.h"

namespace Bor{

/* TODO: make these extend exceptions/exception.h */
class PackError: public Exception::Base {
public:
    PackError(const std::string & file, int line, const std::string & reason);
    virtual ~PackError() throw ();
protected:
    virtual const std::string getReason() const;
    Exception::Base * copy() const;

    std::string reason;
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

    PackReader(const Filesystem::AbsolutePath & path);

private:
    /* Little endian magic number */
    static const uint32_t MAGIC = 0x4B434150;
    Filesystem::AbsolutePath filename;
    std::map<std::string, File> files;
};

}

#endif
