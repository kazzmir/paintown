#ifndef _paintown_cacher_h
#define _paintown_cacher_h

class BlockObject;
class Object;

namespace Level{

class Cacher{
public:
    Cacher();

    /* const may not be a good idea.. */
    virtual Object * cache(const BlockObject & block) const = 0;

    virtual ~Cacher();
};

class DefaultCacher: public Cacher {
public:
    DefaultCacher();

    virtual Object * cache(const BlockObject & block) const;

    virtual ~DefaultCacher();
};

}

#endif
