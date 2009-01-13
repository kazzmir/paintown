#ifndef mugen_item_content_h
#define mugen_item_content_h

#include <string>
#include <vector>
#include <queue>

class MugenItem;

// This hold several Mugen Items
// example like a sprite group number, sprite number, pos x, pos y, display time/ticks, flip, color addition
// so the total MugenItems will have between 5 to 7 entries (last two are optional)

class MugenItemContent{
  public:
    MugenItemContent( const MugenItemContent & copy );
    MugenItemContent();
    virtual ~MugenItemContent();
    
    inline bool hasItems()const { return !itemQueue.empty(); }
    
    void reset();
    
    void clear();
    
    MugenItemContent & operator=( const MugenItemContent & i);
  
    MugenItemContent & operator<<( const std::string & item );
 
    const MugenItem & getNext();
    
    const std::string & getHead() const;
    
  private:
    std::queue< const MugenItem *> itemQueue;
    std::vector< MugenItem > items;
    
    static MugenItem empty;
};


#endif
