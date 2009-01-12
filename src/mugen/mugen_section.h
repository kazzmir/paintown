#ifndef mugen_config_h
#define mugen_config_h

#include <string>
#include <vector>
#include <queue>
#include "mugen_exception.h"

class MugenItemContent;

/* Item represents a section in the mugen Configuration file
example:
--------------------------------
; Standing Animation
[Begin Action 000]
Clsn2Default: 2
 Clsn2[0] = -10,  0, 10,-79
 Clsn2[1] =  -4,-92,  6,-79
0,1, 0,0, 7
0,2, 0,0, 7
0,3, 0,0, 7
0,4, 0,0, 50
0,5, 0,0, 7
0,6, 0,0, 7
0,7, 0,0, 7
0,8, 0,0, 60
--------------------------------
header representing [Begin Action 000]
and each subsequent entries are stored in MugenItemContent
that are stored by the lead item
*/
class MugenSection{
public:

    MugenSection( MugenSection const & copy );
    MugenSection();
    virtual ~MugenSection();
    
    inline const std::string &getHeader() const { return header; }
    
    MugenSection & operator=( MugenSection & s);
    
    MugenSection & operator<<( const MugenItemContent & item ) throw (MugenException);
    
    const MugenItemContent & getNext();
    
    void reset();
    
  private:
    std::string header;
    std::queue< MugenItemContent * > itemContentQueue;
    std::vector< MugenItemContent > itemContent;
    
    // Empty MugenItemContent
    static MugenItemContent empty;

};

#endif
