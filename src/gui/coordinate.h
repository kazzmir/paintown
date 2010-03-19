#ifndef _paintown_gui_coordinate
#define _paintown_gui_coordinate

namespace Gui {

/* Coordinate system for handling scalable graphics */

class Coordinate {
    public:
        Coordinate();
        virtual ~Coordinate();

        virtual setAbsoluteX(int x);
        virtual setAbsoluteY(int y);
        virtual setAbsoluteZ(int z);
        virtual setAbsoluteWidth(int width);
        virtual setAbsoluteHeight(int height);
        virtual setAbsoluteRadius(int radius);
        
        virtual setRelativeX(double x);
        virtual setRelativeY(double y);
        virtual setRelativeZ(double z);
        virtual setRelativeWidth(double width);
        virtual setRelativeHeight(double height);
        virtual setRelativeRadius(double radius);

        virtual int getAbsoluteX();
        virtual int getAbsoluteY();
        virtual int getAbsoluteX2();
        virtual int getAbsoluteY2();
        virtual int getAbsoluteZ();
        virtual int getAbsoluteWidth();
        virtual int getAbsoluteHeight();
        virtual int getAbsouluteRadius();

        virtual inline double getX() {
            return this->getRelativeX();
        }
        virtual inline double getY() {
            return this->getRelativeY();
        }
        virtual inline double getZ() {
            return this->getRelativeZ();
        }
        virtual inline double getX2() {
            return this->getRelativeX2();
        }
        virtual inline double getY2() {
            return this->getRelativeY2();
        }
        virtual inline double getWidth() {
            return this->getRelativeWidth();
        }
        virtual inline double getHeight() {
            return this->getRelativeHeight();
        }
        virtual inline double getRadius() {
            return this->getRelativeRadius();
        }

        virtual double getRelativeX();
        virtual double getRelativeY();
        virtual double getRelativeZ();
        virtual double getRelativeX2();
        virtual double getRelativeY2();
        virtual double getRelativeWidth();
        virtual double getRelativeHeight();
        virtual double getRelativeRadius();

    private:
        double x;
        double y;
        double z;
        double width;
        double height;
        double radius;
};

}


#endif
