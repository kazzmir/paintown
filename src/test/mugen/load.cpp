#include <iostream>
#include "r-tech1/init.h"
#include "r-tech1/thread.h"
#include "r-tech1/message-queue.h"
#include "r-tech1/file-system.h"
#include "mugen/character.h"
#include "mugen/exception.h"
#include "mugen/parse-cache.h"
#include "r-tech1/timedifference.h"
#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/debug.h"

/*
#include <sstream>
#include <unistd.h>
*/

using namespace std;

/*
static int getPid(){
    return getpid();
}

static void runPmap(int pid){
    ostringstream out;
    out << "pmap " << pid;
    system(out.str().c_str());
}

static void showMemory(){
    runPmap(getPid());
}
*/

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

static void mount(const char * path){
    Filesystem::AbsolutePath container(path);
    Filesystem::AbsolutePath where(container.getDirectory());
    Storage::instance().addOverlay(container, where);
}

static int load(const char * path){
    // showMemory();
    for (int i = 0; i < 1; i++){
        try{
            TimeDifference diff;
            diff.startTime();
            Global::debug(0) << "Loading " << path << endl;
            Mugen::Character kfm(Storage::instance().find(Filesystem::RelativePath(path)), 0);
            kfm.load();
            diff.endTime();
            Global::debug(0, "test") << diff.printTime("Success! Took") << endl;
            /*
               int * x = new int[1 << 21];
               delete[] x;
               */
        } catch (const MugenException & e){
            Global::debug(0, "test") << "Test failure!: " << e.getReason() << endl;
            return 1;
        } catch (const Filesystem::NotFound & e){
            Global::debug(0, "test") << "Test failure! Couldn't find a file: " << e.getTrace() << endl;
            return 1;
        }
    }
    return 0;
    // showMemory();
}

static void loadCmd(const char* path){
    Filesystem::AbsolutePath full(path);
    AstRef parsed(Mugen::Util::parseCmd(full));
    Token* token = parsed->serialize();
    DebugLog << "Parsed output: " << token->toString() << endl;
    delete token;
}

int main(int argc, char ** argv){
    Global::InitConditions conditions;
    conditions.graphics = Global::InitConditions::Disabled;
    Global::init(conditions);

    Global::setDebug(1);
    Mugen::ParseCache cache;
    Util::Thread::initializeLock(&MessageQueue::messageLock);

    /*
    for (int i = 0; i < 1; i++){
        // DebugLog1 << "Load " << i << endl;
        loadCmd("x.cns");
    }
    return 0;
    */

    int die = 0;
    if (argc < 2){
        die = load("mugen/chars/kfm/kfm.def");
    } else {
        if (argc > 2){
            mount(argv[1]);
            die = load(argv[2]);
        } else {
            die = load(argv[1]);
        }
    }

    // for (int i = 0; i < 3; i++){
      // }
    return die;
}
