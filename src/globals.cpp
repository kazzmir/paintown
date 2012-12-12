#include "util/graphics/bitmap.h"
#include "globals.h"
#include <streambuf>
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/message-queue.h"
#include "util/thread.h"
#include <iostream>
#include <sstream>

using namespace std;

static int major_version = 3;
static int minor_version = 6;
static int micro_version = 1;

/*
const int MIN_WORLD_Z = 160;
const int MAX_WORLD_Z = 232;
*/

const Filesystem::RelativePath Global::DEFAULT_FONT = Filesystem::RelativePath("fonts/arial.ttf");

namespace Global{
    int do_shutdown = 0;
}

bool Global::shutdown(){
    return do_shutdown > 0;
}

int Global::getVersion(int major, int minor, int micro){
    return major * 1000 + minor * 100 + micro;
}

int Global::getVersion(){
    return getVersion(major_version, minor_version, micro_version);
}

string Global::getVersionString(){
    ostringstream str;
    str << major_version << "." << minor_version << "." << micro_version;
    return str.str();
}

void Global::showTitleScreen(){
    Graphics::Bitmap s(*Storage::instance().open(titleScreen()));
    s.BlitToScreen();
}

const Filesystem::AbsolutePath Global::titleScreen(){
    return Storage::instance().find(Filesystem::RelativePath("menu/paintown.png"));
}

namespace Global{

/* just some random number I picked out of thin air */
const unsigned int MagicId = 0x0dff2110;

static vector<string> messageBuffer;
/* should support infinite queues eventually */
static MessageQueue * current = NULL;
void registerInfo(MessageQueue * queue){
    Util::Thread::acquireLock(&messageLock);
    current = queue;
    if (current != NULL){
        /* push all saved messages immediately to the queue */
        for (vector<string>::iterator it = messageBuffer.begin(); it != messageBuffer.end(); it++){
            current->add(*it);
        }
        messageBuffer.clear();
    }
    Util::Thread::releaseLock(&messageLock);
}

/* clear any buffered messages */
void clearInfo(){
    Util::Thread::acquireLock(&messageLock);
    messageBuffer.clear();
    Util::Thread::releaseLock(&messageLock);
}

void unregisterInfo(MessageQueue * queue){
    Util::Thread::acquireLock(&messageLock);
    if (current == queue){
        current = NULL;
    }
    Util::Thread::releaseLock(&messageLock);
}

void info(const std::string & str){
    Util::Thread::acquireLock(&messageLock);
    if (current != NULL){
        current->add(str);
    } else {
        /* save it for later until a message-queue is registered */
        messageBuffer.push_back(str);
    }
    Util::Thread::releaseLock(&messageLock);
}

int getScreenWidth(){
    return Graphics::Bitmap::getScreenWidth();
}

int getScreenHeight(){
    return Graphics::Bitmap::getScreenHeight();
}

}
