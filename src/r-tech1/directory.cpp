#include "r-tech1/file-system.h"
#include "r-tech1/debug.h"
#include "r-tech1/thread.h"
#include <string>
#include <vector>
#include <map>

#ifndef CROSS_BUILD
#include "libs/filesystem/fs-wrapper.h"
#include "libs/filesystem/glob.h"
#endif

using std::string;
using std::vector;
using std::map;

namespace Storage{

Directory::Directory(){
}

Directory::~Directory(){
}

/* Used to signal .. in a path */
class UpDirectory: public std::exception {
public:
    UpDirectory(Path::AbsolutePath path):
    path(path){
    }

    virtual ~UpDirectory() throw () {
    }

    Path::AbsolutePath path;
};

class Traverser{
public:
    Traverser(){
    }

    virtual void traverseFile(Directory & directory, const string & file) = 0;
    virtual void traverseDirectory(Directory & directory, const string & path) = 0;

    virtual ~Traverser(){
    }
};

/* FIXME: this shares a lot of code with findFiles */
vector<Path::AbsolutePath> Directory::findDirectories(const Path::AbsolutePath & dataPath, const std::string & find, bool caseInsensitive){
    Util::Thread::ScopedLock scoped(lock);
    vector<Path::AbsolutePath> out;

    class FindDirectory: public Traverser {
    public:
        FindDirectory():
        failed(false){
        }

        bool failed;
        Util::ReferenceCount<Directory> last;

        virtual void traverseFile(Directory & directory, const string & file){
            if (directory.directories[file] != NULL){
                last = directory.directories[file];
            }
        }

        virtual void traverseDirectory(Directory & directory, const string & path){
            if (directory.directories[path] != NULL){
                last = directory.directories[path];
            } else {
                failed = true;
            }
        }
    };

    FindDirectory lastDirectory;
    traverse(dataPath, lastDirectory);

    if (lastDirectory.failed || lastDirectory.last == NULL){
        return out;
    }

    Global::debug(1) << "Search in " << dataPath.path() << " for " << find << std::endl;
    vector<string> names = lastDirectory.last->directoryNames();
#ifndef CROSS_BUILD
    for (vector<string>::iterator it = names.begin(); it != names.end(); it++){
        Global::debug(1) << "Check if " << *it << " matches " << find << std::endl;
        //if (file_matches(it->c_str(), find.c_str())){
        
        for (const fs::directory_entry & dirEntry : fs::recursive_directory_iterator(fs::path(*it))){
            Global::debug(2) << "Looking for file: " << find << " in directory: " << dirEntry << std::endl;
            for (fs::path & globFile : glob::glob(dataPath.path() + "/" + dirEntry.path().filename().string())){
                // FIXME add search, instead of just globbing all files
                out.push_back(dataPath.join(Path::RelativePath(*it)));
            }
        }
        
        /*if (it->find(find) != std::string::npos) {
            out.push_back(dataPath.join(Path::RelativePath(*it)));
        }*/
    }
#endif

    return out;
}

vector<Path::AbsolutePath> Directory::findFiles(const Path::AbsolutePath & dataPath, const std::string & find, bool caseInsensitive){
    Util::Thread::ScopedLock scoped(lock);
    vector<Path::AbsolutePath> out;

    class FindDirectory: public Traverser {
    public:
        FindDirectory():
        failed(false){
        }

        bool failed;
        Util::ReferenceCount<Directory> last;

        virtual void traverseFile(Directory & directory, const string & file){
            if (directory.directories[file] != NULL){
                last = directory.directories[file];
            }
        }

        virtual void traverseDirectory(Directory & directory, const string & path){
            if (directory.directories[path] != NULL){
                last = directory.directories[path];
            } else {
                failed = true;
            }
        }
    };

    FindDirectory lastDirectory;
    traverse(dataPath, lastDirectory);

    if (lastDirectory.failed || lastDirectory.last == NULL){
        return out;
    }

    Global::debug(1) << "Search in " << dataPath.path() << " for " << find << std::endl;
    vector<string> names = lastDirectory.last->filenames();
#ifndef CROSS_BUILD
    for (vector<string>::iterator it = names.begin(); it != names.end(); it++){
        Global::debug(1) << "Check if " << *it << " matches " << find << std::endl;
        //if (file_matches(it->c_str(), find.c_str())){
        
        for (const fs::directory_entry & dirEntry : fs::recursive_directory_iterator(fs::path(*it))){
            Global::debug(2) << "Looking for file: " << find << " in directory: " << dirEntry << std::endl;
            for (fs::path & globFile : glob::glob(dataPath.path() + "/" + dirEntry.path().filename().string())){
                // FIXME add search, instead of just globbing all files
                out.push_back(dataPath.join(Path::RelativePath(*it)));
            }
        }

        /*if (it->find(find) != std::string::npos) {
            out.push_back(dataPath.join(Path::RelativePath(*it)));
        }*/
    }
#endif

    return out;
}
        
vector<std::string> Directory::directoryNames() const {
    Util::Thread::ScopedLock scoped(lock);
    vector<string> out;

    for (map<string, Util::ReferenceCount<Directory> >::const_iterator it = directories.begin(); it != directories.end(); it++){
        if (it->second != NULL){
            out.push_back(it->first);
        }
    }
    
    return out;
}

vector<string> Directory::filenames() const {
    Util::Thread::ScopedLock scoped(lock);
    vector<string> out;

    for (map<string, Util::ReferenceCount<Descriptor> >::const_iterator it = files.begin(); it != files.end(); it++){
        if (it->second != NULL){
            out.push_back(it->first);
        }
    }

    for (map<string, Util::ReferenceCount<Directory> >::const_iterator it = directories.begin(); it != directories.end(); it++){
        if (it->second != NULL){
            out.push_back(it->first);
        }
    }
    
    return out;
}

void Directory::doTraverse(const Path::AbsolutePath & path, Traverser & traverser){
    if (path.isFile()){
        traverser.traverseFile(*this, path.path());
    } else {
        string name = path.firstDirectory();
        if (name == "."){
            try{
                return doTraverse(path.removeFirstDirectory(), traverser);
            } catch (const UpDirectory & up){
                return doTraverse(up.path, traverser);
            }
        } else if (name == ".."){
            throw UpDirectory(path.removeFirstDirectory());
        }
        traverser.traverseDirectory(*this, path.firstDirectory());
        Util::ReferenceCount<Directory> directory = directories[path.firstDirectory()];
        if (directory != NULL){
            try{
                return directory->doTraverse(path.removeFirstDirectory(), traverser);
            } catch (const UpDirectory & up){
                return doTraverse(up.path, traverser);
            }
        }
    }
}

void Directory::traverse(const Path::AbsolutePath & path, Traverser & traverser){
    Path::AbsolutePath use = path;

    /* The path might contain .. paths that would go out of this directory
     * so just ignore those paths and use the rest of the path.
     */
    while (true){
        try{
            return doTraverse(use, traverser);
        } catch (const UpDirectory & up){
            use = up.path;
        }
    }
}
        
bool Directory::exists(const Path::AbsolutePath & path){
    Util::Thread::ScopedLock scoped(lock);
    class FindIt: public Traverser {
    public:
        FindIt():
        found(false){
        }

        virtual void traverseFile(Directory & directory, const string & file){
            if (directory.files[file] != NULL){
                found = true;
            } else if (directory.directories[file] != NULL){
                found = true;
            } else {
                found = false;
            }
        }

        virtual void traverseDirectory(Directory & directory, const string & path){
        }

        bool found;
    };

    FindIt find;
    traverse(path, find);
    return find.found;
}

bool Directory::isDirectory(const Path::AbsolutePath & path){
    Util::Thread::ScopedLock scoped(lock);
    class FindIt: public Traverser {
    public:
        FindIt():
        found(false){
        }

        virtual void traverseFile(Directory & directory, const string & file){
            if (directory.files[file] != NULL){
                found = false;
            } else if (directory.directories[file] != NULL){
                found = true;
            } else {
                found = false;
            }
        }

        virtual void traverseDirectory(Directory & directory, const string & path){
        }

        bool found;
    };

    FindIt find;
    traverse(path, find);
    return find.found;
}

/* Might return NULL if the path can't be found */
Util::ReferenceCount<Descriptor> Directory::lookup(const Path::AbsolutePath & path){
    Util::Thread::ScopedLock scoped(lock);
    class FindIt: public Traverser {
    public:

        virtual void traverseFile(Directory & directory, const string & file){
            found = directory.files[file];
        }

        virtual void traverseDirectory(Directory & directory, const string & path){
        }

        Util::ReferenceCount<Descriptor> found;
    };

    FindIt find;
    traverse(path, find);
    return find.found;
}

void Directory::addFile(const Path::AbsolutePath & path, const Util::ReferenceCount<Descriptor> & file){
    Util::Thread::ScopedLock scoped(lock);
    class AddPath: public Traverser {
    public:
        AddPath(const Util::ReferenceCount<Descriptor> & file):
        file(file){
        }

        const Util::ReferenceCount<Descriptor> & file;

        virtual void traverseFile(Directory & directory, const string & path){
            directory.files[path] = file;
        }

        virtual void traverseDirectory(Directory & directory, const string & path){
            if (directory.directories[path] == NULL){
                directory.directories[path] = Util::ReferenceCount<Directory>(new Directory());
            }
        }
    };

    Global::debug(1) << "Add file " << path.path() << std::endl;
    AddPath adder(file);
    traverse(path, adder);
}
        
void Directory::removeFile(const Path::AbsolutePath & path){
    addFile(path, Util::ReferenceCount<Descriptor>(NULL));
}

}
