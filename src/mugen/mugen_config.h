#ifndef mugen_config_h
#define mugen_config_h

#include <string>
#include <vector>
#include <ostream>
#include <queue>
#include <map>
#include "mugen_config_exception.h"

class MugenConfigReader;

// Item represents the information in a Mugen configuration file
// example1 Kung Fu Man is an item for name in name = "Kung Fu Man"
// example2 32 is one item in pos = 32,49
class Item{
  public:
    Item( Item const & copy );
    Item();
    virtual ~Item();
    
    bool operator== ( const string & item );
    bool operator!= ( const string & item );

    Item & operator>>( string & item ) throw( MugenConfigException );
    Item & operator>>( int & item ) throw( MugenConfigException );
    Item & operator>>( double & item ) throw( MugenConfigException );
    Item & operator>>( bool & item ) throw( MugenConfigException );
    
  private:
    std::string name;
};


class ItemContent{
  public:
    ItemContent( ItemContent const & copy );
    ItemContent();
    virtual ~ItemContent();
    
    inline bool hasItems()const { return !itemQueue.empty(); }
    
    void reset();
    
    ItemContent operator=( ItemContent const & i);
    
  private:
    std::queue< const Item &> itemQueue;
    std::vector< Item > items;
};

class ConfigSection{
public:

    ConfigSection( ConfigSection const & copy );
    ConfigSection();
    virtual ~ConfigSection();
    
  private:
    std::string header;
    std::map< const std::string, ItemContent > configItems;

};

#endif
