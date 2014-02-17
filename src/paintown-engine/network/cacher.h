#ifndef _paintown_network_cacher_h
#define _paintown_network_cacher_h

#include "../level/cacher.h"

class NetworkCacher: public Level::Cacher {
public:
    NetworkCacher();
    
    virtual Paintown::Object * cache(const Util::ReferenceCount<BlockObject> & block) const;

    virtual ~NetworkCacher();
};

#endif
