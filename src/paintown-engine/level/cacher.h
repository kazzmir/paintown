#ifndef _paintown_cacher_h
#define _paintown_cacher_h

#include "util/pointer.h"

class BlockObject;

namespace Paintown{
    class Object;
}

namespace Level{

class Cacher{
public:
    Cacher();

    virtual Paintown::Object * cache(const Util::ReferenceCount<BlockObject> & block) const = 0;

    virtual ~Cacher();
};

class DefaultCacher: public Cacher {
public:
    DefaultCacher();

    virtual Paintown::Object * cache(const Util::ReferenceCount<BlockObject> & block) const;

    virtual ~DefaultCacher();
};

}

#endif
