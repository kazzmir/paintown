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
        virtual int getX() const;
        virtual inline void setY(double y){
            this->y = y;
        }
        virtual int getY() const;
        virtual inline void set(double x, double y){
            this->x = x;
            this->y = y;
        }
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
        virtual inline void setRadius(double radius){
            this->radius = radius;
        }
        virtual inline double getRadius() const{
            return this->radius;
        }
        
        virtual int getX() const;
        virtual int getY() const;
        virtual int getWidth() const;
        virtual int getHeight() const;
        virtual int getX2() const;
        virtual int getY2() const;
        virtual void growHorizontal(double by);
        virtual void growVertical(double by);
        virtual void growTo(const Coordinate &, double percent = 0.005);
        virtual void center(const Coordinate &);
        virtual inline double getRelativeX1() const{
            return this->position.getRelativeX();
        }
        virtual inline double getRelativeX2() const{
            return this->dimensions.getRelativeX();
        }
        virtual inline double getRelativeY1() const{
            return this->position.getRelativeY();
        }
        virtual inline double getRelativeY2() const{
            return this->dimensions.getRelativeY();
        }
        virtual inline void set(double x1, double y1, double x2, double y2){
            this->position.setX(x1);
            this->position.setY(y1);
            this->dimensions.setX(x2);
            this->dimensions.setY(y2);
        }
        virtual inline void setX1(double x){
            this->position.setX(x);
        }
        virtual inline void setX2(double x){
            this->dimensions.setX(x);
        }
        virtual inline void setY1(double y){
            this->position.setY(y);
        }
        virtual inline void setY2(double y){
            this->dimensions.setY(y);
        }

        bool operator==( const Coordinate &);
        bool operator!=( const Coordinate &);
        bool operator==( const Bitmap &);
        bool operator!=( const Bitmap &);

        virtual void setPosition(const RelativePoint &);
        virtual void setPosition(AbsolutePoint);
        virtual void setDimensions(const RelativePoint &);
        virtual void setDimensions(AbsolutePoint);
        
        virtual inline RelativePoint & getPosition(){
            return this->position;
        }
        virtual inline RelativePoint & getDimensions(){
            return this->dimensions;
        }
        
        virtual inline bool empty(){
            return (dimensions.getRelativeX() == -1 && dimensions.getRelativeY() == -1);
        }
        
    private:
        RelativePoint position;
        RelativePoint dimensions;
        double z;
        double radius;
};

}


#endif
