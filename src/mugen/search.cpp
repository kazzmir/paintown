#include "util/file-system.h"
#include "search.h"
#include "config.h"
#include "util/debug.h"

#include <algorithm>
#include <iostream>
#include <vector>

using std::endl;
using std::vector;

namespace Mugen{
        
Searcher::Subscriber::Subscriber(){
}

Searcher::Subscriber::~Subscriber(){
}

Searcher::Searcher():
characterSearchThread(PaintownUtil::Thread::uninitializedValue),
stageSearchThread(PaintownUtil::Thread::uninitializedValue),
quitSearching(false),
searchingCheck(quitSearching, searchingLock.getLock()){
}

Searcher::~Searcher(){
    pause();
}
    
void Searcher::start(){
    searchingCheck.set(false);
    if (characterSearchThread == PaintownUtil::Thread::uninitializedValue){
        if (!PaintownUtil::Thread::createThread(&characterSearchThread, NULL, (PaintownUtil::Thread::ThreadFunction) searchForCharacters, this)){
            Global::debug(0) << "Could not create character search thread" << endl;
        }
    }

    if (stageSearchThread == PaintownUtil::Thread::uninitializedValue){
        if (!PaintownUtil::Thread::createThread(&stageSearchThread, NULL, (PaintownUtil::Thread::ThreadFunction) searchStages, this)){
            Global::debug(0) << "Could not create stage search thread" << endl;
        }
    }
}
    
void Searcher::pause(){
    searchingCheck.set(true);
    if (characterSearchThread != PaintownUtil::Thread::uninitializedValue){
        PaintownUtil::Thread::joinThread(characterSearchThread);
        characterSearchThread = PaintownUtil::Thread::uninitializedValue;
    }
    
    if (stageSearchThread != PaintownUtil::Thread::uninitializedValue){
        PaintownUtil::Thread::joinThread(stageSearchThread);
        stageSearchThread = PaintownUtil::Thread::uninitializedValue;
    }
}

/* hold the lock when this method is called */
bool Searcher::existsSubscription(Subscriber * who){
    return std::find(subscriptions.begin(), subscriptions.end(), who) != subscriptions.end();
}

void Searcher::subscribe(Subscriber * who){
    PaintownUtil::Thread::ScopedLock scoped1(subscriptionLock);
    if (! existsSubscription(who)){
        subscriptions.push_back(who);

        PaintownUtil::Thread::ScopedLock characterScoped(addCharactersLock);
        PaintownUtil::Thread::ScopedLock stageScoped(addStagesLock);
        who->receiveCharacters(characters);
        who->receiveStages(stages);
    }
}

void Searcher::unsubscribe(Subscriber * who){
    PaintownUtil::Thread::ScopedLock lock(subscriptionLock);
    vector<Subscriber*>::iterator which = std::find(subscriptions.begin(), subscriptions.end(), who);
    if (which != subscriptions.end()){
        subscriptions.erase(which);
    }
}

void Searcher::publishCharacters(const std::vector<Filesystem::AbsolutePath> & files){
    PaintownUtil::Thread::ScopedLock scoped(subscriptionLock);
    for (vector<Subscriber*>::iterator it = subscriptions.begin(); it != subscriptions.end(); it++){
        Subscriber * who = *it;
        who->receiveCharacters(files);
    }
}

void Searcher::publishStages(const std::vector<Filesystem::AbsolutePath> & files){
    PaintownUtil::Thread::ScopedLock scoped(subscriptionLock);
    for (vector<Subscriber*>::iterator it = subscriptions.begin(); it != subscriptions.end(); it++){
        Subscriber * who = *it;
        who->receiveStages(files);
    }
}

void Searcher::addCharacters(const std::vector<Filesystem::AbsolutePath> & files){
    PaintownUtil::Thread::ScopedLock scoped(addCharactersLock);
    characters.insert(characters.end(), files.begin(), files.end());

    publishCharacters(files);
}

void Searcher::addStages(const std::vector<Filesystem::AbsolutePath> & files){
    PaintownUtil::Thread::ScopedLock scoped(addStagesLock);
    stages.insert(stages.end(), files.begin(), files.end());

    publishStages(files);
}

static vector<Filesystem::AbsolutePath> findFiles(const Filesystem::AbsolutePath & path){
    try{
        return Storage::instance().getFilesRecursive(path, "*.def");
    } catch (const Filesystem::NotFound & fail){
        return vector<Filesystem::AbsolutePath>();
    }
}

static vector<Filesystem::AbsolutePath> findFiles(const Filesystem::RelativePath & path){
    try{
        return findFiles(Storage::instance().find(path));
    } catch (const Filesystem::NotFound & fail){
        return vector<Filesystem::AbsolutePath>();
    }
}   

void * Searcher::searchForCharacters(void * arg){
    try{
        Searcher * searcher = (Searcher*) arg;
        vector<Filesystem::AbsolutePath> searchPaths;
        try{
            searchPaths.push_back(Storage::instance().find(Data::getInstance().getMotifDirectory().join(Filesystem::RelativePath("chars"))));
        } catch (const Filesystem::NotFound & fail){
        }

        try{
            searchPaths.push_back(Storage::instance().find(Data::getInstance().getCharDirectory()));
        } catch (const Filesystem::NotFound & fail){
        }

        try{
            searchPaths.push_back(Storage::instance().userDirectory().join(Filesystem::RelativePath("mugen")));
        } catch (const Filesystem::NotFound & fail){
        }

        /* TODO: use a callback to add new characters to be processed instead
         * of processing all files after they have been found.
         */
        for (vector<Filesystem::AbsolutePath>::iterator it = searchPaths.begin(); it != searchPaths.end() && !searcher->searchingCheck.get(); it++){
            Filesystem::AbsolutePath path = *it;
            searcher->addCharacters(findFiles(path));
        }
        Global::debug(1) << "Done searching for characters" << endl;
    } catch (...){
        Global::debug(0) << "Search thread died for some reason" << endl;
    }
    return NULL;
}

void * Searcher::searchStages(void * arg){
    try{
        Searcher * searcher = (Searcher*) arg;
        vector<Filesystem::AbsolutePath> searchPaths;
        try{
            searchPaths.push_back(Storage::instance().find(Data::getInstance().getMotifDirectory().join(Filesystem::RelativePath("stages"))));
        } catch (const Filesystem::NotFound & fail){
        }

        try{
            searchPaths.push_back(Storage::instance().find(Data::getInstance().getStageDirectory()));
        } catch (const Filesystem::NotFound & fail){
        }

        try{
            searchPaths.push_back(Storage::instance().userDirectory().join(Filesystem::RelativePath("mugen")));
        } catch (const Filesystem::NotFound & fail){
        }

        /* TODO: use a callback to add new characters to be processed instead
         * of processing all files after they have been found.
         */
        for (vector<Filesystem::AbsolutePath>::iterator it = searchPaths.begin(); it != searchPaths.end() && !searcher->searchingCheck.get(); it++){
            Filesystem::AbsolutePath path = *it;
            searcher->addStages(findFiles(path));
        }

        Global::debug(1) << "Done searching for stages" << endl;
    } catch (...){
        Global::debug(0) << "Stage search thread died for some reason" << endl;
    }

    return NULL;
}

}
