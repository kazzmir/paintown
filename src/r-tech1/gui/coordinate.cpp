#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/gui/coordinate.h"
#include "r-tech1/funcs.h"
#include "r-tech1/exceptions/load_exception.h"
#include <sstream>
#include <math.h>

using namespace Gui;

namespace Gui{
namespace Space{

static double translate(double x, double amount){
    return x + amount;
}

static double scale(double x, double amount){
    return x * amount;
}

/* convert a point in a space with the given width to a new space with the given width
 * 1. translate relative original origin
 * 2. scale it to the range -1, 1
 * 3. scale it to the new width
 * 4. translate relative to the new origin
 */
static double convertSpace(double original, double originalMinimum, double originalWidth, double newMinimum, double newWidth){
    return translate(scale(scale(translate(original, -originalMinimum),
                                 1.0 / originalWidth),
                           newWidth),
                     newMinimum);
}

/* hard coded for now */
static double physicalWidth = 640;
static double physicalHeight = 480;

Point::Point(double x, double y, const Space & space):
x(x),
y(y),
space(space){
}

Point::Point(const Point & point, const Space & space):
space(space){
    x = space.getLocalX(point.physicalX());
    y = space.getLocalY(point.physicalY());
}

Point::Point(const Point & point):
x(point.x),
y(point.y),
space(point.space){
}

static bool close(double a, double b){
    return fabs(a - b) < 0.0001;
}

bool Point::operator==(const Point & him) const {
    if (this == &him){
        return true;
    }

    return close(physicalX(), him.physicalX()) &&
           close(physicalY(), him.physicalY());
}

bool Point::operator!=(const Point & him) const {
    return !(*this == him);
}
    
Point & Point::operator=(const Point & point){
    if (*this == point){
        this->x = point.x;
        this->y = point.y;
    } else {
        this->x = space.getLocalX(point.physicalX());
        this->y = space.getLocalY(point.physicalY());
    }
    return *this;
}

Point & Point::operator+=(const Point & point){
    *this = *this + point;
    return *this;
}
    
Point Point::operator+(const Point & point){
    if (sameSpace(point)){
        x = space.boundX(x + point.x);
        y = space.boundY(y + point.y);
        return *this;
    } else {
        return *this + Point(point, space);
    }
}

bool Point::sameSpace(const Point & point) const {
    return space == point.space;
}

double Point::physicalX() const {
    return convertSpace(x, space.getMinimumX(), space.sizeX(), 0, physicalWidth);
}

double Point::physicalY() const {
    return convertSpace(y, space.getMinimumY(), space.sizeY(), 0, physicalHeight);
}

Space::Space(double minX, double minY, double maxX, double maxY):
minX(minX),
minY(minY),
maxX(maxX),
maxY(maxY){
}
    
bool Space::operator==(const Space & space) const {
    if (this == &space){
        return true;
    }

    return minX == space.minX &&
           minY == space.minY &&
           maxX == space.maxX &&
           maxY == space.maxY;
}

Point Space::fromPhysical(int x, int y){
    return Point(getLocalX(x), getLocalY(y), *this);
}
    
Point Space::fromLocal(double x, double y){
    return Point(boundX(x), boundY(y), *this);
}

double Space::sizeX() const {
    return maxX - minX;
}

double Space::sizeY() const {
    return maxY - minY;
}
    
double Space::boundX(double x) const {
    return Util::clamp(x, minX, maxX);
}

double Space::boundY(double y) const {
    return Util::clamp(y, minY, maxY);
}

double Space::centerX() const {
    return (maxX - minX) / 2;
}

double Space::centerY() const {
    return (maxY - minY) / 2;
}

double Space::getLocalX(double physicalX) const {
    return convertSpace(physicalX, 0, physicalWidth, minX, sizeX());
}

double Space::getLocalY(double physicalY) const {
    return convertSpace(physicalY, 0, physicalHeight, minY, sizeY());
}

double Space::getMinimumX() const {
    return minX;
}

double Space::getMinimumY() const {
    return minY;
}

}
}

static int relativeToAbsolute(double x, int center){
    return (int)(center + (center * x));
}

static int amountFromCenterX(int x){
    return x - (640 / 2);
}

static int amountFromCenterY(int y){
    return y - (480 / 2);
}

/* scale an absolute distance to a relative distance
 * distance / absolute = X / relative
 * absolute space = 0 to 640
 * relative space = -1 to 1
 * X = distance * relative / absolute
 */
static double scaleAbsoluteToRelativeDistanceX(int distance){
    return distance * (1.0 - (-1.0)) / (640.0 - 0); 
}

static double scaleAbsoluteToRelativeDistanceY(int distance){
    return distance * (1.0 - (-1.0)) / (480.0 - 0); 
}

/* convert a point in absolute space to relative space */
static double absoluteToRelative(int x, int center){
    return (double)(x-center)/center;
}

static double relativeToAbsoluteX(double x){
    // return relativeToAbsolute(x, Global::getScreenWidth() / 2);
    return relativeToAbsolute(x, 640 / 2);
}

static double relativeToAbsoluteY(double y){
    // return relativeToAbsolute(y, Global::getScreenHeight() / 2);
    return relativeToAbsolute(y, 480 / 2);
}

static double absoluteToRelativeX(int x){
    return absoluteToRelative(x, 640/2);
}

static double absoluteToRelativeY(int y){
    return absoluteToRelative(y, 480/2);
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

RelativePoint::RelativePoint(const AbsolutePoint & point):
x(absoluteToRelativeX(point.getX())),
y(absoluteToRelativeY(point.getY())){
}

RelativePoint::~RelativePoint(){
}

const RelativePoint & RelativePoint::operator=(const RelativePoint & copy){
    this->x = copy.x;
    this->y = copy.y;
    return *this;
}
const RelativePoint & RelativePoint::operator=(const AbsolutePoint & point){
    this->x = absoluteToRelativeX(point.getX());
    this->y = absoluteToRelativeY(point.getY());
    return *this;
}
bool RelativePoint::operator==(const RelativePoint & point){
    return (this->x == point.x && this->y == point.y);
}
bool RelativePoint::operator!=(const RelativePoint & point){
    return !(*this == point);
}

int RelativePoint::getX() const{
    return relativeToAbsoluteX(x);
}

int RelativePoint::getY() const{
    return relativeToAbsoluteY(y);
}

int RelativePoint::getDistanceFromCenterX() const {
    return amountFromCenterX(getX()); 
}

int RelativePoint::getDistanceFromCenterY() const {
    return amountFromCenterY(getY());
}

void RelativePoint::moveX(double percent){
    x += percent;
}

void RelativePoint::moveY(double percent){
    y += percent;
}

void RelativePoint::moveBy(double x, double y){
    moveX(x);
    moveY(y);
}

AbsolutePoint RelativePoint::getAbsolute(){
    return AbsolutePoint(relativeToAbsoluteX(x), relativeToAbsoluteY(y));
}

double RelativePoint::getRelativeX() const{
    return x;
}

double RelativePoint::getRelativeY() const{
    return y;
}

Coordinate::Coordinate():
z(0){
}

Coordinate::Coordinate(AbsolutePoint & position, AbsolutePoint & position2):
position(position),
position2(position2),
z(0){
}

Coordinate::Coordinate(const RelativePoint & position, const RelativePoint & position2):
position(position),
position2(position2),
z(0){
}

Coordinate::Coordinate(const Coordinate & copy):
position(copy.position),
position2(copy.position2),
z(copy.z){
}

Coordinate::~Coordinate(){
}

const Coordinate & Coordinate::operator=(const Coordinate & copy){
    this->position = copy.position;
    this->position2 = copy.position2;
    this->z = copy.z;
    
    return *this;
}

int Coordinate::getX() const{
    return position.getX();
}
int Coordinate::getY() const{
    return position.getY();
}

int Coordinate::getWidth() const {
    return position2.getX() - position.getX();
}

int Coordinate::getHeight() const {
    return position2.getY() - position.getY();
}

int Coordinate::getX2() const{
    return position2.getX();
}
int Coordinate::getY2() const{
    return position2.getY();
}

void Coordinate::growHorizontal(double by){
    position.moveX(-(by));
    position2.moveX(by);
}

void Coordinate::growHorizontalAbsolute(int distance){
    growHorizontal(scaleAbsoluteToRelativeDistanceX(distance));
}

void Coordinate::growVerticalAbsolute(int distance){
    growVertical(scaleAbsoluteToRelativeDistanceY(distance));
}

void Coordinate::growVertical(double by){
    position.moveY(-(by));
    position2.moveY(by);
}

void Coordinate::growTo(const Coordinate & coord, double percent){
    if (position.getRelativeX() > coord.position.getRelativeX()){
        position.moveX(-(percent));
        if (position.getRelativeX() < coord.position.getRelativeX()){
            position.setX(coord.position.getRelativeX());
        }
    } else if (position.getRelativeX() < coord.position.getRelativeX()){
        position.moveX(percent);
        if (position.getRelativeX() > coord.position.getRelativeX()){
            position.setX(coord.position.getRelativeX());
        }
    }
    if (position.getRelativeY() > coord.position.getRelativeY()){
        position.moveY(-(percent));
        if (position.getRelativeY() < coord.position.getRelativeY()){
            position.setY(coord.position.getRelativeY());
        }
    } else if (position.getRelativeY() < coord.position.getRelativeY()){
        position.moveY(percent);
        if (position.getRelativeY() > coord.position.getRelativeY()){
            position.setY(coord.position.getRelativeY());
        }
    }
    if (position2.getRelativeX() > coord.position2.getRelativeX()){
        position2.moveX(-(percent));
        if (position2.getRelativeX() < coord.position2.getRelativeX()){
            position2.setX(coord.position2.getRelativeX());
        }
    } else if (position2.getRelativeX() < coord.position2.getRelativeX()){
        position2.moveX(percent);
        if (position2.getRelativeX() > coord.position2.getRelativeX()){
            position2.setX(coord.position2.getRelativeX());
        }
    }
    if (position2.getRelativeY() > coord.position2.getRelativeY()){
        position2.moveY(-(percent));
        if (position2.getRelativeY() < coord.position2.getRelativeY()){
            position2.setY(coord.position2.getRelativeY());
        }
    } else if (position2.getRelativeY() < coord.position2.getRelativeY()){
        position2.moveY(percent);
        if (position2.getRelativeY() > coord.position2.getRelativeY()){
            position2.setY(coord.position2.getRelativeY());
        }
    }
}

void Coordinate::center(const Coordinate & coord){
    const double centerx = (coord.getRelativeX1() + coord.getRelativeX2())/2;
    const double centery = (coord.getRelativeY1() + coord.getRelativeY2())/2;
    
    set(centerx, centery, centerx, centery);
}

void Coordinate::moveBy(double x, double y){
    position.moveBy(x, y);
    position2.moveBy(x, y);
}

bool Coordinate::operator==( const Coordinate & coord){
    return ( (position == coord.position) &&
            (position2 == coord.position2));
}

bool Coordinate::operator!=( const Coordinate & coord){
    return ( !(position == coord.position) ||
            !(position2 == coord.position2));
}

bool Coordinate::operator==( const Graphics::Bitmap & bmp){
    return ( (getWidth() == bmp.getWidth()) &&
            (getHeight() == bmp.getHeight()));
}

bool Coordinate::operator!=( const Graphics::Bitmap & bmp){
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
        
void Coordinate::moveTo(const AbsolutePoint & where){
    double dx = position2.getRelativeX() - position.getRelativeX();
    double dy = position2.getRelativeY() - position.getRelativeY();
    setPosition(where);

    /* sort of ugly, use operator+ for RelativePosition */
    setPosition2(RelativePoint(position.getRelativeX() + dx, position.getRelativeY() + dy));
}

void Coordinate::checkDimensions(){
    if (getWidth() < 0 || getHeight() < 0){
        std::ostringstream out;
        out << "Cannot have a negative coordinate dimension " << getWidth() << ", " << getHeight();
        throw LoadException(__FILE__, __LINE__, out.str());
    }
}

void Coordinate::setDimensions(int width, int height){
    position2 = RelativePoint(AbsolutePoint(getX() + width, getY() + height));
}
        
void Coordinate::setCenterPosition(const RelativePoint & center){
    int width = getWidth();
    int height = getHeight();
    this->position = center;
    this->position2 = center;
    growHorizontalAbsolute(width / 2.0);
    growVerticalAbsolute(height / 2.0);
}
