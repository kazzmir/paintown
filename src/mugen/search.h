#ifndef _paintown_mugen_search_h
#define _paintown_mugen_search_h

/* searches for mugen stuff (characters, stages, etc) in the background */

namespace Mugen{

class Searcher{
public:
    Searcher();
    virtual ~Searcher();

    /* either start searching or unpause */
    void start();
    /* pause searching */
    void pause();

protected:
    static void * searchForCharacters(void * arg);
};

}

#endif
