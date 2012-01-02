#include "character-select.h"

using namespace Mugen;

::Util::ReferenceCount<MugenSprite> Cell::background = ::Util::ReferenceCount<MugenSprite>(NULL);
::Util::ReferenceCount<MugenSprite> Cell::randomIcon = ::Util::ReferenceCount<MugenSprite>(NULL);

Cell::Cell():
empty(true),
isRandom(false){
}

Cell::~Cell(){
}

void Cell::act(){
}

void Cell::draw(int x, int y, int width, int height, const Graphics::Bitmap &, const Font &) const{
}

bool Cell::isEmpty() const{
    return empty;
}

void Cell::setRandom(bool r){
    isRandom = r;
}

void Cell::setStaticData(::Util::ReferenceCount<MugenSprite> background, ::Util::ReferenceCount<MugenSprite> randomIcon){
    Cell::background = background;
    Cell::randomIcon = randomIcon;
}

CharacterSelect::CharacterSelect(){
}

CharacterSelect::~CharacterSelect(){
}