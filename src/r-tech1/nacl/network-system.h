#ifndef _rtech1_network_system_h
#define _rtech1_network_system_h

#ifdef NACL

#include <map>
#include <string>
#include <vector>
#include "../file-system.h"
#include "../thread.h"
#include "../pointer.h"

namespace pp{
    class Instance;
    class Core;
}

namespace Nacl{

typedef Path::AbsolutePath AbsolutePath;
typedef Path::RelativePath RelativePath;
typedef Path::InsensitivePath InsensitivePath;
    
class Manager;
class OpenHandle;
class FileHandle;
class File;

class NetworkSystem: public Storage::System {
public:
    NetworkSystem(pp::Instance * instance, pp::Core * core);
    virtual ~NetworkSystem();

    virtual AbsolutePath find(const RelativePath & path);
    virtual RelativePath cleanse(const AbsolutePath & path);
    using System::exists;
    virtual bool exists(const RelativePath & path);
    virtual std::vector<AbsolutePath> getFilesRecursive(const AbsolutePath & dataPath, const std::string & find, bool caseInsensitive = false);
    using Storage::System::getFiles;
    virtual std::vector<AbsolutePath> getFiles(const AbsolutePath & dataPath, const std::string & find, bool caseInsensitive = false);
    virtual std::vector<AbsolutePath> getFiles(const RelativePath & path, const RelativePath & find, bool caseInsensitive);
    virtual AbsolutePath configFile();
    virtual AbsolutePath userDirectory();
    virtual std::vector<AbsolutePath> findDirectories(const RelativePath & path);
    virtual AbsolutePath findInsensitive(const RelativePath & path);
    virtual AbsolutePath lookupInsensitive(const AbsolutePath & directory, const RelativePath & path);

public:
    virtual int libcOpen(const char * path, int mode, int params);
    virtual ssize_t libcRead(int fd, void * buf, size_t count);
    virtual int libcClose(int fd);
    virtual off_t libcLseek(int fd, off_t offset, int whence);
    virtual int libcLstat(const char * path, struct stat * buf);
    virtual int libcAccess(const char *filename, int mode);

protected:
    virtual bool systemExists(const AbsolutePath & path);
    /*
    string readFileAsString(const AbsolutePath & path);
    Directory parseDirectory(const AbsolutePath & path);
    */
    unsigned int fileSize(Manager & manager, const Filesystem::AbsolutePath & path);
    using System::open;
    Util::ReferenceCount<FileHandle> open(const Util::ReferenceCount<OpenHandle> & handle);

protected:
    pp::Instance * instance;
    pp::Core * core;
    /* only one thread at a time to access the network system */
    Util::Thread::LockObject fileLock;
    Util::Thread::LockObject existsLock;
    // Util::ReferenceCount<Manager> manager;
    std::map<AbsolutePath, bool> existsCache;

    /* map a filesystem path to an object that holds the data for the file */
    std::map<std::string, Util::ReferenceCount<OpenHandle> > files;

    /* map a file descriptor number to an object that stores information for
     * reading from the file
     */
    std::map<int, Util::ReferenceCount<File> > fileTable;

};

}

#endif

#endif
