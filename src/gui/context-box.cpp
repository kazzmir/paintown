#include "gui/context-box.h"

using namespace std;
using namespace Gui;

ContextItem::ContextItem(){
}
ContextItem::~ContextItem(){
}

ContextBox::ContextBox():
current(0),
fadeState(NotActive),
fontWidth(0),
fontHeight(0){
}
ContextBox::ContextBox( const ContextBox & copy ):
current(0),
fadeState(NotActive){
    this->context = copy.context;
    this->font = copy.font;
    this->fontWidth = copy.fontWidth;
    this->fontHeight = copy.fontHeight;
    
}
ContextBox::~ContextBox(){
}
ContextBox & ContextBox::operator=( const ContextBox & copy){
    this->current = 0;
    this->fadeState = NotActive;
    this->context = copy.context;
    this->font = copy.font;
    this->fontWidth = copy.fontWidth;
    this->fontHeight = copy.fontHeight;
    return *this;
}

void ContextBox::act(){
}

void ContextBox::render(const Bitmap & work){
}
void ContextBox::next(){
}
void ContextBox::previous(){
}
