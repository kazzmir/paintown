#ifndef _paintown_block_h
#define _paintown_block_h

#include <vector>

class BlockObject;
class Token;
class Heart;
class Object;

namespace Level{
    class Cacher;
}

class Block{
public:
    Block(Token * tok, const Level::Cacher & cacher);
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

    void addBlockObject(BlockObject * object);

    bool empty();

    std::vector< Heart * > createObjects( int total_length, int min_x, int max_x, int min_z, int max_z, std::vector< Object * > * list );

    virtual ~Block();

    const std::vector<BlockObject*> & getObjects() const {
        return objects;
    }

protected:
    int length;
    int id;
    bool wait;

    std::vector< BlockObject * > objects;
    int finished;
    bool continuous;
};

#endif
