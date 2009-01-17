#ifndef mugen_sprite_h
#define mugen_sprite_h

#include <string>

class MugenSprite{
public:
    MugenSprite();
    MugenSprite( const MugenSprite &copy );
    ~MugenSprite();
    
    int next;
    int pcxSize;
    int x;
    int y;
    int groupNumber;
    int imageNumber;
    int prev;
    bool samePalette;
    char comments[14];
    char * pcxData;

};

#endif
