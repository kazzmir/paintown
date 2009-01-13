#include "mugen_item_content.h"
#include "mugen_item.h"

MugenItem MugenItemContent::empty("empty");

MugenItemContent::MugenItemContent( const MugenItemContent & copy ) : 
itemCounter(0){
  items = copy.items;
  reset();
}

MugenItemContent::MugenItemContent() :
itemCounter(0){
}

MugenItemContent::~MugenItemContent(){
  for( std::vector< MugenItem *>::iterator i = items.begin(); i != items.end(); ++i ){
      delete (*i);
  }
}

const bool MugenItemContent::hasItems()const { 
  return ( itemCounter < items.size() ); 
}

void MugenItemContent::reset(){
  itemCounter = 0;
}

void MugenItemContent::clear(){
  itemCounter = 0;
  for( std::vector< MugenItem *>::iterator i = items.begin(); i != items.end(); ++i ){
      delete (*i);
  }
  items.clear();
}

const MugenItem * MugenItemContent::getNext(){
  if( items.empty() || itemCounter == items.size() )return &empty;
  const unsigned int temp = itemCounter;
  itemCounter++;
  return items[temp];
}

const std::string & MugenItemContent::getHead() const{ 
   if( items.empty() ) empty.query();
   return items.front()->query(); 
}

MugenItemContent & MugenItemContent::operator=( const MugenItemContent & i){
  items = i.items;
  reset();
  return *this;
}

MugenItemContent & MugenItemContent::operator<<( const std::string & item ){
  this->items.push_back( new MugenItem( item ) );
  return *this;
}


