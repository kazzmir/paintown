#ifndef _paintown_block_h
#define _paintown_block_h

#include <vector>
#include "util/pointer.h"

class BlockObject;
class Token;

namespace Paintown{
    class Object;
}

namespace Level{
    class Cacher;
}

class Block{
public:
    Block(const Token * tok, const Level::Cacher & cacher);
    Block();

    inline void setLength( const int l ){
        length = l;
    }

    inline int getId() const {
        return id;
    }

    inline void setId(const int id){
        this->id = id;
    }

    inline int getLength() const {
        return length;
    }

    inline bool isContinuous() const {
        return continuous;
    }

    inline void setContinuous( const bool b ){
        continuous = b;
    }

    /* finished is the length the player has to walk
     * to complete the level
     */
    inline int getFinished() const {
        return finished;
    }

    inline void setFinished( const int f ){
        finished = f;
    }

    void addBlockObject(const Util::ReferenceCount<BlockObject> & object);

    bool empty();

    // std::vector< Heart * > createObjects( int total_length, int min_x, int max_x, int min_z, int max_z, std::vector< Paintown::Object * > * list );

    virtual ~Block();

    const std::vector<Util::ReferenceCount<BlockObject> > & getObjects() const {
        return objects;
    }

protected:
    int length;
    int id;
    bool wait;

    std::vector<Util::ReferenceCount<BlockObject> > objects;
    int finished;
    bool continuous;
};

#endif
