#include <iostream>
#include <string>
#include <fcntl.h>
#include <fstream>
#include <algorithm>
#include <vector>
#include "util/file-system.h"

namespace Util{

static int upperCase(int c){
    return toupper(c);
}

std::string upperCaseAll(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), upperCase);
    return str;
}

Filesystem::AbsolutePath getDataPath2(){
    return Filesystem::AbsolutePath(".");
}

}

void testGetFiles(){
    std::vector<Filesystem::AbsolutePath> paths = Storage::instance().getFiles(Filesystem::AbsolutePath("data"), Filesystem::RelativePath("m*/*.txt"), false);
    for (std::vector<Filesystem::AbsolutePath>::iterator it = paths.begin(); it != paths.end(); it++){
        std::cout << it->path() << std::endl;
    }
}

void testZip(){
    try{
        open("/", O_RDONLY, 0);
        Storage::instance().addOverlay(Filesystem::AbsolutePath("src/test/util/test.zip"), Filesystem::AbsolutePath("test"));
        Util::ReferenceCount<Storage::File> file = Storage::instance().open(Filesystem::AbsolutePath("test/SConstruct"));
        char line[1024];
        int read = file->readLine(line, sizeof(line) - 1);
        line[read] = '\0';
        std::cout << "Read '" << std::string(line) << "'" << std::endl;
    } catch (const Storage::Exception & fail){
        Global::debug(0) << "Zip file error: " << fail.getTrace() << std::endl;
        exit(1);
    }
}

static void testLastComponent(){
    Filesystem::AbsolutePath foo("a/b/c/");
    Global::debug(0) << "Last component of " << foo.path() << " is " << foo.getLastComponent() << std::endl;
    if (foo.getLastComponent() != "c"){
        Global::debug(0) << "Last component failed" << std::endl;
        exit(1);
    }
}

static void testDirectory(){
    Storage::Directory directory;
    class EmptyDescriptor: public Storage::Descriptor {
    public:
         virtual Util::ReferenceCount<Storage::File> open(Storage::File::Access mode){
             return Util::ReferenceCount<Storage::File>(NULL);
         }
    };

    Util::ReferenceCount<Storage::Descriptor> test1(new EmptyDescriptor());
    Util::ReferenceCount<Storage::Descriptor> test2(new EmptyDescriptor());
    directory.addFile(Filesystem::AbsolutePath("a/b"), test1);
    directory.addFile(Filesystem::AbsolutePath("b/a"), test2);
    if (directory.lookup(Filesystem::AbsolutePath("a/b")) != test1){
        Global::debug(0) << "[1] Directory test failed" << std::endl;
        exit(1);
    }
    if (directory.lookup(Filesystem::AbsolutePath("a/../a/./b")) != test1){
        Global::debug(0) << "[2] Directory test failed" << std::endl;
        exit(1);
    }
    if (directory.lookup(Filesystem::AbsolutePath("b/a")) != test2){
        Global::debug(0) << "[3] Directory test failed" << std::endl;
        exit(1);
    }
}

int main(){
    testGetFiles();
    testZip();
    testLastComponent();
    testDirectory();
}
