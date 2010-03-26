#include "gui/coordinate.h"

#include "globals.h"

using namespace Gui;

static int relativeToAbsolute(double x, int center){
    return (int)(center + (center * x));
}

static double absoluteToRelative(int x, int center){
    return (x-center)/center;
}

AbsolutePoint::AbsolutePoint(){
}
AbsolutePoint::AbsolutePoint(int x, int y){
}
AbsolutePoint::~AbsolutePoint(){
}
int AbsolutePoint::getX(){
    return x;
}
int AbsolutePoint::getY(){
    return y;
}


RelativePoint::RelativePoint(){
}
RelativePoint::RelativePoint(double x, double y){
}
RelativePoint::~RelativePoint(){
}
int RelativePoint::getX(){
    return relativeToAbsolute(x,Global::getScreenWidth()/2);
}
int RelativePoint::getY(){
    return relativeToAbsolute(y,Global::getScreenHeight()/2);
}
AbsolutePoint RelativePoint::getAbsolute(){
    return AbsolutePoint(relativeToAbsolute(x,Global::getScreenWidth()/2), relativeToAbsolute(y,Global::getScreenHeight()/2));
}
double RelativePoint::getRelativeX(){
    return x;
}
double RelativePoint::getRelativeY(){
    return y;
}

Coordinate::Coordinate(){
}
Coordinate::Coordinate(const AbsolutePoint &, const AbsolutePoint &){
}
Coordinate::Coordinate(const RelativePoint &, const RelativePoint &){
}
Coordinate::~Coordinate(){
}
void Coordinate::setZ(double z){
}
void Coordinate::setRadius(double radius){
}
int Coordinate::getX(){
    return position.getX();
}
int Coordinate::getY(){
    return position.getY();
}
int Coordinate::getWidth(){
    return dimensions.getX();
}
int Coordinate::getHeight(){
    return dimensions.getY();
}

