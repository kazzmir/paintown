#include "util/bitmap.h"
#include "widget.h"
#include <math.h>
#include "globals.h"
#include "util/token.h"
#include "coordinate.h"
#include "util/load_exception.h"
#include <sstream>

using namespace Gui;

static const double S_PI = 3.14159265358979323846;
static const double DEG_TO_RAD = 180.0/S_PI;
static const double RAD_TO_DEG = S_PI/180.0;

static void round_double (double dbl_to_round , int &rounded_num) {
    rounded_num = static_cast<int>(dbl_to_round);
    if ((dbl_to_round - static_cast<double>(rounded_num)) >= 0.5) {rounded_num++;}
}

//! min (borrowed from allegro)
static inline int Min(int x, int y){ return (((x) < (y)) ? (x) : (y)); }

//! max (borrowed from allegro)
static inline int Max(int x, int y){ return (((x) > (y)) ? (x) : (y)); }

//! mid (borrowed from allegro)
static inline int Mid(int x,int y,int z){ return (Max((x), Min((y), (z)))); }

Widget::Widget() : workArea(0)
{
	// Nothing yet
}
		
Widget::Widget( const Widget & w ){
    this->location = w.location;
    this->workArea = w.workArea;
}

Widget::~Widget(){
	if ( workArea ){
		delete workArea;
	}
}

// copy
Widget &Widget::operator=( const Widget &copy){
    location = copy.location;
    workArea = copy.workArea;

    return *this;
}

void Widget::setCoordinates(const Token * token){
    if ( *token == "position" ){
        int x, y, width, height;
        token->view() >> x >> y >> width >> height;
        AbsolutePoint pos(x, y);
        AbsolutePoint dimensions(x + width, y + height);
        location.setPosition(pos);
        location.setPosition2(dimensions);
    } else if ( *token == "relative-position" ){
        double x1, y1, x2, y2;
        token->view() >> x1 >> y1 >> x2 >> y2;
        RelativePoint pos(x1,y1);
        RelativePoint dimensions(x2,y2);
        location = Coordinate(pos, dimensions);
    } else if ( *token == "coordinate" ){
        TokenView view = token->view();
        while (view.hasMore()){
            const Token * coordToken;
            view >> coordToken;
            if (*coordToken == "absolute"){
                int x, y, width, height;
                coordToken->view() >> x >> y >> width >> height;
                AbsolutePoint pos(x, y);
                AbsolutePoint dimensions(x + width, y + height);
                location = Coordinate(pos, dimensions);
            } else if (*coordToken == "relative"){
                double x1, y1, x2, y2;
                coordToken->view() >> x1 >> y1 >> x2 >> y2;
                RelativePoint pos(x1,y1);
                RelativePoint dimensions(x2,y2);
                location = Coordinate(pos, dimensions);
            } else if (*coordToken == "radius"){
                double radius;
                coordToken->view() >> radius;
                location.setRadius(radius);
            } else if (*coordToken == "z"){
                double z;
                coordToken->view() >> z;
                location.setZ(z);
            }
        }
    }

    if (location.getWidth() < 0 || location.getHeight() < 0){
        std::ostringstream out;
        out << "Invalid location dimension (cannot have a negative size). Width " << location.getWidth() << " Height " << location.getHeight() << ". From token: ";
        token->toString(out, "");
        throw LoadException(__FILE__, __LINE__, out.str());
    }
}

void Widget::setColors(const Token * token){
    if ( *token == "position-body" ) {
        // This handles the body color of the widget
        int r,g,b;
        token->view() >> r >> g >> b >> colors.bodyAlpha;
        colors.body = Bitmap::makeColor(r,g,b);
    } else if ( *token == "position-border" ) {
        // This handles the border color of the widget
        int r,g,b;
        token->view() >> r >> g >> b >> colors.borderAlpha;
        colors.border = Bitmap::makeColor(r,g,b);
    } 
}
        
void Widget::render(const Bitmap & bitmap, const Font & font){
    render(bitmap);
}

void Widget::arc( const Bitmap & work, int x, int y, double startAngle, int radius, int color ){
    int q = 0;// for counters
    double d_q = 0.0;// for percentage of loop completed
    double d_q_plus_one = 0.0;

    const double d_angular_length_deg = 0.030;
    const double d_start_angle_rad = startAngle*DEG_TO_RAD;
    const double d_angular_length_rad = d_angular_length_deg*DEG_TO_RAD;
    const double d_arc_distance_between_points = 0.25*pow(2.0 , static_cast<double>(10) / 10.0);
    double d_angular_length_rad_per_segment = 0.0;

    double arc_length = radius*d_angular_length_rad;
    if (arc_length < 0.0) {arc_length *= -1.0;}
    const double d_num_segments = arc_length / d_arc_distance_between_points;

    int num_segments = 0;
    round_double(d_num_segments , num_segments);

    if (num_segments == 0) {num_segments += 1;} // need at least one segment (two points)
    const int num_points = num_segments + 1;
    const double d_num_points_minus_one = static_cast<double>(num_points - 1);

    int arc_point_x;//[num_points];
    int arc_point_y;//[num_points];
    int arc_point2_x;//[num_points];
    int arc_point2_y;//[num_points];
    double d_arc_point_x = 0.0;
    double d_arc_point_y = 0.0;
    double d_arc_point2_x = 0.0;
    double d_arc_point2_y = 0.0;

    double current_angle_rad = 0.0;
    double current_angle2_rad = 0.0;

    if (d_arc_distance_between_points <= 1.0) {
        for (q = 0 ; q < num_points ; q++) {
            d_q = static_cast<double>(q);
            current_angle_rad = d_start_angle_rad + (d_q / d_num_points_minus_one)*d_angular_length_rad;
            d_arc_point_x = x + radius*cos(current_angle_rad);
            d_arc_point_y = y + radius*sin(current_angle_rad);

            round_double(d_arc_point_x , arc_point_x);
            round_double(d_arc_point_y , arc_point_y);

            work.putPixel(arc_point_x,arc_point_y,color);
        }
    }
    if (d_arc_distance_between_points > 1.0) {

        d_angular_length_rad_per_segment = d_angular_length_rad / d_num_points_minus_one;
        for (q = 0 ; q < num_segments ; q++) {
            d_q = static_cast<double>(q);
            d_q_plus_one = static_cast<double>(q + 1);

            current_angle_rad = d_start_angle_rad + d_q*d_angular_length_rad_per_segment;
            current_angle2_rad = d_start_angle_rad + d_q_plus_one*d_angular_length_rad_per_segment;

            d_arc_point_x = x + radius*cos(current_angle_rad);
            d_arc_point_y = y + radius*sin(current_angle_rad);

            round_double(d_arc_point_x , arc_point_x);
            round_double(d_arc_point_y , arc_point_y);

            d_arc_point2_x = x + radius*cos(current_angle2_rad);
            d_arc_point2_y = y + radius*sin(current_angle2_rad);

            round_double(d_arc_point2_x , arc_point2_x);
            round_double(d_arc_point2_y , arc_point2_y);

            work.line(arc_point_x,arc_point_y, arc_point2_x, arc_point2_y,color);
        }
    }
}

void Widget::roundRect( const Bitmap & work, int radius, int x1, int y1, int x2, int y2, int color ){
    const int width = x2 - x1;
    const int height = y2 - y1;
    radius = Mid(0, radius, Min((x1+width - x1)/2, (y1+height - y1)/2));
    work.line(x1+radius, y1, x1+width-radius, y1, color);
    work.line(x1+radius, y1+height, x1+width-radius,y1+height, color);
    work.line(x1, y1+radius,x1, y1+height-radius, color);
    work.line(x1+width, y1+radius,x1+width, y1+height-radius, color);

    arc(work, x1+radius, y1+radius, S_PI-1.115, radius, color);
    arc(work, x1+radius + (width - radius *2), y1+radius, -S_PI/2 +0.116, radius, color);
    arc(work, x1+width-radius, y1+height-radius, -0.110, radius ,color);
    arc(work, x1+radius, y1+height-radius, S_PI/2-0.119, radius, color);
}

void Widget::roundRectFill( const Bitmap & work, int radius, int x1, int y1, int x2, int y2, int color ){
    const int width = x2 - x1;
    const int height = y2 - y1;
    radius = Mid(0, radius, Min((x1+width - x1)/2, (y1+height - y1)/2));
    work.circleFill(x1+radius, y1+radius, radius, color);
    work.circleFill((x1+width)-radius, y1+radius, radius, color);
    work.circleFill(x1+radius, (y1+height)-radius, radius, color);
    work.circleFill((x1+width)-radius, (y1+height)-radius, radius, color);
    work.rectangleFill( x1+radius, y1, x2-radius, y1+radius, color);
    work.rectangleFill( x1, y1+radius, x2, y2-radius, color);
    work.rectangleFill( x1+radius, y2-radius, x2-radius, y2, color);
}

static void foobar(int a, int b){
    a = a + b;
}

void Widget::checkWorkArea(){
    /*
    if (location.getWidth() < 0 || location.getHeight() < 0){
        foobar(location.getWidth(), location.getHeight());
    }
    */

    if ( ! workArea ){
        workArea = new Bitmap(location.getWidth(), location.getHeight());
    } else if (location.getWidth() < workArea->getWidth() || location.getHeight() < workArea->getHeight()){
        delete workArea;
        workArea = new Bitmap(location.getWidth(), location.getHeight());
    } else if (location.getWidth() > workArea->getWidth() || location.getHeight() > workArea->getHeight()){
        delete workArea;
        workArea = new Bitmap(location.getWidth(), location.getHeight());
    }

    if (workArea){
        workArea->fill(Bitmap::makeColor(255,0,255));
    }
}
