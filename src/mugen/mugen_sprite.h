#ifndef mugen_sprite_h
#define mugen_sprite_h

#include <string>

/* basically a struct so no need for accessors/mutators */
class MugenSprite{
public:
    MugenSprite();
    MugenSprite( const MugenSprite &copy );
    ~MugenSprite();
    
    MugenSprite & operator=( const MugenSprite &copy );
    // For map searching
    bool operator<( const MugenSprite &copy );
    
    unsigned long next;
    unsigned long location;
    unsigned long length;
    unsigned long reallength;
    unsigned long newlength;
    short x;
    short y;
    unsigned short groupNumber;
    unsigned short imageNumber;
    unsigned short prev;
    bool samePalette;
    char comments[13];
    char * pcx;
};

#endif
