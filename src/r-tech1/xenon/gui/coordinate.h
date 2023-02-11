#ifndef _rtech1_gui_coordinate_h
#define _rtech1_gui_coordinate_h

namespace Graphics{
class Bitmap;
}

namespace Gui {

namespace Space{

class Space;
/* a 2-d point */
class Point{
public:
    Point(double x, double y, const Space & space);
    Point(const Point & point, const Space & space);
    Point(const Point & point);

    Point operator+(const Point &);
    Point & operator+=(const Point &);
    Point & operator=(const Point &);
    bool operator==(const Point &) const;
    bool operator!=(const Point &) const;

    /* convert to physical space */
    double physicalX() const;
    double physicalY() const;

    double x;
    double y;
    const Space & space;
private:
    /* true if points use the same space system */
    bool sameSpace(const Point & point) const;
};

/* mapping from coordinate space to physical space */
class Space{
public:
    Space(double minX, double minY, double maxX, double maxY);
    Point fromPhysical(int x, int y);
    Point fromLocal(double x, double y);

    bool operator==(const Space & space) const;

    /* clamp the values to the size of the space */
    double boundX(double x) const;
    double boundY(double y) const;

    double getMinimumX() const;
    double getMinimumY() const;

    double sizeX() const;
    double sizeY() const;
    double centerX() const;
    double centerY() const;
    double getLocalX(double physicalX) const;
    double getLocalY(double physicalY) const;
private:
    double minX, minY, maxX, maxY;
};

}

/* Coordinate system for handling scalable graphics */

class AbsolutePoint {
    public:
        AbsolutePoint();
        explicit AbsolutePoint(int x, int y);
        AbsolutePoint(const AbsolutePoint &);
        virtual ~AbsolutePoint();
        
        const AbsolutePoint & operator=(const AbsolutePoint &);
        virtual inline void setX(int x){
            this->x = x;
        }
        virtual int getX() const;
        virtual inline void setY(int y){
            this->y = y;
        }
        virtual int getY() const;
    private:
        int x;
        int y;
};

class RelativePoint {
    public:
        RelativePoint();
        explicit RelativePoint(double x, double y);
        RelativePoint(const RelativePoint &);
        RelativePoint(const AbsolutePoint &);
        virtual ~RelativePoint();
        
        const RelativePoint & operator=(const RelativePoint &);
        const RelativePoint & operator=(const AbsolutePoint &);
        bool operator==(const RelativePoint &);
        bool operator!=(const RelativePoint &);
        virtual inline void setX(double x){
            this->x = x;
        }
        virtual int getX() const;
        virtual inline void setY(double y){
            this->y = y;
        }
        virtual int getY() const;
        virtual inline void set(double x, double y){
            this->x = x;
            this->y = y;
        }
        virtual int getDistanceFromCenterX() const;
        virtual int getDistanceFromCenterY() const;
        virtual void moveX(double percent);
        virtual void moveY(double percent);
        virtual void moveBy(double x, double y);
        virtual AbsolutePoint getAbsolute();
        virtual double getRelativeX() const;
        virtual double getRelativeY() const;
    private:
        double x;
        double y;
};

class Coordinate {
    public:
        Coordinate();
        Coordinate(AbsolutePoint &, AbsolutePoint &);
        Coordinate(const RelativePoint &, const RelativePoint &);
        Coordinate(const Coordinate &);
        virtual ~Coordinate();
        
        const Coordinate & operator=(const Coordinate &);
    
        virtual inline void setZ(double z){
            this->z = z;
        }
        virtual inline double getZ() const{
            return this->z;
        }
        
        virtual int getX() const;
        virtual int getY() const;
        virtual int getWidth() const;
        virtual int getHeight() const;
        virtual int getX2() const;
        virtual int getY2() const;
        /* give a relative quantity */
        virtual void growHorizontal(double by);
        /* give an absolute quantity */
        virtual void growHorizontalAbsolute(int by);
        /* give a relative quantity */
        virtual void growVertical(double by);
        /* give an absolute quantity */
        virtual void growVerticalAbsolute(int by);

        virtual void growTo(const Coordinate &, double percent = 0.005);
        virtual void center(const Coordinate &);
        virtual void moveBy(double x, double y);
        virtual inline double getRelativeX1() const{
            return this->position.getRelativeX();
        }
        virtual inline double getRelativeX2() const{
            return this->position2.getRelativeX();
        }
        virtual inline double getRelativeY1() const{
            return this->position.getRelativeY();
        }
        virtual inline double getRelativeY2() const{
            return this->position2.getRelativeY();
        }

        virtual inline void set(double x1, double y1, double x2, double y2){
            this->position.setX(x1);
            this->position.setY(y1);
            this->position2.setX(x2);
            this->position2.setY(y2);
        }

        virtual inline void setX1(double x){
            this->position.setX(x);
        }
        virtual inline void setX2(double x){
            this->position2.setX(x);
        }
        virtual inline void setY1(double y){
            this->position.setY(y);
        }
        virtual inline void setY2(double y){
            this->position2.setY(y);
        }

        bool operator==( const Coordinate &);
        bool operator!=( const Coordinate &);
        bool operator==( const Graphics::Bitmap &);
        bool operator!=( const Graphics::Bitmap &);

        /* moves the positions so that `center' is in the middle but the
         * width/height is maintained
         */
        virtual void setCenterPosition(const RelativePoint & center);

        virtual void setPosition(const RelativePoint &);
        virtual void setPosition(const AbsolutePoint &);
        virtual void setPosition2(const RelativePoint &);
        virtual void setPosition2(const AbsolutePoint &);

        virtual void moveTo(const AbsolutePoint &);

        virtual void setDimensions(int width, int height);

        virtual inline const RelativePoint & getPosition() const {
            return this->position;
        }

        virtual inline const RelativePoint & getPosition2() const {
            return this->position2;
        }
        
        virtual inline bool empty(){
            return (getWidth() == 0 && getHeight() == 0);
        }
        
    private:
        void checkDimensions();

        RelativePoint position;
        RelativePoint position2;
        double z;
};

}


#endif
