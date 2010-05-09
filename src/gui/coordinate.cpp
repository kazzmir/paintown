#include "util/bitmap.h"
#include "coordinate.h"
#include "globals.h"
#include "util/load_exception.h"
#include <sstream>

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

int AbsolutePoint::getX() const {
    return x;
}

int AbsolutePoint::getY() const {
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

RelativePoint::RelativePoint(const AbsolutePoint & point):
x(absoluteToRelative(point.getX(), Global::getScreenWidth()/2)),
y(absoluteToRelative(point.getY(), Global::getScreenHeight()/2)){
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
    return relativeToAbsolute(x, Global::getScreenWidth()/2);
}

int RelativePoint::getY() const{
    return relativeToAbsolute(y, Global::getScreenHeight()/2);
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
Coordinate::Coordinate(AbsolutePoint & position, AbsolutePoint & position2):
position(position),
position2(position2),
z(0),
radius(0){
}
Coordinate::Coordinate(const RelativePoint & position, const RelativePoint & position2):
position(position),
position2(position2),
z(0),
radius(0){
}
Coordinate::Coordinate(const Coordinate & copy):
position(copy.position),
position2(copy.position2),
z(copy.z),
radius(copy.radius){
}
Coordinate::~Coordinate(){
}

const Coordinate & Coordinate::operator=(const Coordinate & copy){
    this->position = copy.position;
    this->position2 = copy.position2;
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
    return position2.getX() - position.getX();
}
int Coordinate::getHeight() const{
    return position2.getY() - position.getY();
}
int Coordinate::getX2() const{
    return position2.getX();
}
int Coordinate::getY2() const{
    return position2.getY();
}

void Coordinate::growHorizontal(double by){
    position.setX(position.getRelativeX() - by);
    position2.setX(position2.getRelativeX() + by);
}

void Coordinate::growVertical(double by){
    position.setY(position.getRelativeY() - by);
    position2.setY(position2.getRelativeY() + by);
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
    if (position2.getRelativeX() > coord.position2.getRelativeX()){
		position2.setX(position2.getRelativeX() - percent);
        if (position2.getRelativeX() < coord.position2.getRelativeX()){
            position2.setX(coord.position2.getRelativeX());
        }
    } else if (position2.getRelativeX() < coord.position2.getRelativeX()){
		position2.setX(position2.getRelativeX() + percent);
        if (position2.getRelativeX() > coord.position2.getRelativeX()){
            position2.setX(coord.position2.getRelativeX());
        }
	}
    if (position2.getRelativeY() > coord.position2.getRelativeY()){
		position2.setY(position2.getRelativeY() - percent);
        if (position2.getRelativeY() < coord.position2.getRelativeY()){
            position2.setY(coord.position2.getRelativeY());
        }
    } else if (position2.getRelativeY() < coord.position2.getRelativeY()){
		position2.setY(position2.getRelativeY() + percent);
        if (position2.getRelativeY() > coord.position2.getRelativeY()){
            position2.setY(coord.position2.getRelativeY());
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
            (position2 == coord.position2));
}

bool Coordinate::operator!=( const Coordinate & coord){
    return ( !(position == coord.position) ||
            !(position2 == coord.position2));
}

bool Coordinate::operator==( const Bitmap & bmp){
    return ( (getWidth() == bmp.getWidth()) &&
            (getHeight() == bmp.getHeight()));
}

bool Coordinate::operator!=( const Bitmap & bmp){
    return !(*this == bmp);
}

void Coordinate::setPosition(const RelativePoint & point){
    position = point;
}

void Coordinate::setPosition(const AbsolutePoint & point){
    position = RelativePoint(point);
}

void Coordinate::setPosition2(const RelativePoint & point){
    position2 = point;
}

void Coordinate::setPosition2(const AbsolutePoint & point){
    position2 = RelativePoint(point);
}

void Coordinate::checkDimensions(){
    if (getWidth() < 0 || getHeight() < 0){
        std::ostringstream out;
        out << "Cannot have a negative coordinate dimension " << getWidth() << ", " << getHeight();
        throw LoadException();
    }
}

void Coordinate::setDimensions(int width, int height){
    position2 = RelativePoint(AbsolutePoint(getX() + width, getY() + height));
}
