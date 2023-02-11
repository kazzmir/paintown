#ifndef _rtech1_file_system_h
#define _rtech1_file_system_h

#include "exceptions/exception.h"
#include "pointer.h"
#include "thread.h"
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>
#include <stdint.h>

class Token;
struct stat;
/* path utilities */
namespace Path{
    class Path{
    public:
        const std::string & path() const;
        const std::string getExtension() const;
        bool isEmpty() const;
        
        virtual ~Path();

    protected:
        Path();
        Path(const std::string & path);
        Path(const Path & path);

        virtual inline void setPath(const std::string & s){
            mypath = s;
        }

        std::string mypath;
    };

    class InsensitivePath: public Path {
    public:
        InsensitivePath(const Path & what);
        bool operator==(const Path & path) const;
    };

    /* relative path should not have the leading data directory on it, just
     * the path within the r-tech system.
     */
    class RelativePath: public Path {
    public:
        explicit RelativePath();
        explicit RelativePath(const std::string & path);
        RelativePath(const RelativePath & path);

        bool operator<(const RelativePath & path) const;
        
        virtual RelativePath getDirectory() const;
        virtual RelativePath getFilename() const;

        RelativePath removeFirstDirectory() const;
        RelativePath firstDirectory() const;

        /* true if there are no directory parts to this path
         * foo is a file
         * bar/foo is not a file
         */
        bool isFile() const;

        /* a/ + b/ = a/b/ */
        RelativePath join(const RelativePath & path) const;
        RelativePath & operator=(const RelativePath & copy);

        bool operator==(const RelativePath & path) const;
        bool operator!=(const RelativePath & path) const;
    };

    /* absolute paths should have the entire filesystem path on it */
    class AbsolutePath: public Path {
    public:
        explicit AbsolutePath();
        explicit AbsolutePath(const std::string & path);
        AbsolutePath(const AbsolutePath & path);
        AbsolutePath & operator=(const AbsolutePath & copy);

        bool operator<(const AbsolutePath & path) const;
        bool operator==(const AbsolutePath & path) const;
        bool operator!=(const AbsolutePath & path) const;
        
        /* Remove a given path from the start of this path */
        virtual RelativePath remove(const AbsolutePath & path) const;

        virtual AbsolutePath getDirectory() const;
        virtual AbsolutePath getFilename() const;

        AbsolutePath removeFirstDirectory() const;
        std::string firstDirectory() const;
        bool isFile() const;

        /* If the filename is empty then get the name of the directory.
         * a/b -> b
         * a/b/ -> b
         *
         * Otherwise (a/b/).getFilename() will be ""
         */
        virtual std::string getLastComponent() const;
        
        AbsolutePath join(const RelativePath & path) const;
    };

    std::string invertSlashes(std::string str);
    std::string sanitize(std::string path);

    /* remove extension. foo.txt -> foo */
    std::string removeExtension(const std::string & str);

    /* Replace the extension on `input' with `extension'. Don't put the . on the extension,
     * so call it as replaceExtension(path, "foo")
     * instead of replaceExtension(path, ".foo")
     */
    AbsolutePath replaceExtension(const AbsolutePath & input, const std::string & extension);

    /* basename, just get the filename and remove the directory part */
    std::string stripDir(const std::string & str);
    
    /* dirname, just get the directory and remove the filename part */
    std::string stripFilename(const std::string & str);
}

namespace Storage{
    /* sorry for the crappy abbreviation, but can't collide with the
     * Exception class here
     */
    namespace Exc = ::Exception;

    class Exception: public Exc::Base {
    public:
        Exception(const std::string & where, int line, const std::string & file);
        Exception(const std::string & where, int line, const Exc::Base & nested, const std::string & file);
        Exception(const Exception & copy);
        virtual ~Exception() throw ();
        virtual void throwSelf() const {
            throw *this;
        }

    protected:
        virtual const std::string getReason() const;

        virtual Exc::Base * copy() const {
            return new Exception(*this);
        }

    private:
        std::string reason;
    };

    class NotFound: public Exception {
    public:
        NotFound(const std::string & where, int line, const std::string & file);
        NotFound(const std::string & where, int line, const Exc::Base & nested, const std::string & file);
        virtual ~NotFound() throw();
        NotFound(const NotFound & copy);
        virtual void throwSelf() const {
            throw *this;
        }
    protected:
        virtual Exc::Base * copy() const {
            return new NotFound(*this);
        }
    };

    class IllegalPath: public Exception {
    public:
        IllegalPath(const std::string & where, int line, const std::string & file);
        IllegalPath(const std::string & where, int line, const Exc::Base & nested, const std::string & file);
        virtual ~IllegalPath() throw();
        IllegalPath(const IllegalPath & copy);
        virtual void throwSelf() const {
            throw *this;
        }
    protected:
        virtual Exc::Base * copy() const {
            return new IllegalPath(*this);
        }
    };

    class Eof: public std::exception {
        public:
            Eof(){
            }

            virtual ~Eof() throw (){
            }
    };

    /* Abstraction for files. Should be used instead of FILE, ifstream, SDL_RWOps, anything else */
    class File{
    public:
        enum Access{
            Read,
            Write,
            ReadWrite
        };

        File();
        virtual ~File();

        /* Returns the number of bytes read */
        virtual int readLine(char * output, int size) = 0;

        /* Mostly for normal files, reset their error flags so
         * we can seek after eof
         */
        virtual void reset() = 0;

        /* true if the underlying object can be streamed. generally
         * this should be true if the object can be kept around while other files
         * of the same type can be opened.
         * zip files aren't streamable because only one zip entry can be
         * open at a time so it must be closed as soon as possible.
         */
        virtual bool canStream() = 0;

        virtual long getModificationTime() = 0;

        /* seek to an absolute position */
        virtual off_t seek(off_t position, int whence) = 0;

        virtual int getSize() = 0;

        virtual long tell() = 0;

        /* Returns a token that represents the path. For normal files it will just be
         * a single string containing the path -- "data/x/y/z.txt"
         * For zip files it will be (container "x/y/z.zip" "mount/point" "a.txt")
         */
        virtual Token * location() = 0;

        /* if the file is at eof and can't read anymore */
        virtual bool eof() = 0;

        /* if the file can still be read */
        virtual bool good() = 0;

        /* read one unsigned byte */
        virtual File & operator>>(unsigned char &) = 0;
    };
    
    class EndianReader{
        public:
            EndianReader(const Util::ReferenceCount<Storage::File> & file){
                internal = new FileInternal(file);
            }
            
            EndianReader(std::ifstream & stream){
                internal = new StreamInternal(stream);
            }

            class Internal{
            public:
                Internal();
                virtual bool eof() = 0;
                virtual int read(char * data, int length) = 0;
                virtual void seekEnd(std::streamoff where) = 0;
                virtual void seek(std::streamoff where) = 0;
                virtual int tell() = 0;
                virtual ~Internal();
            };

            class StreamInternal: public Internal {
            public:
                StreamInternal(std::ifstream & stream):
                stream(stream){
                }
                
                virtual bool eof();
                virtual int read(char * data, int length);
                virtual void seekEnd(std::streamoff where);
                virtual void seek(std::streamoff where);
                virtual int tell();
                virtual ~StreamInternal();

                /* Yes, use a reference here */
                std::ifstream & stream;
            };

            class FileInternal: public Internal {
            public:
                FileInternal(const Util::ReferenceCount<Storage::File> & file):
                file(file){
                }

                virtual bool eof();
                virtual void seekEnd(std::streamoff where);
                virtual void seek(std::streamoff where);
                virtual int read(char * data, int length);
                virtual int tell();
                virtual ~FileInternal();

                Util::ReferenceCount<Storage::File> file;
            };

            virtual ~EndianReader(){
            }

            virtual int8_t readByte1(){
                return convert(readBytes(sizeof(int8_t)));
            }

            virtual int16_t readByte2(){
                return convert(readBytes(sizeof(int16_t)));
            }

            virtual int32_t readByte4(){
                return convert(readBytes(sizeof(int32_t)));
            }

            virtual std::string readStringX(int length);
            virtual std::string readString2(int length);
            virtual void readBytes(uint8_t * out, int length);

            virtual void seekEnd(std::streamoff where);
            virtual void seek(std::streampos where);

            virtual int position();

        protected:
            virtual int32_t convert(const std::vector<uint8_t> & bytes) = 0;

            std::vector<uint8_t> readBytes(int length);
                
            Util::ReferenceCount<Internal> internal;
    };

    /* combines bytes b0 b1 b2 b3 as b0 + b1*2^8 + b2*2^16 + b3*2^24 */
    class LittleEndianReader: public EndianReader {
        public:
            LittleEndianReader(const Util::ReferenceCount<Storage::File> & file):
            EndianReader(file){
            }

            LittleEndianReader(std::ifstream & stream):
            EndianReader(stream){
            }

        protected:
            virtual int32_t convert(const std::vector<uint8_t> & bytes){
                uint32_t out = 0;
                for (std::vector<uint8_t>::const_reverse_iterator it = bytes.rbegin(); it != bytes.rend(); it++){
                    out = (out << 8) + *it;
                }
                return out;
            }
    };

    /* combines bytes b0 b1 b2 b3 as b0*2^24 + b1*2^16 + b2*2^8 + b3 */
    class BigEndianReader: public EndianReader {
        public:
            BigEndianReader(const Util::ReferenceCount<Storage::File> & file):
            EndianReader(file){
            }
            
            BigEndianReader(std::ifstream & stream):
            EndianReader(stream){
            }

        protected:
            virtual int32_t convert(const std::vector<uint8_t> & bytes){
                uint32_t out = 0;
                for (std::vector<uint8_t>::const_iterator it = bytes.begin(); it != bytes.end(); it++){
                    out = (out << 8) + *it;
                }
                return out;
            }
    };

    class LzmaContainer;
    class ZipContainer;
   
    class StringFile: public File {
    public:
        StringFile(const std::string & start);
        virtual int readLine(char * output, int size);
        virtual bool eof();
        virtual bool good();
        virtual int getSize();
        virtual long getModificationTime();
        virtual bool canStream();
        virtual void reset();
        virtual long tell();
        virtual Token * location();
        virtual off_t seek(off_t position, int whence);
        virtual File & operator>>(unsigned char &);
        virtual ~StringFile();

    protected:
        std::string data;
        std::istringstream stream;
    };

    /* Contains information about an abstract File. */
    class Descriptor{
    public:
        Descriptor();

        virtual Util::ReferenceCount<File> open(File::Access mode) = 0;

        virtual ~Descriptor();
    };

    class Traverser;
    class Directory{
    public:
        Directory();
        virtual ~Directory();
    
        /* Finds any path in the given directory that matches find, files and directories */
        std::vector<Path::AbsolutePath> findFiles(const Path::AbsolutePath & dataPath, const std::string & find, bool caseInsensitive);
        /* Only finds directories */
        std::vector<Path::AbsolutePath> findDirectories(const Path::AbsolutePath & dataPath, const std::string & find, bool caseInsensitive);

        void addFile(const Path::AbsolutePath & path, const Util::ReferenceCount<Descriptor> & file);
        void removeFile(const Path::AbsolutePath & path);

        /* FIXME: maybe filenames() should just return files? */
        /* Files + directories */
        std::vector<std::string> filenames() const;
        /* Just directories */
        std::vector<std::string> directoryNames() const;

        /* Finds a file with the given path. Will not find directories.
         * Might return NULL if the path can't be found
         */
        Util::ReferenceCount<Descriptor> lookup(const Path::AbsolutePath & path);

        /* true if the path (either file or directory) exists */
        bool exists(const Path::AbsolutePath & path);

        /* true if path is a directory */
        bool isDirectory(const Path::AbsolutePath & path);

    protected:
        void doTraverse(const Path::AbsolutePath & path, Traverser & traverser);
        void traverse(const Path::AbsolutePath & path, Traverser & traverser);

        // Util::ReferenceCount<File> doLookup(const Path::AbsolutePath & path);

        std::map<std::string, Util::ReferenceCount<Directory> > directories;
        std::map<std::string, Util::ReferenceCount<Descriptor> > files;

        Util::Thread::LockObject lock;
    };

    class System{
    public:
        System();
        virtual ~System();

        typedef Path::AbsolutePath AbsolutePath;
        typedef Path::RelativePath RelativePath;

        virtual AbsolutePath find(const RelativePath & path) = 0;
        virtual RelativePath cleanse(const AbsolutePath & path) = 0;
        virtual bool exists(const RelativePath & path) = 0;
        virtual bool exists(const AbsolutePath & path);
        virtual bool isDirectory(const AbsolutePath & path);
        virtual std::vector<AbsolutePath> getFilesRecursive(const AbsolutePath & dataPath, const std::string & find, bool caseInsensitive = false) = 0;

        /* Finds all supported container files. Currently
         *  .zip
         */
        virtual std::vector<AbsolutePath> getContainerFilesRecursive(const AbsolutePath & dataPath);
        /* Gets container files only in the specified directory */
        virtual std::vector<AbsolutePath> getContainerFiles(const AbsolutePath & dataPath);
        
        /* Gets container files in <user>/path ./path and <data>/path */
        virtual std::vector<AbsolutePath> getContainerFiles(const RelativePath & path);
        
        /* Given a path with no extension, find a container file that is <name>.zip or
         * <name>.7z or whatever exists.
         * So if dataPath is "mugen/foo" this function will look for
         *  mugen/foo.zip
         *  mugen/foo.7z
         *  mugen/foo.rar
         *  ...
         *
         * and return the first one it finds
         */
        virtual AbsolutePath findContainer(const RelativePath & dataPath);

        /* search for a pattern of a single file within a directory */
        virtual std::vector<AbsolutePath> getFiles(const AbsolutePath & dataPath, const std::string & find, bool caseInsensitive = false) = 0;

        /* Container should be a path to a zip file */
        virtual void addOverlay(const AbsolutePath & container, const AbsolutePath & where);
        virtual void removeOverlay(const AbsolutePath & container, const AbsolutePath & where);
        virtual std::vector<std::string> containerFileList(const AbsolutePath & container);

        /* search for some path which may contain wildcards in a directory */
        virtual std::vector<AbsolutePath> getFiles(const AbsolutePath & dataPath, const RelativePath & find, bool caseInsensitive);
        
        /* Gets all the files in all directories matched by the relative path */
        virtual std::vector<AbsolutePath> getFiles(const RelativePath & dataPath, const RelativePath & find, bool caseInsensitive) = 0;

        virtual AbsolutePath configFile() = 0;
        virtual AbsolutePath userDirectory() = 0;
        virtual std::vector<AbsolutePath> findDirectories(const RelativePath & path) = 0;
        virtual AbsolutePath findInsensitive(const RelativePath & path) = 0;
        virtual AbsolutePath lookupInsensitive(const AbsolutePath & directory, const RelativePath & path) = 0;

        virtual Util::ReferenceCount<File> open(const AbsolutePath & path, File::Access mode = File::Read);

    /* Should be protected but needs to be public so we can use it in template methods
     * in the implementation file.
     */
    public:
        virtual void overlayFile(const AbsolutePath & where, Util::ReferenceCount<ZipContainer> zip);
        virtual void overlayFile(const AbsolutePath & where, Util::ReferenceCount<LzmaContainer> container);

    /* package: */
        virtual bool systemExists(const AbsolutePath & path) = 0;
        virtual bool systemIsDirectory(const AbsolutePath & path) = 0;

        virtual void unoverlayFile(const AbsolutePath & where);

        // std::map<AbsolutePath, Util::ReferenceCount<ZipContainer> > overlays;
        Storage::Directory virtualDirectory;
    };

    System & instance();
    bool hasInstance();
    System & setInstance(const Util::ReferenceCount<System> & what);
    bool isContainer(const Path::AbsolutePath & path);

    std::string readFile(const Path::AbsolutePath & path);

    std::vector<std::string> containerTypes();
}

/*
 * class Filesystem
 * class NetworkStorage
 * class ZipStorage
 */
class Filesystem: public Storage::System {
public:
    Filesystem(const Path::AbsolutePath & dataPath);

    typedef Path::AbsolutePath AbsolutePath;
    typedef Path::RelativePath RelativePath;
    typedef Path::InsensitivePath InsensitivePath;
    typedef Storage::Exception Exception;
    typedef Storage::NotFound NotFound;

    /* given a relative path like sounds/arrow.png, prepend the proper
     * data path to it to give data/sounds/arrow.png
     */
    AbsolutePath find(const RelativePath & path);

    /* like `find' but ignores case */
    AbsolutePath findInsensitive(const RelativePath & path);

    /* findInsensitive but starts in the given absolute directory path */
    AbsolutePath lookupInsensitive(const AbsolutePath & directory, const RelativePath & path);

    // void initialize();

    /* whether the file exists at all */
    using System::exists;
    bool exists(const RelativePath & path);

    /* remove the data path from a string
     * data/sounds/arrow.png -> sounds/arrow.png
     */
    RelativePath cleanse(const AbsolutePath & path);

    /* returns all the directories starting with the given path.
     * will look in the main data directory, the user directory, and
     * the current working directory.
     */
    std::vector<AbsolutePath> findDirectories(const RelativePath & path);

    /* user specific directory to hold persistent data */
    AbsolutePath userDirectory();

    /* user specific path to store the configuration file */
    AbsolutePath configFile();

    using Storage::System::getFiles;
    /* search a directory for some files matching pattern `find' */
    virtual std::vector<AbsolutePath> getFiles(const AbsolutePath & dataPath, const std::string & find, bool caseInsensitive = false);
    virtual std::vector<AbsolutePath> getFiles(const RelativePath & dataPath, const RelativePath & find, bool caseInsensitive);

    /* same as getFiles but search directories recursively */
    std::vector<AbsolutePath> getFilesRecursive(const AbsolutePath & dataPath, const std::string & find, bool caseInsensitive = false);

protected:
    virtual bool systemExists(const AbsolutePath & path);
    virtual bool systemIsDirectory(const AbsolutePath & path);
    AbsolutePath lookup(const RelativePath path);
    std::vector<AbsolutePath> findDirectoriesIn(const AbsolutePath & path);
    std::vector<AbsolutePath> getAllDirectories(const AbsolutePath & path);

protected:
    Util::Thread::LockObject lock;
    AbsolutePath dataPath;
};

#endif
