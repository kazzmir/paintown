#include "util/file-system.h"
#include "search.h"
#include "config.h"

#include <iostream>
#include <vector>

using std::endl;
using std::vector;

namespace Mugen{

Searcher::Searcher(){
}

Searcher::~Searcher(){
}
    
void Searcher::start(){
}
    
void Searcher::pause(){
}
    

void * Searcher::searchForCharacters(void * arg){
    try{
        Searcher * select = (Searcher*) arg;
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
        /*
        for (vector<Filesystem::AbsolutePath>::iterator it = searchPaths.begin(); it != searchPaths.end() && !select->searchingCheck.get(); it++){
            Filesystem::AbsolutePath path = *it;
            select->addFiles(findFiles(path));
        }
        */

        Global::debug(1) << "Done searching for characters" << endl;
    } catch (...){
        Global::debug(0) << "Search thread died for some reason" << endl;
    }
    return NULL;
}

}
