#include "mugen_item_content.h"
#include "mugen_item.h"

MugenItem MugenItemContent::empty("empty");


MugenItemContent::MugenItemContent( const MugenItemContent & copy ){
  items = copy.items;
  reset();
}

MugenItemContent::MugenItemContent(){
}

MugenItemContent::~MugenItemContent(){
}

void MugenItemContent::reset(){
  while( !itemQueue.empty() )itemQueue.pop();
  for ( std::vector< MugenItem >::iterator begin = items.begin() ; begin != items.end() ; ++begin ){
    this->itemQueue.push( &items.back() );
  }
}

const MugenItem & MugenItemContent::getNext(){
  if( items.empty() )return empty;
  const MugenItem *temp = itemQueue.front();
  itemQueue.pop();
  return *temp;
}

const std::string & MugenItemContent::getHead() const{ 
   if( items.empty() ) empty.query();
   return items.front().query(); 
}

MugenItemContent & MugenItemContent::operator=( MugenItemContent & i){
  items.clear();
  while( !itemQueue.empty() )itemQueue.pop();
  for ( std::vector< MugenItem >::iterator begin = i.items.begin() ; begin != i.items.end() ; ++begin ){
    this->items.push_back( (*begin) );
    this->itemQueue.push( &items.back() );
  }
  return *this;
}

MugenItemContent & MugenItemContent::operator<<( const std::string & item ){
  this->items.push_back( MugenItem( item ) );
  this->itemQueue.push( &items.back() );
  return *this;
}


