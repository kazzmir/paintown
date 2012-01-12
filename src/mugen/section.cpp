#include "mugen/section.h"
#include "mugen/item-content.h"

MugenItemContent MugenSection::empty;

MugenSection::MugenSection( MugenSection const & copy ) :
header("empty"),
contentCounter(0){
  this->header = copy.header;
  this->itemContent = copy.itemContent;
  this->reset();
}

MugenSection::MugenSection() :
header( "empty" ),
contentCounter(0){
}

MugenSection::~MugenSection(){
  for( std::vector< MugenItemContent *>::iterator i = itemContent.begin(); i != itemContent.end(); ++i ){
      if(*i)delete (*i);
  }
}

bool MugenSection::hasItems() const { 
  return ( contentCounter < itemContent.size() ); 
}
    
MugenSection & MugenSection::operator=( const MugenSection & s){
  this->header = s.header;
  this->itemContent = s.itemContent;
  this->reset();
  
  return *this;
}

MugenSection & MugenSection::operator<<( MugenItemContent * item ) throw( MugenException ){
  if ( header == "empty" ) throw MugenException( std::string("This section has no header, cannot add items!"), __FILE__, __LINE__);
  this->itemContent.push_back( item );  
  return *this;
  
}
    
MugenItemContent * MugenSection::getNext(){
  if( itemContent.empty() || ( contentCounter == itemContent.size() ) )return &empty;
  const unsigned int temp = contentCounter;
  contentCounter++;
  return itemContent[temp];
}

void MugenSection::reset(){
  contentCounter =0;
}

void MugenSection::clear(){
  contentCounter = 0;
  for( std::vector< MugenItemContent *>::iterator i = itemContent.begin(); i != itemContent.end(); ++i ){
      delete (*i);
  }
  itemContent.clear();
  header = "empty";
}

