#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/font.h"
#include "r-tech1/gui/lineedit.h"
#include "r-tech1/debug.h"
#include <iostream>

using namespace Gui;

static Global::stream_type & debug(int level){
    Global::debug(level) << "[line edit] ";
    return Global::debug(level);
}

LineEdit::LineEdit():
blinkRate(30),
cursorTime(0),
changed(false){
    colors.body = Graphics::makeColor(0,0,60);
    colors.border = Graphics::makeColor(0,0,20);
    colors.bodyAlpha = colors.borderAlpha = 150;
    textColor = Graphics::makeColor(255,255,255);
    cursorColor = Graphics::makeColor(128,128,128);
    transforms.setRadius(10);
    input.enable();
}

LineEdit::~LineEdit(){
}

void LineEdit::setTextColor(const Graphics::Color color){
    textColor = color;
}

void LineEdit::setCursorColor(const Graphics::Color color){
    cursorColor = color;
}

void LineEdit::act(const Font &){
    if (input.doInput()){
        changed = true;
    } else {
        changed = false;
    }
    cursorTime++;
    if (cursorTime > blinkRate*2){
        cursorTime = 0;
    }
}

void LineEdit::draw(const Font & font, const Graphics::Bitmap & work){
    
    Graphics::Bitmap temp = Graphics::Bitmap(work, location.getX(), location.getY(), location.getWidth(), location.getHeight());
    
    drawBox(transforms.getRadius(), 0, 0, location.getWidth(), location.getHeight(), colors, temp);
    
    const int modifier = (temp.getWidth() * (transforms.getRadius()*.001)) == 0 ? 2 : (temp.getWidth() * (transforms.getRadius()*.001));
    
    Graphics::Bitmap textTemp = Graphics::Bitmap(temp, modifier, 0, temp.getWidth()-(modifier * 2), temp.getHeight());
    const int fontLength = font.textLength(input.getText().c_str());
    if (fontLength >= textTemp.getWidth()){
        font.printf(textTemp.getWidth() - modifier - fontLength, 0, textColor, textTemp, input.getText(), 0);
        renderCursor(textTemp.getWidth() - modifier - 1, 0, font, textTemp);
    } else {
        font.printf(0, 0, textColor, textTemp, input.getText(), 0);
        renderCursor(fontLength+1, 0, font, textTemp);
    }
}

void LineEdit::render(const Graphics::Bitmap &){
}

void LineEdit::toggleEnabled(){
    if (input.isEnabled()){
        input.disable();
    } else {
        input.enable();
    }
}

void LineEdit::setFocused(bool enabled){
    if (enabled){
        input.enable();
    } else {
        input.disable();
    }
}

void LineEdit::addHook(int key, void (*callback)(void *), void * arg){
    input.addBlockingHandle(key, callback, arg);
}

bool LineEdit::didChanged(unsigned long long &){
    return changed;
}

void LineEdit::renderCursor(int x, int y, const Font & font, const Graphics::Bitmap & work){
    if (cursorTime <= blinkRate){
        font.printf(x, y, cursorColor, work, "|", 0);
    }
}
