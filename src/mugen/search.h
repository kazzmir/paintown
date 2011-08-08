#ifndef _paintown_mugen_search_h
#define _paintown_mugen_search_h

#include <vector>

#include "util/thread.h"
#include "util/file-system.h"

namespace PaintownUtil = ::Util;

/* searches for mugen stuff (characters, stages, etc) in the background */

namespace Mugen{

class Searcher{
public:
    Searcher();
    virtual ~Searcher();

    /* gets updates from the searcher */
    class Subscriber{
    public:
        Subscriber();
        virtual ~Subscriber();

        /* receive data from the searcher */
        virtual void receiveCharacters(const std::vector<Filesystem::AbsolutePath> & paths) = 0;

        virtual void receiveStages(const std::vector<Filesystem::AbsolutePath> & paths) = 0;
    };

    /* either start searching or unpause */
    void start();
    /* pause searching */
    void pause();

    void subscribe(Subscriber * who);
    void unsubscribe(Subscriber * who);

    // std::vector<Filesystem::AbsolutePath> characters

protected:
    static void * searchForCharacters(void * arg);
    static void * searchStages(void * arg);
    void addCharacters(const std::vector<Filesystem::AbsolutePath> & files);
    void addStages(const std::vector<Filesystem::AbsolutePath> & files);
    bool existsSubscription(Subscriber * who);

    void publishCharacters(const std::vector<Filesystem::AbsolutePath> & files);
    void publishStages(const std::vector<Filesystem::AbsolutePath> & files);
        
    PaintownUtil::Thread::Id characterSearchThread;
    PaintownUtil::Thread::Id stageSearchThread;

    PaintownUtil::Thread::LockObject searchingLock;
    volatile bool quitSearching;
    PaintownUtil::ThreadBoolean searchingCheck;

    PaintownUtil::Thread::LockObject addCharactersLock;
    std::vector<Filesystem::AbsolutePath> characters;
    
    PaintownUtil::Thread::LockObject addStagesLock;
    std::vector<Filesystem::AbsolutePath> stages;

    PaintownUtil::Thread::LockObject subscriptionLock;
    std::vector<Subscriber*> subscriptions;
};

}

#endif
