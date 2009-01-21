#ifndef mugen_sprite_h
#define mugen_sprite_h

#include <string>

/* basically a struct so no need for accessors/mutators */
class MugenSprite{
public:
    MugenSprite();
    MugenSprite( const MugenSprite &copy );
    ~MugenSprite();
    
    unsigned long next;
    unsigned long length;
    unsigned short x;
    unsigned short y;
    unsigned short groupNumber;
    unsigned short imageNumber;
    unsigned short prev;
    bool samePalette;
    char comments[13];
    char * pcx;
};

#endif
