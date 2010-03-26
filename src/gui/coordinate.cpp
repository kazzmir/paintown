#include "gui/coordinate.h"

#include "globals.h"

using namespace Gui;

static int getHorizontalAbsolute(double x){
    const int center = Global::getScreenWidth()/2;
    return (int)(center + (center * x));
}

static int getVerticalAbsolute(double y){
    const int center = Global::getScreenHeight()/2;
    return (int)(center + (center * y));
}

static double getHorizontalRelative(int x){
    const int center = Global::getScreenWidth()/2;
    if (x == center){
        return 0;
    } else if (x < center){
        return -( (x/center) * .01);
    } else if (x > center){
        return ( ((x - center)/center) * .01);
    }
    return 0;
}

static double getVerticalRelative(int y){
    const int center = Global::getScreenHeight()/2;
    if (y == center){
        return 0;
    } else if (y < center){
        return -( (y/center) * .01);
    } else if (y > center){
        return ( ((y - center)/center) * .01);
    }
    return 0;
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
    return getHorizontalAbsolute(x);
}
int RelativePoint::getY(){
    return getVerticalAbsolute(y);
}
AbsolutePoint RelativePoint::getAbsolute(){
    return AbsolutePoint(getHorizontalAbsolute(x),getVerticalAbsolute(y));
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

