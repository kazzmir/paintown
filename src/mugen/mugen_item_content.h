#ifndef mugen_item_content_h
#define mugen_item_content_h

#include <string>
#include <vector>
#include <queue>

// This hold several Mugen Items
// example like a sprite group number, sprite number, pos x, pos y, display time/ticks, flip, color addition
// so the total MugenItems will have between 5 to 7 entries (last two are optional)

class MugenItemContent{
  public:
    MugenItemContent( MugenItemContent const & copy );
    MugenItemContent();
    virtual ~MugenItemContent();
    
    inline bool hasItems()const { return !itemQueue.empty(); }
    
    void reset();
    
    MugenItemContent & operator=( const MugenItemContent & i);
    MugenItemContent & operator<<( const string & item );
    
    inline const std::string &getHead() const { return items.front().query(); };
    
  private:
    std::queue< const MugenItem &> itemQueue;
    std::vector< MugenItem > items;
};


#endif
