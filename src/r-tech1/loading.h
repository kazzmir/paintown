#ifndef _rtech1_loading_h
#define _rtech1_loading_h

#include <string>
#include "thread.h"
#include "file-system.h"

namespace Graphics{
    class Bitmap;
}

namespace Loader{

class Info{
public:

    Info(const std::string & message, const Filesystem::AbsolutePath & background);
    Info(const Info & info);

    virtual ~Info();
    
    virtual void setBackground(const Graphics::Bitmap * background);
    virtual void setLoadingMessage(const std::string & str);
    virtual void addAuthorInfo(const std::vector<std::string> &);
    virtual void setPosition(int x, int y);

    virtual const Graphics::Bitmap * getBackground() const;

    virtual const std::string & loadingMessage() const;
    virtual const std::vector<std::string> & authorInfo() const;
    virtual const Filesystem::AbsolutePath & loadingBackground() const;

    virtual int getPositionX() const;
    virtual int getPositionY() const;

protected:
    int x, y;
    std::string _loadingMessage;
    std::vector<std::string> _authorInfo;
    const Graphics::Bitmap * background;
    Filesystem::AbsolutePath _loadingBackground;
};

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
    static void * load_it(void * arg);
protected:
    Util::Thread::Lock lock;
    bool finished;
};

extern volatile bool done_loading;
// extern Util::Thread::Lock loading_screen_mutex;

/* deprecated, remove */
void startLoadingX(Util::Thread::Id * thread, void * arg = 0, Kind kind = Default);
void stopLoadingX(Util::Thread::Id thread);

void loadScreen(LoadingContext & context, const Info & info, Kind kind = Default);

void * loadingScreen(void *);

}

#endif
