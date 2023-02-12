#include "r-tech1/graphics/gradient.h"
#include "r-tech1/funcs.h"
#include "r-tech1/token.h"

namespace Effects{
    
Gradient::Gradient():
colors(NULL),
size(1),
index(0){
    colors = new Graphics::Color[size];
    colors[0] = Graphics::makeColor(255, 255, 255);
}
    
Gradient::Gradient(Graphics::Color singleColor):
colors(NULL),
size(1),
index(0){
    colors = new Graphics::Color[size];
    colors[0] = singleColor;
}

/* this class does virtually no error checking. great job */
Gradient::Gradient(int size, Graphics::Color startColor, Graphics::Color endColor):
colors(0),
size(size),
index(0){
    size = Util::clamp(size, 2, 10000);
    colors = new Graphics::Color[size];
    Graphics::blend_palette(colors, size / 2, startColor, endColor);
    Graphics::blend_palette(colors + size / 2, size / 2, endColor, startColor);
}

Gradient::Gradient(const Token * token):
colors(NULL),
size(10),
index(0){
    int lowRed = 0, lowGreen = 0, lowBlue = 0;
    int highRed = 255, highGreen = 255, highBlue = 255;
    token->match("_/low", lowRed, lowGreen, lowBlue);
    token->match("_/high", highRed, highGreen, highBlue);
    token->match("_/distance", size);
    token->match("_/size", size);

    size = Util::clamp(size, 2, 10000);

    Graphics::Color startColor = Graphics::makeColor(lowRed, lowGreen, lowBlue);
    Graphics::Color endColor = Graphics::makeColor(highRed, highGreen, highBlue);

    colors = new Graphics::Color[size];
    Graphics::blend_palette(colors, size / 2, startColor, endColor);
    Graphics::blend_palette(colors + size / 2, size / 2, endColor, startColor);
}

Gradient::Gradient(const Gradient & copy):
colors(NULL),
size(copy.size),
index(copy.index){
    colors = new Graphics::Color[size];
    for (unsigned int i = 0; i < size; i++){
        colors[i] = copy.colors[i];
    }
}
    
Gradient & Gradient::operator=(const Gradient & copy){
    delete[] colors;
    size = copy.size;
    index = copy.index;
    colors = new Graphics::Color[size];
    for (unsigned int i = 0; i < size; i++){
        colors[i] = copy.colors[i];
    }
    return *this;
}

void Gradient::forward(){
    index = (index + 1) % size;
}

void Gradient::backward(){
    /* This goofy logic is used because index is an unsigned int and its a bad idea
     * to subtract 1 from an unsigned int whose value is 0.
     */
    if (index == 0){
        index = size - 1;
    } else {
        index -= 1;
    }
}

void Gradient::update(){
    forward();
}

void Gradient::reset(){
    index = 0;
}

Graphics::Color Gradient::current() const {
    return colors[index];
}
    
Graphics::Color Gradient::current(int offset) const {
    return colors[(index + offset + size) % size];
}

Gradient::~Gradient(){
    delete[] colors;
}

}
