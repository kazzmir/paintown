#include "mugen_item.h"
#include <ostream>
#include <sstream>
#include <iostream>

MugenItem::MugenItem( MugenItem const & copy ) :
name(""){
  this->name = copy.name;
}
MugenItem::MugenItem( const std::string &name ) :
name(""){
  this->name = name;
}

MugenItem::~MugenItem(){
}
    
bool MugenItem::operator== ( const std::string & item ){
  return (name == item);
}
    
MugenItem & MugenItem::operator=( const std::string &item ){
  this->name = item;
  return *this;
}

MugenItem & MugenItem::operator=( const MugenItem &item ){
  this->name = item.name;
  return *this;
}

MugenItem & MugenItem::operator>>( std::string & item ){
  item = this->name;
  return *this;
}
MugenItem & MugenItem::operator>>( int & item ){
  std::istringstream is ( this->name );
  is >> item;
  return *this;
}
MugenItem & MugenItem::operator>>( double & item ){
  std::istringstream is ( this->name );
  is >> item;
  return *this;
}
MugenItem & MugenItem::operator>>( bool & item ){
  std::istringstream is ( this->name );
  is >> item;
  return *this;
}


