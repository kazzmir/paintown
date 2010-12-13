#ifndef _paintown_loading_h
#define _paintown_loading_h

#include "util/thread.h"

namespace Level{
    class LevelInfo;
}

namespace Loader{

/* Kind of loader to show */
enum Kind{
    Default,
    SimpleCircle
};

class LoadingContext{
public:
    LoadingContext();
    virtual ~LoadingContext();
    virtual void doLoad();
    virtual void load() = 0;
    virtual bool done();
    static int load_it(void * arg);
protected:
    Util::Thread::Lock lock;
    bool finished;
};

extern volatile bool done_loading;
extern Util::Thread::Lock loading_screen_mutex;

void startLoading(Util::Thread::Id * thread, void * arg = 0, Kind kind = Default);
void stopLoading(Util::Thread::Id thread);

void loadScreen(LoadingContext & context, const Level::LevelInfo & info);

void * loadingScreen(void *);

}

#endif
