#ifndef mugen_item_h
#define mugen_item_h

#include <string>

// Item represents the information in a Mugen configuration file
// example1 Kung Fu Man is an item for name in name = "Kung Fu Man"
// example2 32 is one item in pos = 32,49
class MugenItem{
  public:
    MugenItem( MugenItem const & copy );
    MugenItem( const std::string &name );
    virtual ~MugenItem();
    
    bool operator== ( const std::string & item );
    
    MugenItem & operator=( const std::string &item );
    MugenItem & operator=( const MugenItem &item );

    MugenItem & operator>>( std::string & item );
    MugenItem & operator>>( int & item );
    MugenItem & operator>>( char & item );
    MugenItem & operator>>( double & item );
    MugenItem & operator>>( bool & item );
    
    inline const std::string &query() const { return name; }
    
  private:
    std::string name;
};

#endif
