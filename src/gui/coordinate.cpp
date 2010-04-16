#include "util/bitmap.h"

#include "gui/coordinate.h"

#include "globals.h"

using namespace Gui;

static int relativeToAbsolute(double x, int center){
    return (int)(center + (center * x));
}

static double absoluteToRelative(int x, int center){
    return (double)(x-center)/center;
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
x(-1),
y(-1){
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
const RelativePoint & RelativePoint::operator=(AbsolutePoint & point){
    this->x = absoluteToRelative(point.getX(), Global::getScreenWidth()/2);
    this->y = absoluteToRelative(point.getY(), Global::getScreenHeight()/2);
    return *this;
}
bool RelativePoint::operator==(const RelativePoint & point){
    return (this->x == point.x && this->y == point.y);
}
bool RelativePoint::operator!=(const RelativePoint & point){
    return !(*this == point);
}

int RelativePoint::getX() const{
    return relativeToAbsolute(x,Global::getScreenWidth()/2);
}
int RelativePoint::getY() const{
    return relativeToAbsolute(y,Global::getScreenHeight()/2);
}
AbsolutePoint RelativePoint::getAbsolute(){
    return AbsolutePoint(relativeToAbsolute(x,Global::getScreenWidth()/2), relativeToAbsolute(y,Global::getScreenHeight()/2));
}
double RelativePoint::getRelativeX() const{
    return x;
}
double RelativePoint::getRelativeY() const{
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
Coordinate::Coordinate(const RelativePoint & position, const RelativePoint & dimensions):
position(position),
dimensions(dimensions),
z(0),
radius(0){
}
Coordinate::Coordinate(const Coordinate & copy):
position(copy.position),
dimensions(copy.dimensions),
z(copy.z),
radius(copy.radius){
}
Coordinate::~Coordinate(){
}

const Coordinate & Coordinate::operator=(const Coordinate & copy){
    this->position = copy.position;
    this->dimensions = copy.dimensions;
    this->z = copy.z;
    this->radius = copy.radius;
    
    return *this;
}

int Coordinate::getX() const{
    return position.getX();
}
int Coordinate::getY() const{
    return position.getY();
}
int Coordinate::getWidth() const{
    return dimensions.getX() - position.getX();
}
int Coordinate::getHeight() const{
    return dimensions.getY() - position.getY();
}
int Coordinate::getX2() const{
    return dimensions.getX();
}
int Coordinate::getY2() const{
    return dimensions.getY();
}

void Coordinate::growHorizontal(double by){
    position.setX(position.getRelativeX() - by);
    dimensions.setX(dimensions.getRelativeX() + by);
}

void Coordinate::growVertical(double by){
    position.setY(position.getRelativeY() - by);
    dimensions.setY(dimensions.getRelativeY() + by);
}

void Coordinate::growTo(const Coordinate & coord, double percent){
    if (position.getRelativeX() > coord.position.getRelativeX()){
		position.setX(position.getRelativeX() - percent);
        if (position.getRelativeX() < coord.position.getRelativeX()){
            position.setX(coord.position.getRelativeX());
        }
    } else if (position.getRelativeX() < coord.position.getRelativeX()){
		position.setX(position.getRelativeX() + percent);
        if (position.getRelativeX() > coord.position.getRelativeX()){
            position.setX(coord.position.getRelativeX());
        }
	}
    if (position.getRelativeY() > coord.position.getRelativeY()){
		position.setY(position.getRelativeY() - percent);
        if (position.getRelativeY() < coord.position.getRelativeY()){
            position.setY(coord.position.getRelativeY());
        }
    } else if (position.getRelativeY() < coord.position.getRelativeY()){
		position.setY(position.getRelativeY() + percent);
        if (position.getRelativeY() > coord.position.getRelativeY()){
            position.setY(coord.position.getRelativeY());
        }
	}
    if (dimensions.getRelativeX() > coord.dimensions.getRelativeX()){
		dimensions.setX(dimensions.getRelativeX() - percent);
        if (dimensions.getRelativeX() < coord.dimensions.getRelativeX()){
            dimensions.setX(coord.dimensions.getRelativeX());
        }
    } else if (dimensions.getRelativeX() < coord.dimensions.getRelativeX()){
		dimensions.setX(dimensions.getRelativeX() + percent);
        if (dimensions.getRelativeX() > coord.dimensions.getRelativeX()){
            dimensions.setX(coord.dimensions.getRelativeX());
        }
	}
    if (dimensions.getRelativeY() > coord.dimensions.getRelativeY()){
		dimensions.setY(dimensions.getRelativeY() - percent);
        if (dimensions.getRelativeY() < coord.dimensions.getRelativeY()){
            dimensions.setY(coord.dimensions.getRelativeY());
        }
    } else if (dimensions.getRelativeY() < coord.dimensions.getRelativeY()){
		dimensions.setY(dimensions.getRelativeY() + percent);
        if (dimensions.getRelativeY() > coord.dimensions.getRelativeY()){
            dimensions.setY(coord.dimensions.getRelativeY());
        }
	}
}

void Coordinate::center(const Coordinate & coord){
    const double centerx = (coord.getRelativeX1() + coord.getRelativeX2())/2;
    const double centery = (coord.getRelativeY1() + coord.getRelativeY2())/2;
    
    set(centerx,centery,centerx,centery);
}

bool Coordinate::operator==( const Coordinate & coord){
    return ( (position == coord.position) &&
            (dimensions == coord.dimensions));
}

bool Coordinate::operator!=( const Coordinate & coord){
    return ( !(position == coord.position) ||
            !(dimensions == coord.dimensions));
}

bool Coordinate::operator==( const Bitmap & bmp){
    return ( (dimensions.getX() == bmp.getWidth()) &&
            (dimensions.getY() == bmp.getHeight()));
}

bool Coordinate::operator!=( const Bitmap & bmp){
    return ( !(dimensions.getX() == bmp.getWidth()) ||
            !(dimensions.getY() == bmp.getHeight()));
}

void Coordinate::setPosition(const RelativePoint & point){
    position = point;
}
void Coordinate::setPosition(AbsolutePoint point){
    position = RelativePoint(point);
}
void Coordinate::setDimensions(const RelativePoint & point){
    dimensions = point;
}
void Coordinate::setDimensions(AbsolutePoint point){
    dimensions = RelativePoint(point);
}

