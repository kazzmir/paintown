#ifndef mugen_item_h
#define mugen_item_h

#include <string>
#include "mugen_config_exception.h"

Class MugenItem;

// Item represents the information in a Mugen configuration file
// example1 Kung Fu Man is an item for name in name = "Kung Fu Man"
// example2 32 is one item in pos = 32,49
class MugenItem{
  public:
    MugenItem( MugenItem const & copy );
    MugenItem( const std::string &name );
    virtual ~Item();
    
    bool operator== ( const std::string & item );
    bool operator!= ( const std::string & item );
    
    MugenItem & operator=( const std::string &item );
    MugenItem & operator=( const MugenItem &item );

    MugenItem & operator>>( const std::string & item ) throw( MugenConfigException );
    MugenItem & operator>>( int & item ) throw( MugenConfigException );
    MugenItem & operator>>( double & item ) throw( MugenConfigException );
    MugenItem & operator>>( bool & item ) throw( MugenConfigException );
    
    inline const std::string &query() const { return name; }
    
  private:
    std::string name;
};

#endif
