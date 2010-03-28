#ifndef _paintown_gui_coordinate_h
#define _paintown_gui_coordinate_h

class Bitmap;

namespace Gui {

/* Coordinate system for handling scalable graphics */

class AbsolutePoint {
    public:
        AbsolutePoint();
        AbsolutePoint(int x, int y);
        AbsolutePoint(const AbsolutePoint &);
        virtual ~AbsolutePoint();
        
        const AbsolutePoint & operator=(const AbsolutePoint &);
        virtual inline void setX(int x){
            this->x = x;
        }
        virtual int getX();
        virtual inline void setY(int y){
            this->y = y;
        }
        virtual int getY();
    private:
        int x;
        int y;
};

class RelativePoint {
    public:
        RelativePoint();
        RelativePoint(double x, double y);
        RelativePoint(const RelativePoint &);
        RelativePoint(AbsolutePoint &);
        virtual ~RelativePoint();
        
        const RelativePoint & operator=(const RelativePoint &);
        const RelativePoint & operator=(AbsolutePoint &);
        bool operator==(const RelativePoint &);
        bool operator!=(const RelativePoint &);
        virtual inline void setX(double x){
            this->x = x;
        }
        virtual int getX();
        virtual inline void setY(double y){
            this->y = y;
        }
        virtual int getY();
        virtual AbsolutePoint getAbsolute();
        virtual double getRelativeX();
        virtual double getRelativeY();
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
        virtual inline double getZ(){
            return this->z;
        }
        virtual inline void setRadius(double radius){
            this->radius = radius;
        }
        virtual inline double getRadius(){
            return this->radius;
        }
        
        virtual int getX();
        virtual int getY();
        virtual int getWidth();
        virtual int getHeight();
        virtual int getX2();
        virtual int getY2();
        bool operator==( const Coordinate &);
        bool operator!=( const Coordinate &);
        bool operator==( const Bitmap &);
        bool operator!=( const Bitmap &);
        
        virtual inline RelativePoint & getPosition(){
            return this->position;
        }
        virtual inline RelativePoint & getDimensions(){
            return this->dimensions;
        }
        
        virtual inline bool empty(){
            return (dimensions.getX() == 0 && dimensions.getY() == 0);
        }
        
    private:
        RelativePoint position;
        RelativePoint dimensions;
        double z;
        double radius;
};

}


#endif
