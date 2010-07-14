#ifndef mugen_item_content_h
#define mugen_item_content_h

#include <string>
#include <vector>

class MugenItem;

// This hold several Mugen Items
// example like a sprite group number, sprite number, pos x, pos y, display time/ticks, flip, color addition
// so the total MugenItems will have between 5 to 7 entries (last two are optional)

class MugenItemContent{
  public:
    MugenItemContent( const MugenItemContent & copy );
    MugenItemContent();
    virtual ~MugenItemContent();
    
    bool hasItems() const;
    
    void reset();
    
    void clear();
    
    MugenItemContent & operator=( const MugenItemContent & i);
  
    MugenItemContent & operator<<( const std::string & item );
 
    MugenItem * getNext();
    
    const std::string & getHead() const;
    
  private:
    unsigned int itemCounter;
    std::vector< MugenItem *> items;
    
    static MugenItem empty;
};


#endif
