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
characterSearch(*this),
stageSearch(*this){
}

Searcher::~Searcher(){
    characterSearch.pause();
}
    
void Searcher::start(){
    characterSearch.start();
    stageSearch.start();
    /*
    if (characterSearchThread == PaintownUtil::Thread::uninitializedValue){
        if (!PaintownUtil::Thread::createThread(&characterSearchThread, NULL, (PaintownUtil::Thread::ThreadFunction) searchForCharacters, this)){
            Global::debug(0) << "Could not create character search thread" << endl;
        }
    }
    */

    /*
    if (stageSearchThread == PaintownUtil::Thread::uninitializedValue){
        if (!PaintownUtil::Thread::createThread(&stageSearchThread, NULL, (PaintownUtil::Thread::ThreadFunction) searchStages, this)){
            Global::debug(0) << "Could not create stage search thread" << endl;
        }
    }
    */
}
    
void Searcher::pause(){
    characterSearch.pause();
    stageSearch.pause();

    // searchingCheck.set(true);
    /*
    if (characterSearchThread != PaintownUtil::Thread::uninitializedValue){
        PaintownUtil::Thread::joinThread(characterSearchThread);
        characterSearchThread = PaintownUtil::Thread::uninitializedValue;
    }
    */
    
    /*
    if (stageSearchThread != PaintownUtil::Thread::uninitializedValue){
        PaintownUtil::Thread::joinThread(stageSearchThread);
        stageSearchThread = PaintownUtil::Thread::uninitializedValue;
    }
    */
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

static vector<Filesystem::AbsolutePath> findFiles(const Filesystem::AbsolutePath & path, const std::string & extension){
    try{
        return Storage::instance().getFilesRecursive(path, std::string("*.") + extension);
    } catch (const Filesystem::NotFound & fail){
        return vector<Filesystem::AbsolutePath>();
    }
}

static vector<Filesystem::AbsolutePath> findFiles(const Filesystem::RelativePath & path, const std::string & extension){
    try{
        return findFiles(Storage::instance().find(path), extension);
    } catch (const Filesystem::NotFound & fail){
        return vector<Filesystem::AbsolutePath>();
    }
}   
        
Searcher::CharacterSearch::CharacterSearch(Searcher & owner):
owner(owner),
thread(PaintownUtil::Thread::uninitializedValue),
searching(false),
searchingCheck(searching, searchingLock.getLock()){
    try{
        paths.push_back(Storage::instance().find(Data::getInstance().getMotifDirectory().join(Filesystem::RelativePath("chars"))));
    } catch (const Filesystem::NotFound & fail){
    }

    try{
        paths.push_back(Storage::instance().find(Data::getInstance().getCharDirectory()));
    } catch (const Filesystem::NotFound & fail){
    }

    try{
        paths.push_back(Storage::instance().userDirectory().join(Filesystem::RelativePath("mugen")));
    } catch (const Filesystem::NotFound & fail){
    }
}

void Searcher::CharacterSearch::search(){
    /* Quit if either we run out of paths to process or if the searcher
     * is paused
     */
    while (paths.size() > 0 && searchingCheck.get()){
        Filesystem::AbsolutePath path = paths.front();
        paths.erase(paths.begin());
        owner.addCharacters(findFiles(path, "def"));
        /* FIXME: change this to add all container files */
        owner.addCharacters(findFiles(path, "zip"));
    }
}

void * Searcher::CharacterSearch::runSearch(void * self_){
    Searcher::CharacterSearch * self = (Searcher::CharacterSearch*) self_;
    self->search();
    return NULL;
}

void Searcher::CharacterSearch::start(){
    if (!searchingCheck.get()){
        searchingCheck.set(true);
        if (!PaintownUtil::Thread::createThread(&thread, NULL, (PaintownUtil::Thread::ThreadFunction) runSearch, this)){
            searchingCheck.set(false);
        }
    }
}

void Searcher::CharacterSearch::pause(){
    searchingCheck.set(false);
    if (thread != PaintownUtil::Thread::uninitializedValue){
        PaintownUtil::Thread::joinThread(thread);
        thread = PaintownUtil::Thread::uninitializedValue;
    }
}
        
Searcher::CharacterSearch::~CharacterSearch(){
    pause();
}

Searcher::StageSearch::StageSearch(Searcher & owner):
owner(owner),
thread(PaintownUtil::Thread::uninitializedValue),
searching(false),
searchingCheck(searching, searchingLock.getLock()){
    try{
        paths.push_back(Storage::instance().find(Data::getInstance().getMotifDirectory().join(Filesystem::RelativePath("stages"))));
    } catch (const Filesystem::NotFound & fail){
    }

    try{
        paths.push_back(Storage::instance().find(Data::getInstance().getStageDirectory()));
    } catch (const Filesystem::NotFound & fail){
    }

    try{
        paths.push_back(Storage::instance().userDirectory().join(Filesystem::RelativePath("mugen")));
    } catch (const Filesystem::NotFound & fail){
    }
}

void Searcher::StageSearch::start(){
    if (!searchingCheck.get()){
        searchingCheck.set(true);
        if (!PaintownUtil::Thread::createThread(&thread, NULL, (PaintownUtil::Thread::ThreadFunction) runSearch, this)){
            searchingCheck.set(false);
        }
    }
}

void Searcher::StageSearch::pause(){
    searchingCheck.set(false);
    if (thread != PaintownUtil::Thread::uninitializedValue){
        PaintownUtil::Thread::joinThread(thread);
        thread = PaintownUtil::Thread::uninitializedValue;
    }
}

Searcher::StageSearch::~StageSearch(){
    pause();
}

void * Searcher::StageSearch::runSearch(void * self_){
    Searcher::StageSearch * self = (Searcher::StageSearch*) self_;
    self->search();
    return NULL;
}

void Searcher::StageSearch::search(){
    while (paths.size() > 0 && searchingCheck.get()){
        Filesystem::AbsolutePath path = paths.front();
        paths.erase(paths.begin());
        owner.addStages(findFiles(path, "def"));
    }
}

}
