#ifndef _paintown_gui_coordinate_h
#define _paintown_gui_coordinate_h

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
        Coordinate(const AbsolutePoint &, const AbsolutePoint &);
        Coordinate(const RelativePoint &, const RelativePoint &);
        virtual ~Coordinate();
    
        virtual void setZ(double z);
        virtual inline double getZ(){
            return this->z;
        }
        virtual void setRadius(double radius);
        virtual inline double getRadius(){
            return this->radius;
        }
        
        virtual int getX();
        virtual int getY();
        virtual int getWidth();
        virtual int getHeight();
        
        virtual inline const RelativePoint & getPosition(){
            return this->position;
        }
        virtual inline const RelativePoint & getDimensions(){
            return this->dimensions;
        }
        
    private:
        RelativePoint position;
        RelativePoint dimensions;
        double z;
        double radius;
};

}


#endif
