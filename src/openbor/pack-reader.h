#ifndef _paintown_openbor_packreader_h
#define _paintown_openbor_packreader_h

#include <string>
#include <map>
#include <vector>
#include <stdint.h>
#include <fstream>
#include "util/exceptions/exception.h"
#include "util/file-system.h"
#include "util/thread.h"

namespace Bor{

class PackError: public Exception::Base {
public:
    PackError(const std::string & file, int line, const std::string & reason);
    virtual ~PackError() throw ();
protected:
    virtual const std::string getReason() const;
    Exception::Base * copy() const;

    std::string reason;
};

/* Reads Bor/Openbor packfiles
 * TODO: extend the util/reader.h interface
 */
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

    char * readFile(const File & file);

    std::vector<std::string> findPaths(const std::string & search);

    const std::map<std::string, File> & getFiles() const {
        return files;
    }

    const File & getFile(const std::string & path);
    uint32_t getFileLength(const std::string & path);

    virtual ~PackReader();

private:
    /* Little endian magic number */
    static const uint32_t MAGIC = 0x4B434150;
    Filesystem::AbsolutePath filename;
    std::map<std::string, File> files;
    std::ifstream handle;

    /* locked when reading data */
    Util::Thread::Lock readLock;
};

}

#endif
