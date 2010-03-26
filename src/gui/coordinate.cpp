#include "gui/coordinate.h"

#include "globals.h"

using namespace Gui;

static int relativeToAbsolute(double x, int center){
    return (int)(center + (center * x));
}

static double absoluteToRelative(int x, int center){
    return (x-center)/center;
}

AbsolutePoint::AbsolutePoint():
x(0),
y(0){
}
AbsolutePoint::AbsolutePoint(int x, int y):
x(x),
y(y){
}
AbsolutePoint::AbsolutePoint(const AbsolutePoint & copy):
x(copy.x),
y(copy.y){
}
AbsolutePoint::~AbsolutePoint(){
}

const AbsolutePoint & AbsolutePoint::operator=(const AbsolutePoint & copy){
    this->x = copy.x;
    this->y = copy.y;
    return *this;
}

int AbsolutePoint::getX(){
    return x;
}
int AbsolutePoint::getY(){
    return y;
}


RelativePoint::RelativePoint():
x(0),
y(0){
}
RelativePoint::RelativePoint(double x, double y):
x(x),
y(y){
}

RelativePoint::RelativePoint(const RelativePoint & copy):
x(copy.x),
y(copy.y){
}

RelativePoint::RelativePoint(AbsolutePoint & point):
x(absoluteToRelative(point.getX(),Global::getScreenWidth()/2)),
y(absoluteToRelative(point.getY(),Global::getScreenHeight()/2)){
}

RelativePoint::~RelativePoint(){
}

const RelativePoint & RelativePoint::operator=(const RelativePoint & copy){
    this->x = copy.x;
    this->y = copy.y;
    return *this;
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
Coordinate::Coordinate(const AbsolutePoint & position, const AbsolutePoint & dimensions){
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

