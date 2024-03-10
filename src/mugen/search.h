#ifndef _paintown_mugen_search_h
#define _paintown_mugen_search_h

#include <vector>

#include "r-tech1/thread.h"
#include "r-tech1/file-system.h"

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

    bool stagesDone();

    /* either start searching or unpause */
    void start();
    /* pause searching */
    void pause();

    void subscribe(Subscriber * who);
    void unsubscribe(Subscriber * who);

    // std::vector<Filesystem::AbsolutePath> characters

protected:

    /* Saves the search state so it can be paused/resumed */
    class CharacterSearch{
    public:
        CharacterSearch(Searcher & owner);
        void start();
        void pause();
        virtual ~CharacterSearch();

        Searcher & owner;
        std::vector<Filesystem::AbsolutePath> paths;
        volatile bool searching;
        PaintownUtil::Thread::Id thread;
        /* Searching lock *must be* initialized before searchingCheck */
        PaintownUtil::Thread::LockObject searchingLock;
        PaintownUtil::ThreadBoolean searchingCheck;

        static void * runSearch(void * self_);
        void search();
    };

    class StageSearch{
    public:
        StageSearch(Searcher & owner);
        void start();
        void pause();
        /* True if there are no more paths to search */
        bool done();
        virtual ~StageSearch();

        Searcher & owner;
        PaintownUtil::Thread::Id thread;
        std::vector<Filesystem::AbsolutePath> paths;
        volatile bool searching;
        /* Searching lock *must be* initialized before searchingCheck */
        PaintownUtil::Thread::LockObject searchingLock;
        PaintownUtil::ThreadBoolean searchingCheck;
        bool isDone;

        static void * runSearch(void * self_);
        void search();
    };

    CharacterSearch characterSearch;
    StageSearch stageSearch;

    static void * searchForCharacters(void * arg);
    static void * searchStages(void * arg);
    void addCharacters(const std::vector<Filesystem::AbsolutePath> & files);
    void addStages(const std::vector<Filesystem::AbsolutePath> & files);
    bool existsSubscription(Subscriber * who);

    void publishCharacters(const std::vector<Filesystem::AbsolutePath> & files);
    void publishStages(const std::vector<Filesystem::AbsolutePath> & files);
        
    PaintownUtil::Thread::LockObject addCharactersLock;
    std::vector<Filesystem::AbsolutePath> characters;
    
    PaintownUtil::Thread::LockObject addStagesLock;
    std::vector<Filesystem::AbsolutePath> stages;

    PaintownUtil::Thread::LockObject subscriptionLock;
    std::vector<Subscriber*> subscriptions;
};

}

#endif
