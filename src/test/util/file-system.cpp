#include <iostream>
#include <string>
#include <fcntl.h>
#include <fstream>
#include <algorithm>
#include <vector>
#include "r-tech1/debug.h"
#include "r-tech1/file-system.h"
#include "r-tech1/system.h"

#ifdef SWITCH
#include <switch.h>
#endif


#ifndef WINDOWS

/* FIXME: dont put these methods in this test file */
Filesystem::AbsolutePath Filesystem::configFile(){
    std::ostringstream str;
    /* what if HOME isn't set? */
    str << getenv("HOME") << "/.paintownrc";
    return Filesystem::AbsolutePath(str.str());
}

Filesystem::AbsolutePath Filesystem::userDirectory(){
    std::ostringstream str;
    char * home = getenv("HOME");
    if (home == NULL){
        str << "/tmp/paintown";
    } else {
        str << home << "/.paintown/";
    }
    return Filesystem::AbsolutePath(str.str());
}

#endif

namespace Util{

static int upperCase(int c){
    return toupper(c);
}

/*
std::string upperCaseAll(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), upperCase);
    return str;
}

Filesystem::AbsolutePath getDataPath2(){
    return Filesystem::AbsolutePath(".");
}
*/
}

// Do stuff for switch testing
class ConsoleWrapper{
public:
    ConsoleWrapper():
    count(0),
    done(false){
#ifdef SWITCH
        consoleInit(NULL);
        // Configure our supported input layout: a single player with standard controller styles
        padConfigureInput(1, HidNpadStyleSet_NpadStandard);
        padInitializeDefault(&pad);
#endif
    }
    ~ConsoleWrapper(){
#ifdef SWITCH
        consoleExit(NULL);
#endif
    }
    bool doLoop(){
#ifdef SWITCH
        // Updade stuff
        appletMainLoop();
        // Scan the gamepad. This should be done once for each frame
        padUpdate(&pad);

        // padGetButtonsDown returns the set of buttons that have been
        // newly pressed in this frame compared to the previous one
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_Plus)
            done = true; // break in order to return to hbmenu

        // Keep going
        return !done;
#else
        if (count++ >= 1){
            done = true;
        }
        return !done;
#endif
    }
    void update(){
#if SWITCH
        consoleUpdate(NULL);
#endif
    }
protected:
#ifdef SWITCH
    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
#endif
    int count;
    bool done;
};

std::string getArchivesPath(){
#ifdef DEVKITPRO
    return "/apps/paintown/";
#else
    return "src/test/util/";
#endif
}


// Use other directories
std::string getDataPath(){
    std::vector<std::string> locations = { "paintown-data",
                                           "data-new",
                                           "data-other",
                                           "/apps/paintown/data",
                                         };
    for (std::vector<std::string>::iterator it = locations.begin(); it != locations.end(); it++){
        if (System::isDirectory(*it)){
            return *it;
        }
    }
    return "";
}

class ExitException : std::exception {
public:
    ExitException(){}
    ~ExitException(){}
};


void testGetFiles(){
    DebugLog << "Testing get files. Setting data path to data." << std::endl;
    Filesystem::AbsolutePath dataPath = Filesystem::AbsolutePath(getDataPath());
    DebugLog << "Setting relative path m*/*.txt" << std::endl;
    Filesystem::RelativePath relativePath = Filesystem::RelativePath("m*/*.txt");
    std::vector<Filesystem::AbsolutePath> paths = Storage::instance().getFiles(dataPath, relativePath, false);
    DebugLog << "Got total matched files: " << paths.size() << std::endl;
    for (std::vector<Filesystem::AbsolutePath>::iterator it = paths.begin(); it != paths.end(); it++){
        DebugLog << it->path() << std::endl;
    }
}

void testZip(){
    try{
        // open("/", O_RDONLY, 0);
        Storage::instance().addOverlay(Filesystem::AbsolutePath(getArchivesPath() + "test.zip"), Filesystem::AbsolutePath("test"));
        Util::ReferenceCount<Storage::File> file = Storage::instance().open(Filesystem::AbsolutePath("test/SConstruct"));
        char line[1024];
        int read = file->readLine(line, sizeof(line) - 1);
        line[read] = '\0';
        std::cout << "Read '" << std::string(line) << "'" << std::endl;
    } catch (const Storage::Exception & fail){
        Global::debug(0) << "Zip file error: " << fail.getTrace() << std::endl;
        throw ExitException();
    }
}

static void testLastComponent(){
    Filesystem::AbsolutePath foo("a/b/c/");
    Global::debug(0) << "Last component of " << foo.path() << " is " << foo.getLastComponent() << std::endl;
    if (foo.getLastComponent() != "c"){
        Global::debug(0) << "Last component failed" << std::endl;
        throw ExitException();
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
        throw ExitException();
    }
    if (directory.lookup(Filesystem::AbsolutePath("a/../a/./b")) != test1){
        Global::debug(0) << "[2] Directory test failed" << std::endl;
        throw ExitException();
    }
    if (directory.lookup(Filesystem::AbsolutePath("b/a")) != test2){
        Global::debug(0) << "[3] Directory test failed" << std::endl;
        throw ExitException();
    }
}

static void test7z(){
    Storage::instance().addOverlay(Filesystem::AbsolutePath(getArchivesPath() + "test.7z"), Filesystem::AbsolutePath("test"));
}
#ifndef CROSS_BUILD
int main(){
#else
#include <SDL2/SDL.h>
int main(int argv, char *args[]){
#endif
    Global::setDebug(2);
    ConsoleWrapper cw;

    try {
        testGetFiles();
        testZip();
        testLastComponent();
        testDirectory();
        test7z();
    } catch (const ExitException & ex){
#ifndef DEVKITPRO
        exit(0);
#endif
    }
    while(cw.doLoop()){
        cw.update();
    }

    return 0;
}
