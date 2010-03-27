#include "util/bitmap.h"

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
bool RelativePoint::operator==(const RelativePoint & point){
    return (this->x == point.x && this->y == point.y);
}
bool RelativePoint::operator!=(const RelativePoint & point){
    return (this->x != point.x || this->y != point.y);
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

Coordinate::Coordinate():
z(0),
radius(0){
}
Coordinate::Coordinate(AbsolutePoint & position, AbsolutePoint & dimensions):
position(position),
dimensions(dimensions),
z(0),
radius(0){
}
Coordinate::Coordinate(const RelativePoint &, const RelativePoint &):
position(position),
dimensions(dimensions),
z(0),
radius(0){
}
Coordinate::~Coordinate(){
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
int Coordinate::getX2(){
    return position.getX() + dimensions.getX();
}
int Coordinate::getY2(){
    return position.getY() + dimensions.getY();
}
bool Coordinate::operator==( const Coordinate & coord){
    return ( (position == coord.position) &&
            (dimensions == coord.dimensions));
}

bool Coordinate::operator!=( const Coordinate & coord){
    return ( (position != coord.position) ||
            (dimensions != coord.dimensions));
}

bool Coordinate::operator==( const Bitmap & bmp){
    return ( (dimensions.getX() == bmp.getWidth()) &&
            (dimensions.getY() == bmp.getHeight()));
}

bool Coordinate::operator!=( const Bitmap & bmp){
    return ( (dimensions.getX() != bmp.getWidth()) ||
            (dimensions.getY() != bmp.getHeight()));
}
