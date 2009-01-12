#include "mugen_section.h"
#include "mugen_item_content.h"

MugenSection::MugenSection( MugenSection const & copy ) :
header("empty"){
  this->header = copy.header;
  this->itemContent = copy.itemContent;
}

MugenSection::MugenSection() :
header("empty"){
}

MugenSection::~MugenSection(){
}
    
MugenSection & MugenSection::operator=( MugenSection & s){
  this->header = s.header;
  this->itemContent = s.itemContent;
  
  return *this;
}

