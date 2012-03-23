/*************************************************************************

Missle Defend v. 1.0
for Allegro Games Mini Collection 2 (by Miran)

Written by Miguel A. Gavidia (juvinious) on 16 March 2006

Do not try to make profit from this or you will surely die.

**************************************************************************/

#include "game.h"

#include "globals.h"
#include "configuration.h"
#include "util/bitmap.h"
#include "util/stretch-bitmap.h"
#include "util/trans-bitmap.h"
#include "util/debug.h"
#include "util/pointer.h"
#include "util/system.h"
#include "util/input/input-map.h"
#include "util/input/input-source.h"
#include "util/input/input-manager.h"
#include "util/input/keyboard.h"
#include "util/file-system.h"
#include "util/music.h"
#include "util/sound.h"
#include "util/events.h"
#include "util/init.h"

#include <math.h>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <queue>
#include <exception>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265
#endif

#ifndef TWO_PI
#define TWO_PI (M_PI*2)
#endif

#define WHITE       Graphics::makeColor(255,255,255)
#define BLACK       Graphics::makeColor(0,0,0)
#define BLUE        Graphics::makeColor(0,0,255)
#define GREEN       Graphics::makeColor(0,255,0)
#define RED     Graphics::makeColor(255,0,0)
#define YELLOW      Graphics::makeColor(255,255,0)
#define TEAL        Graphics::makeColor(0,255,255)
#define PINK        Graphics::makeColor(255,0,255)
#define PURPLE      Graphics::makeColor(150,0,150)
#define GREY        Graphics::makeColor(150,150,150)
#define RANDOMCOLOR Graphics::makeColor(rand()%255,rand()%255,rand()%255)

namespace MissileDefend{
    
const int GFX_X = 640;
const int GFX_Y = 480;

//! FIXME get rid of this crap when everything is working
static Graphics::Bitmap BLANK_BITMAP(50,50);

//! Game settings
/*GameSettings mdefendSettings[] =
{
    { "Particles", "Particles", "10|20|30|50|80|120|200|400|600|1000|2000", 10 },
    { "Volume", "Volume", "OFF|LOW|MID|HIGH", 2 },  
    { "Bonus" , "Bonus", "500|1000|1500|2000|2500|3000", 2 },
    { 0,0,0,0 }
};*/

//! Graphics throttle
int maxGfxThrottle;
bool toggleGfxThrottle;
int maxVolume;

#if 0
// Global timer
volatile int counter;       
void timeCounter()
{
    ++counter;
}
END_OF_FUNCTION(timeCounter);
#endif

//! Angle routines
static double getAngle(double x1, double y1, double x2, double y2)
{
    double angle;
    /* Get the angle between the two points. */
    angle = atan2 ((y2 - y1), (x2 - x1));
    
    return angle;//(ftofix(angle*128/M_PI+64.0)& 0x00FFFFFF);
}

static bool moveDirection(double &x1, double &y1, double x2, double y2, double velocity)
{
    double angle;
    
    angle = atan2 ((y2 - y1), (x2 - x1));
    
    x1 += velocity*cos(angle);
    y1 += velocity*sin(angle);
    
    //If the object is within a 5 point radius of the target area return true
    if(x1 > x2+5)return true;
    if(x1+5 < x2)return true;
    if(y1 > y2+5)return true;
    if(y1+5 < y2)return true;
    
    return false;
}


class Position{
public:
    Position(){
    }
    Position(int setX, int setY){
        otherX=setX;
        otherY=setY;
    }
    
    ~Position(){
    }
    
    bool checkCollision(const Position  & obj){
        if(x > obj.getX2()){
            return false;
        }
        if(getX2() < obj.x){
            return false;
        }
        if(y > obj.getY2()){
            return false;
        }
        if(getY2() < obj.y){
            return false;
        }
        return true;
    }
    
    void setX(int x){
        this->x = x;
    }
    
    int getX() const {
        return x;
    }
    
    void setY(int y){
        this->y = y;
    }
    
    int getY() const {
        return y;
    }
    
    void setWidth(int width){
        this->width = width;
    }
    
    int getX2() const {
        return x + width;
    }
    
    void setHeight(int height){
        this->height = height;
    }
    
    int getY2() const {
        return y + height;
    }
    
    int getOtherX() const {
        return otherX;
    }
    
    int getOtherY() const {
        return otherY;
    }
    
protected:
    // What the hell is this?
    int otherX;
    int otherY;
    
    int x;
    int y;
    int width;
    int height;
};

/* do_circle:
 *  Helper function for the circle drawing routines. Calculates the points
 *  in a circle of radius r around point x, y, and calls the specified 
 *  routine for each one. The output proc will be passed first a copy of
 *  the bmp parameter, then the x, y point, then a copy of the d parameter
 *  (so putpixel() can be used as the callback).
 */
static void do_circle(const Graphics::Bitmap & bmp, int x, int y, int radius, int d, void (*proc)(const Graphics::Bitmap &, int, int, int)){
   int cx = 0;
   int cy = radius;
   int df = 1 - radius;
   int d_e = 3;
   int d_se = -2 * radius + 5;

   do {
      proc(bmp, x+cx, y+cy, d);

      if (cx){
          proc(bmp, x-cx, y+cy, d);
      }

      if (cy){
        proc(bmp, x+cx, y-cy, d);
      }

      if ((cx) && (cy)){
        proc(bmp, x-cx, y-cy, d);
      }

      if (cx != cy){
        proc(bmp, x+cy, y+cx, d);

        if (cx){
            proc(bmp, x+cy, y-cx, d);
        }

        if (cy){
            proc(bmp, x-cy, y+cx, d);
        }

        if (cx && cy){
            proc(bmp, x-cy, y-cx, d);
        }
      }

      if (df < 0)  {
        df += d_e;
        d_e += 2;
        d_se += 2;
      }
      else {
        df += d_se;
        d_e += 2;
        d_se += 4;
        cy--;
      }
      cx++;
   } while (cx <= cy);
}


vector<Position>getPointData;

void getPoint(const Graphics::Bitmap & bmp, int x, int y, int)
{
    getPointData.push_back(Position(x,y));
}

void getCirclePoints(double x, double y, double radius, vector<Position> & pos)
{
    getPointData.clear();
    do_circle(NULL,(int)x,(int)y,(int)radius,0,getPoint);
    pos = getPointData;
}

//! Timer class for managing timed events
class Timer
{
public:
    Timer(){
        startTime = System::currentSeconds();
    }
    Timer(int seconds){
        startTime = System::currentSeconds();
        expireTime = seconds + startTime;
    }
    ~Timer(){
    }
    
    bool expired(){
        if(System::currentSeconds()>=expireTime){
            return true;
        }
        return false;
    }
    
    int getElapsed(){
        return (System::currentSeconds() - startTime);
    }
    
    int getRemaining(){
        return (expireTime - System::currentSeconds());
    }
    
    enum Type{
        Seconds=0,
        TimerSeconds,
        Minutes,
        Total,
        Elapsed,
        Remaining,
    };
    
    int getTotalTime(Type unit)
    {
        return unitConverter(unit,System::currentSeconds());
    }
    void reset()
    {
        startTime = System::currentSeconds();
    }
    
    void pause(bool set){
        /*if(set){
            remove_int(timeCounter);
        }
        else
        {
            install_int_ex(timeCounter, 1000000);
        }*/
    }
    
    void print(Type type, int x, int y, Graphics::Color color, const Graphics::Bitmap & dest, string msg=""){
        switch(type){
            case (Total):
                printFormat(getTotalTime(Seconds),x,y,color,dest,msg);
                break;
            case (Elapsed):
                printFormat(getElapsed(),x,y,color,dest,msg);
                break;
            case (Remaining):
                printFormat(getRemaining(),x,y,color,dest,msg);
                break;
            default:
                break;
        }
    }
private:
    // Timer for doing time related stuff
    unsigned int startTime;
    unsigned int expireTime;
    
    // Pause related
    int pauseTime;
    
    int unitConverter(Type unit, int s){
        switch(unit){
            case (TimerSeconds):
                return (s-((s / 60)*60));
            case (Seconds):
                return s;
            case (Minutes):
                return (s / 60);
            default:
                break;
        }
        return -1;
    }
    
    void printFormat(int seconds, int x, int y, int color, const Graphics::Bitmap & dest, string msg){
        int secs = unitConverter(TimerSeconds,seconds);
        int mins = unitConverter(Minutes,seconds);
        
        if(secs<10 && mins<10){
            //textprintf_ex(dest,font,x,y,color,-1, (string(msg+"0%d:0%d")).c_str(),mins,secs);
            return;
        }
        if(secs<10){
            //textprintf_ex(dest,font,x,y,color,-1, (string(msg+"%d:0%d")).c_str(),mins,secs);
            return;
        }
        if(mins<10){
            //textprintf_ex(dest,font,x,y,color,-1, (string(msg+"0%d:%d")).c_str(),mins,secs);
            return;
        }
        
        //textprintf_ex(dest,font,x,y,color,-1, (string(msg+"%d:%d")).c_str(),mins,secs);
    }
};

//! Score handler

class scoreHandler
{
    list<int>deniedTypes;
    int pointQueue;
    
    public:
    scoreHandler()
    {
        pointQueue=0;
    }
    ~scoreHandler()
    {
    }
    
    void addBadType(int oType)
    {
        deniedTypes.push_back(oType);
    }
    
    void addPoints(int killedBy, int points)
    {
        for(list<int>::iterator i=deniedTypes.begin();i!=deniedTypes.end();++i)
        {
            if((*i)==killedBy)return;
        }
        pointQueue+=points;
    }
    
    void doPoints(int &pointAdder)
    {
        if(pointQueue>0)
        {
            ++pointAdder;
            --pointQueue;
        }
    }
};

//! Object class

class object
{
    public:
    
    object()
    {
        alive=true;
        points=0;
        velocity=0;
        color=RANDOMCOLOR;
        collidable=true;
    }
    
    virtual ~object()
    {
    }
    // Atributes
    int ID;
    bool alive;
    int points;
    bool collidable;
    double x, y;
    double width, height;
    double velocity, maxVelocity;
    double speedIncrement;
    double angle;
    
    Graphics::Color color;
    double alpha;
    double decayRate;
    
    Graphics::Bitmap bmp;
    
    virtual bool checkCollision(object *obj)
    {
        if(!collidable)return false;
        if(!obj->collidable)return false;
        if(x > (obj->x+obj->width)) return false;
        if((x+width) < obj->x) return false;
        if(y > (obj->y+obj->height)) return false;
        if((y+height) < obj->y) return false;
        return true;
    }
    
    bool checkClick(int mouse_x, int mouse_y)
    {
        if(mouse_x > x+width) return false;
        if(mouse_x < x) return false;
        if(mouse_y > y+height) return false;
        if(mouse_y < y) return false;
        return true;
    }
    
    bool isHover(int mouse_x, int mouse_y)
    {
        if(mouse_x > x+width) return false;
        if(mouse_x < x) return false;
        if(mouse_y > y+height) return false;
        if(mouse_y < y) return false;
        return true;
    }
    
    void doCollisions(vector<object *> & obj, scoreHandler &scoreCheck, bool noSelfKill=false, bool toggleCollidable=false)
    {
        for(unsigned int i=0;i<obj.size();++i)
        {
            if(checkCollision(obj[i]))
            {
                if(!noSelfKill)alive=false;
                obj[i]->alive=false;
                if(toggleCollidable)obj[i]->collidable=false;
                scoreCheck.addPoints(ID,obj[i]->points);
                return;
            }
        }
    }
    
    // Destroy the object
    void destroy()
    {
        delete this;
    }
    
    // Draw the object
    virtual void draw(const Graphics::Bitmap & dest)=0;
    
    // Update the object
    virtual void update(int mouse_x, int mouse_y)=0;
    
    // Use for doing anything before actually setting alive to false
    virtual void kill()=0;
    
    // If the other methods don't suit your needs use this method check the Moam base for an example
    virtual void specialMethod(int type)=0;
    
};

//! Building class

class building : public object
{
    public:
    
    // Constructor
    building(int setID, double setx, double sety,const Graphics::Bitmap & setAliveBmp, const Graphics::Bitmap & setDeadBmp)
    {
        ID = setID;
        x = setx;
        y = sety;
        bmp = setAliveBmp;
        deadBmp = setDeadBmp;
        width=bmp.getWidth();
        height=bmp.getHeight();
        points = 0;
    }
    
    // Destruction!!!!!!!!!!
    ~building()
    {
        
    }
    
    Graphics::Bitmap deadBmp;
    
    void update(int mouse_x, int mouse_y)
    {
    }
    
    void draw(const Graphics::Bitmap & dest)
    {
        if(alive)
        {
            bmp.draw((int)x,(int)y, dest);
        }
        else
        {
            deadBmp.draw((int)x,(int)y, dest);
        }
    }
    
    void kill()
    {
    }
    
    void specialMethod(int type)
    {
    }

};

class missleBase : public object
{
    public:
    
    // Constructor
    missleBase(int setID, double setx, double sety,const Graphics::Bitmap & setAliveBmp, const Graphics::Bitmap & setDeadBmp)
    {
        ID = setID;
        x = setx;
        y = sety;
        bmp = setAliveBmp;
        deadBmp = setDeadBmp;
        width=bmp.getWidth();
        height=bmp.getHeight();
        points=0;
    }
    
    // Destruction!!!!!!!!!!
    ~missleBase()
    {
        
    }
    
    Graphics::Bitmap deadBmp;
    
    void update(int mouse_x, int mouse_y)
    {
    }
    
    void draw(const Graphics::Bitmap & dest)
    {
        if(alive)
        {
            bmp.draw((int)x,(int)y, dest);
        }
        else
        {
            deadBmp.draw((int)x,(int)y, dest);
        }
    }
    
    void kill()
    {
    }
    
    void specialMethod(int type)
    {
    }

};

class turret : public object
{
    public:
    
    // Constructor
    turret(int setID, double setx, double sety,double setangle, const Graphics::Bitmap & setBmp)
    {
        ID = setID;
        x = setx;
        y = sety;
        angle = setangle;
        bmp = setBmp;
        width=bmp.getWidth();
        height=bmp.getHeight();
        points=0;
    }
    
    // Destruction!!!!!!!!!!
    ~turret()
    {
        
    }
    
    void update(int mouse_x, int mouse_y)
    {
        angle = getAngle(x,y,mouse_x,mouse_y);
    }
    
    void draw(const Graphics::Bitmap & dest)
    {
        //if(alive)rotate_sprite(dest,bmp,(int)x,(int)y,angle);
        if(alive)bmp.drawRotate((int)x,(int)y,angle,dest);
    }
    
    void kill()
    {
    }
    
    void specialMethod(int type)
    {
    }
};


class moamBase : public object
{
    public:
    
    //Constructor
    
    moamBase(int setID, double setx, double sety,vector<Graphics::Bitmap> bmpCollection)
    {
        ID = setID;
        x = setx;
        y = sety;
        ticks=0;
        currentFrame=0;
        animForward=false;
        bmpAnim = bmpCollection;
        width=bmpCollection[0].getWidth();
        height=bmpCollection[0].getHeight();
        points=0;
    }
    
    // Destruction!!!!!!!!!!
    ~moamBase()
    {
        
    }
    
    vector<Graphics::Bitmap> bmpAnim;
    int ticks;
    bool animForward;
    unsigned currentFrame;
    
    enum
    {
        SETANIMFRAME=0
    };
    
    void update(int mouse_x, int mouse_y)
    {
        ++ticks;
        if(ticks>30)
        {
            if(animForward)
            {
                if(currentFrame<bmpAnim.size()-1)++currentFrame;
            }
            else 
            {
                if(currentFrame>0)--currentFrame;
            }
            ticks=0;
        }
    }
    
    void draw(const Graphics::Bitmap & dest)
    {
        //draw_sprite(dest,bmpAnim[currentFrame],(int)x,(int)y);
        bmpAnim[currentFrame].draw(int(x), int(y), dest);
    }

    void kill()
    {
    }
    
    void specialMethod(int type)
    {
        switch(type)
        {
            case (SETANIMFRAME):
                animForward=!animForward;
                break;
        }
    }
};

//! Enemy Missle class

class enemyMissle : public object
{
    private:
    
    double gox;
    double goy;
    
    public:
    enemyMissle(int setID, double setx, double sety, double targetx, double targety, double setAngle, double setVelocity, double setSpeed, const Graphics::Bitmap & setBmp)
    {
        ID = setID;
        x = setx;
        y = sety;
        gox=targetx;
        goy=targety;
        maxVelocity = setVelocity;
        speedIncrement = setSpeed;
        angle = setAngle;
        bmp = setBmp;
        points=15;
        width=bmp.getWidth();
        height=bmp.getHeight();
    }
    
    ~enemyMissle()
    {
        
    }
    
    void update(int mouse_x, int mouse_y)
    {
        
        if(!moveDirection(x,y,gox,goy,velocity))
        {
            alive=false;
        }
        
        if(velocity<maxVelocity)velocity+=speedIncrement;
    }
    
    void draw(const Graphics::Bitmap & dest)
    {
        if(alive)bmp.drawRotate((int)x,(int)y,angle,dest);//rotate_sprite(dest,bmp,(int)x,(int)y,angle);
    }

    void kill()
    {
    }
    
    void specialMethod(int type)
    {
    }
};

//! Player Missle class

class playerMissle : public object
{
    private:
    
    double gox;
    double goy;
    
    public:
    playerMissle(int setID, double setx, double sety, double targetx, double targety, double setAngle, double setVelocity, double setSpeed, const Graphics::Bitmap & setBmp)
    {
        ID = setID;
        x = setx;
        y = sety;
        gox=targetx;
        goy=targety;
        maxVelocity = setVelocity;
        speedIncrement = setSpeed;
        angle = setAngle;
        bmp = setBmp;
        points=0;
        width=bmp.getWidth();
        height=bmp.getHeight();
    }
    
    ~playerMissle()
    {
        
    }
    
    void update(int mouse_x, int mouse_y)
    {
        if(!moveDirection(x,y,gox,goy,velocity))
        {
            alive=false;
        }
        
        if(velocity<maxVelocity)velocity+=speedIncrement;
    }
    
    void draw(const Graphics::Bitmap & dest)
    {
        if(alive)bmp.drawRotate((int)x,(int)y,angle,dest);//rotate_sprite(dest,bmp,(int)x,(int)y,angle);
    }
    
    void kill()
    {
        delete this;
    }
    
    void specialMethod(int type)
    {
    }
};

class moam : public object
{
    private:
    
    double goy;
    
    public:
    moam(int setID, double setx, double sety, const Graphics::Bitmap & setBmp)
    {
        ID = setID;
        x = 305;
        y = 460;
        goy=423;
        maxVelocity = .1;
        speedIncrement = .02;
        bmp = setBmp;
        points=0;
        width=bmp.getWidth();
        height=bmp.getHeight();
        loaded=false;
        launched=false;
        collidable=false;
    }
    
    ~moam()
    {
        
    }
    
    void update(int mouse_x, int mouse_y)
    {
        if(!loaded)
        {
            
            if(!moveDirection(x,y,x,goy,velocity))
            {
                loaded=true;
                goy=100;
                maxVelocity = 5;
                velocity=0;
                speedIncrement = .05;
                collidable=true;
            }
            if(velocity<maxVelocity)velocity+=speedIncrement;
        }
        else if(launched)
        {
            if(!moveDirection(x,y,x,goy,velocity))
            {
                alive=false;
            }
            if(velocity<maxVelocity)velocity+=speedIncrement;
        }
    }
    
    void draw(const Graphics::Bitmap & dest)
    {
        //if(alive)draw_sprite(dest,bmp,(int)x,(int)y);
        if(alive)
        {
            bmp.draw((int)x,(int)y, dest);
        }
    }
    
    void kill()
    {
        delete this;
    }
    
    void specialMethod(int type)
    {
    }

    bool loaded;
    bool launched;
};


class particle : public object
{
    bool hasSprite;
    double oldx;
    double oldy;
    double gox;
    double goy;
    bool reverse;
    bool dontReverse;
    int ticks;
    int maxTicks;
    int colorChanger;
    bool showInitial;
    bool slow;
    int slowTicks;
    int slowTicker;
    
    public:
    particle(int setID, double setx, double sety, double targetx, double targety, double setVelocity, double setSpeed, int tickCount, double setDecay, bool noreverse=false, bool showInit=false, bool setCollide=true, const Graphics::Bitmap & setBmp = NULL)
    {
        ID = setID;
        oldx = x = setx;
        oldy = y = sety;
        gox=targetx;
        goy=targety;
        if(gox==x&&goy==y)
        {
            slow=true;
            slowTicks=rand()%200;
            slowTicker=0;
        }
        else slow=false;
        maxVelocity = setVelocity;
        speedIncrement = setSpeed;
        /*if(setBmp==NULL)
        {
            hasSprite=false;
        }
        else
        {*/
            hasSprite=true;
            width=setBmp.getWidth();
            height=setBmp.getHeight();
        //}
        bmp = setBmp;
        reverse=false;
        dontReverse=noreverse;
        showInitial=showInit;
        collidable=setCollide;
        ticks=0;
        maxTicks=tickCount;
        decayRate=setDecay;
        alpha=rand()%100;
        colorChanger=0;
        points=0;
    }
    
    ~particle()
    {
        
    }
    
    void update(int mouse_x, int mouse_y)
    {
        if(!slow)
        {
            if(!reverse)
            {
                if(ticks==maxTicks)
                {
                    if(!moveDirection(x,y,gox,goy,velocity))
                    {
                        reverse=true;
                        ticks=0;
                        maxTicks=25;
                    }
                }
            }
            else
            {
                if(dontReverse)
                {
                    alive=false;
                    return;
                }
                if(ticks==maxTicks)
                {
                    if(!moveDirection(x,y,oldx,oldy,velocity))
                    {
                        alive=false;
                    }
                }
            }
            if(ticks<maxTicks)++ticks;
            if(velocity<maxVelocity)velocity+=speedIncrement;
        }
        else
        {
            if(slowTicker==slowTicks)
            {
                alive=false;
                return;
            }
            if(slowTicker<slowTicks)++slowTicker;
        }
        if(colorChanger<20)++colorChanger;
        if(colorChanger==20)
        {
            color=RANDOMCOLOR;
            colorChanger=0;
        }
    }
    
    void draw(const Graphics::Bitmap & dest)
    {
        if(alpha<0)alpha-=decayRate;
        //set_trans_blender(rand()%255,rand()%255,rand()%255,(int)alpha);
        Graphics::Bitmap::transBlender(rand()%255,rand()%255,rand()%255,(int)alpha);
        if(alive)
        {
            if(hasSprite)
            {
                if(ticks==maxTicks||showInitial)bmp.translucent().draw((int)x,(int)y,dest);//draw_trans_sprite(dest,bmp,(int)x,(int)y);
            }
            else
            {
                if(ticks==maxTicks||showInitial)dest.putPixel((int)x,(int)y,color);//putpixel(dest,(int)x,(int)y,color);
            }
        }
    }

    void kill()
    {
        delete this;
    }
    
    void specialMethod(int type)
    {
    }
    
};

class explosionGroup : public object
{
    private:
    
    void createPoints()
    {
        getCirclePoints(x,y,radius,circlePoints);
    }
    
    double radius;
    
    public:
    explosionGroup(int setID, double setx, double sety, double setVelocity, double setSpeed, double setRadius, bool setCollide=true, const Graphics::Bitmap & setBmp = NULL)
    {
        ID = setID;
        x = setx;
        y = sety;
        radius = setRadius;
        maxVelocity = setVelocity;
        speedIncrement = setSpeed;
        bmp = setBmp;
        createPoints();
        collidable=setCollide;
        points=0;
        for(unsigned int i=0;i<circlePoints.size();++i)
        {
            if ((rand()%4)>2){
                particles.push_back(new particle(particles.size(),x,y,circlePoints[i].getOtherX(),circlePoints[i].getOtherY(),maxVelocity,speedIncrement,0,5,false,false,collidable,bmp));
            }
            
            if((rand()%4)>2&&!toggleGfxThrottle){
                particles.push_back(new particle(particles.size(),x,y,circlePoints[i].getOtherX(),circlePoints[i].getOtherY(),maxVelocity,speedIncrement,15,5,false,false,collidable,bmp));
            }
            
            if((rand()%4)>2&&!toggleGfxThrottle){
                particles.push_back(new particle(particles.size(),x,y,circlePoints[i].getOtherX(),circlePoints[i].getOtherY(),maxVelocity,speedIncrement,30,5,false,false,collidable,bmp));
            }
            
            if((rand()%4)>2&&!toggleGfxThrottle){
                particles.push_back(new particle(particles.size(),x,y,circlePoints[i].getOtherX(),circlePoints[i].getOtherY(),maxVelocity,speedIncrement,45,5,false,false,collidable,bmp));
            }
            
            if((rand()%4)>2){
                particles.push_back(new particle(particles.size(),x,y,circlePoints[i].getOtherX(),circlePoints[i].getOtherY(),maxVelocity,speedIncrement,60,5,false,false,collidable,bmp));
            }   
        }
    }
    ~explosionGroup()
    {
    }
    
    void update(int mouse_x, int mouse_y)
    {
        unsigned int partCounter=0,littCounter=0;
        for(list<particle *>::iterator i=particles.begin();i!=particles.end();++i)
        {
            (*i)->update(mouse_x, mouse_y);
            if((rand()%500)>=499&&!toggleGfxThrottle)litter.push_back(new particle(litter.size(),(*i)->x+5,(*i)->y+5,(rand()%2<1 ? (*i)->x+5+rand()%40 : (*i)->x+5-rand()%40),
                            (rand()%2<1 ? (*i)->y+20+rand()%40 : (*i)->y+20-rand()%40),.5,.1,0,40,true,true,false));
            if((rand()%500)>=499&&!toggleGfxThrottle)litter.push_back(new particle(litter.size(),(*i)->x,(*i)->y,(*i)->x,
                            (*i)->y+20+rand()%40,.5,.1,0,40,true,true,false));
            if(!(*i)->alive)
            {
                ++partCounter;
            }
        }
        if(partCounter==particles.size())
        {
            for(list<particle *>::iterator i=particles.begin();i!=particles.end();++i)
            {
                (*i)->kill();
            }
            particles.clear();
        }
        for(list<particle *>::iterator i=litter.begin();i!=litter.end();++i)
        {
            (*i)->update(mouse_x, mouse_y);
            if(!(*i)->alive)
            {
                ++littCounter;
            }
        }
        if(littCounter==litter.size())
        {
            for(list<particle *>::iterator i=litter.begin();i!=litter.end();++i)
            {
                (*i)->kill();
            }
            litter.clear();
        }
        if(particles.empty()&&litter.empty())alive=false;
    }
    
    void draw(const Graphics::Bitmap & dest)
    {
        for(list<particle *>::iterator i=litter.begin();i!=litter.end();++i)
        {
            (*i)->draw(dest);
        }
        for(list<particle *>::iterator i=particles.begin();i!=particles.end();++i)
        {
            (*i)->draw(dest);
        }
    }
    
    bool checkCollision(object *obj)
    {
        for(list<particle *>::iterator i=particles.begin();i!=particles.end();++i)
        {
            if((*i)->checkCollision(obj))return true;
        }
        return false;
    }

    void kill()
    {
        delete this;
    }
    
    void specialMethod(int type)
    {
    }
    
    vector<Position> circlePoints;
    list<particle *>particles;
    list<particle *>litter;
    
};

class window : public object
{
    bool hasSprite;
    
    public:
    
    // Constructor
    window()
    {
    }
    window(int setID, double setx, double sety, double setWidth, double setHeight, int setColor = BLACK, const Graphics::Bitmap & setBmp = BLANK_BITMAP,bool setButton=false, string setText="nothing")
    {
        ID = setID;
        x = setx;
        y = sety;
        bmp = setBmp;
        color = setColor;
        /*if(bmp==NULL)
        {
            hasSprite=false;
            width = setWidth;
            height = setHeight;
        }
        else 
        {*/
            hasSprite=true;
            width=bmp.getWidth();
            height=bmp.getHeight();
        //}
        if(setText=="nothing")
        {
            displayText.clear();
        }
        else displayText=setText;
        isButton=setButton;
    }
    
    // Destruction!!!!!!!!!!
    ~window()
    {
        
    }
    
    void update(int mouse_x, int mouse_y)
    {
        this->mouse_x = mouse_x;
        this->mouse_y = mouse_y;
    }
    
    void draw(const Graphics::Bitmap & dest)
    {
        if(hasSprite)
        {
            //draw_sprite(dest,bmp,(int)x,(int)y);
            bmp.draw(int(x),int(y),dest);
        }
        else
        {
            //rect(dest,(int)x,(int)y,(int)x+(int)width,(int)y+(int)height,WHITE);
            //rectfill(dest,(int)x+1,(int)y+1,(int)(x+(int)width)-1,(int)(y+(int)height)-1,color);
            //if(isHover())rect(dest,(int)x,(int)y,(int)x+(int)width,(int)y+(int)height,RED);
            dest.rectangle((int)x,(int)y,(int)x+(int)width,(int)y+(int)height,WHITE);
            dest.rectangleFill((int)x+1,(int)y+1,(int)(x+(int)width)-1,(int)(y+(int)height)-1,color);
            if(isHover(mouse_x, mouse_y))dest.rectangle((int)x,(int)y,(int)x+(int)width,(int)y+(int)height,RED);
        }
        
        //if(!displayText.empty())textout_centre_ex(dest,font,displayText.c_str(),(int)x+((int)width/2),(int)y+((int)height/2),WHITE,-1);
    }
    
    void kill()
    {
    }
    
    void specialMethod(int type)
    {
    }
    
    string displayText;
    bool isButton;
    int mouse_x, mouse_y;
        
};

//! Manager class

class Manager
{
    private:
    
    void loadBitmaps()
    {
        /*DATAFILE *temp = load_datafile("games/mdefend/mdefend.dat");
        for(int i=0;i<24;++i)
        {
            const Graphics::Bitmap & b = create_bitmap(((const Graphics::Bitmap & )temp[i].dat)->w,((const Graphics::Bitmap & )temp[i].dat)->h);
            blit((const Graphics::Bitmap & )temp[i].dat,b,0,0,0,0,b->w,b->h);
            gameBitmaps.insert(make_pair(i,(const Graphics::Bitmap & )b));
        }
        unload_datafile(temp);*/
        
        gameBitmaps[0] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/background.png")).path()));
        gameBitmaps[1] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/city1.png")).path()));
        gameBitmaps[2] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/city2.png")).path()));
        gameBitmaps[3] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/missle1.png")).path()));
        gameBitmaps[4] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/missle2.png")).path()));
        gameBitmaps[5] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/missle3.png")).path()));
        gameBitmaps[6] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/missle4.png")).path()));
        gameBitmaps[7] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/missle5.png")).path()));
        gameBitmaps[8] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/moam.png")).path()));
        gameBitmaps[9] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/misslebase.png")).path()));
        gameBitmaps[10] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/moambase.png")).path()));
        gameBitmaps[11] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/turret.png")).path()));
        gameBitmaps[12] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/deadcity1.png")).path()));
        gameBitmaps[13] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/deadcity2.png")).path()));
        gameBitmaps[14] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/deadmisslebase.png")).path()));
        gameBitmaps[15] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/closedmoambase.png")).path()));
        gameBitmaps[16] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/moambaseanim1.png")).path()));
        gameBitmaps[17] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/moambaseanim2.png")).path()));
        gameBitmaps[18] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/moambaseanim3.png")).path()));
        gameBitmaps[19] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/moambaseanim4.png")).path()));
        gameBitmaps[20] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/fireball.png")).path()));
        gameBitmaps[21] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/crosshair.png")).path()));
        gameBitmaps[22] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("missile-defend/images/hand.png")).path()));
    }
    void loadSamples()
    {
        /*DATAFILE *temp = load_datafile("games/mdefend/mdefend.dat");
        for(int i=24;i<29;++i)
        {
            SAMPLE *s = create_sample(((SAMPLE *)temp[i].dat)->bits,((SAMPLE *)temp[i].dat)->stereo,((SAMPLE *)temp[i].dat)->freq,((SAMPLE *)temp[i].dat)->len);
            memcpy(s->data, ((SAMPLE *)temp[i].dat)->data,
                ((SAMPLE *)temp[i].dat)->len * ((((SAMPLE *)temp[i].dat)->bits==8) ? 1 : sizeof(short)) * ((((SAMPLE *)temp[i].dat)->stereo) ? 2 : 1));
            gameSamples.insert(make_pair(i,(SAMPLE *)s));
        }
        unload_datafile(temp);*/
    }
    
    void destroyBitmaps()
    {
        /*for(unsigned int i=0;i<gameBitmaps.size();++i)
        {
            map<int, const Graphics::Bitmap & >::iterator p;
            p = gameBitmaps.find(i);
            if(p != gameBitmaps.end())
            {
                destroy_bitmap(p->second);
            }
        }
        gameBitmaps.clear();*/
    }
    void destroySamples()
    {
        /*for(unsigned int i=0;i<gameSamples.size();++i)
        {
            map<int, SAMPLE *>::iterator p;
            p = gameSamples.find(i);
            if(p != gameSamples.end())
            {
                destroy_sample(p->second);
            }
        }
        gameBitmaps.clear();*/
    }
    
    const Graphics::Bitmap & bitmapLookup(int index)
    {
        map<int, Util::ReferenceCount<Graphics::Bitmap> >::iterator p;
        p = gameBitmaps.find(index);
        if(p != gameBitmaps.end())
        {
            return *p->second;
        }
        return BLANK_BITMAP;
    }
    
    /*SAMPLE *sampleLookup(int index)
    {
        map<int, SAMPLE *>::iterator p;
        p = gameSamples.find(index);
        if(p != gameSamples.end())
        {
            return (SAMPLE *)p->second;
        }
        return NULL;
    }*/
    
    object *objectFactory(int oType, double x, double y, double angle=0, double velocity=0, double speed=0, int extra1=0, int extra2=0, int points=0)
    {
        switch(oType)
        {
            case (CITY):
            {
                return new building(oType,x,y,
                (cities.size()%2==0) ? bitmapLookup(BUILDING1) : bitmapLookup(BUILDING2), 
                (cities.size()%2==0) ? bitmapLookup(DEADBUILDING1) : bitmapLookup(DEADBUILDING2));
            }
            case (MISSLEBASE):
                return new missleBase(oType,x,y,
                bitmapLookup(MISSLEBASE),bitmapLookup(DEADMISSLEBASE));
            case (EMISSLE):
            {
                return new enemyMissle(oType,x,y,extra1,extra2,
                angle,velocity,speed,bitmapLookup(rand()%3+5));
                break;
            }
            case (PMISSLE):
                return new playerMissle(oType,x,y,extra1,extra2,
                angle,velocity,speed,bitmapLookup(PLAYERMISSLE));
                break;
            case (MOAM):
                return new moam(oType,x,y,bitmapLookup(MOAM));
                break;
            case (MOAMBASE):
            {
                vector<Graphics::Bitmap>tempBmps;
                tempBmps.push_back(bitmapLookup(CLOSEDMOAMBASE));
                for(int i=16;i<20;++i)
                {
                    tempBmps.push_back(bitmapLookup(i));
                }
                tempBmps.push_back(bitmapLookup(MOAMBASE));
                return new moamBase(oType,x,y,tempBmps);
            }
            case (TURRET):
                return new turret(oType,x,y,angle,bitmapLookup(TURRET));
                break;
            case (EEXPLOSIONS):
                return new explosionGroup(oType,x,y,velocity,speed,extra1,true,bitmapLookup(FIREBALL));
                break;
            case (PEXPLOSIONS):
                return new explosionGroup(oType,x,y,velocity,speed,extra1,true,bitmapLookup(FIREBALL));
                break;
            case (SMOKE):
                return new particle(oType,x,y+2,x,y+2,.1,.1,0,.2,true,false,false,bitmapLookup(SMOKE));
                break;
            case (SMOKINGEXPLOSION):
                return new explosionGroup(oType,x,y,velocity,speed,extra1,false,bitmapLookup(SMOKE));
                break;
        }
        
        return NULL;
    }
    
    void playSample(int oType, int frequency=1000)
    {
        //play_sample(sampleLookup(oType), maxVolume, 127, frequency, 0);
    }
    
    void doMouse(int mouse_x, int mouse_y)
    {
        if(mouse_y<460&&!inMenu)mouseCursor=bitmapLookup(CROSSHAIR);
        else mouseCursor=bitmapLookup(HAND);
        
        leftClick=rightClick=midClick=false;
        /*
        if(mouse_b&1)
        {
            if(!mouseLeft)
            {
                leftClick=mouseLeft=true;
            }
        }
        else mouseLeft=false;
        if(mouse_b&2)
        {
            if(!mouseRight)
            {
                rightClick=mouseRight=true;
            }
        }
        else mouseRight=false;
        if(mouse_b&4)
        {
            if(!mouseMid)
            {
                midClick=mouseMid=true;
            }
        }
        else mouseMid=false;
        */
    }
    
    void createEnemyMissle(double x1, double x2, double y1 = -15)
    {
        eMissles.push_back(objectFactory(EMISSLE,x1,y1,
                    getAngle(x1,y1,x2,480), enemyVelocity, enemySpeedIncrement, x2, 480));
    }
    
    void checkGameOver()
    {
        int gameOverChecker=0;
        for(unsigned int i=0;i<cities.size();++i)
        {
            if(!cities[i]->alive)++gameOverChecker;
        }
        if((unsigned)gameOverChecker==cities.size()&&money==0)
        {
            paused=true;
            gameOver=true;
            gameTime.pause(true);
            waveClear=false;
            return;
        }
        else if((unsigned)gameOverChecker==cities.size())
        {
            int fixme=rand()%cities.size();
            cities[fixme]->alive=true;
            cities[fixme]->collidable=true;
            --money;
        }
    }
    
    void checkMoney()
    {
        if(totalScore>=nextMoneyBonus)
        {
            ++money;
            nextMoneyBonus+=gameBonus;
            playSample(DINGSND);
        }
    }
        
public:
    Manager():
    gameTime(){
        
        //! Set up intial wait time
        waitTime(7);
        
        //! Set states to false
        paused=false;
        waiting=false;
        waveClear=false;
        gameOver=false;
        initial=true;
        mouseRight=false;
        mouseLeft=false;
        mouseMid=false;
        rightClick=false;
        leftClick=false;
        midClick=false;
        warning=false;
        
        // Wave Cleared data
        waveClearTotalCities=0;
        waveClearTotalMissles=0;
        addedCityBonus=false;
        addedMissleBonus=false;
        
        //! Score
        totalScore = 0;
        scoreManager.addBadType(CITY);
        scoreManager.addBadType(MISSLEBASE);
        scoreManager.addBadType(MOAMBASE);
        scoreManager.addBadType(EMISSLE);
        scoreManager.addBadType(TURRET);
        scoreManager.addBadType(EEXPLOSIONS);
        
        //! Set menu at startup
        inMenu=false;
        
        //! Load up the bitmaps
        loadBitmaps();
        
        //! Load up the samples
        loadSamples();
        
        //! Create necessary game objects
        backBMP = bitmapLookup(BACKGROUND);
        mBases.push_back(objectFactory(MISSLEBASE,-1,365));
        mBases.push_back(objectFactory(MISSLEBASE,594,365));
        turrets.push_back(objectFactory(TURRET,13,360));
        turrets.push_back(objectFactory(TURRET,608,360));
        moamBase1 = objectFactory(MOAMBASE,297,427);
        column1 = (building *)objectFactory(CITY,-1,413);
        column2 = (building *)objectFactory(CITY,594,413);
        column1->width=48;
        column1->height=32;
        column2->width=48;
        column2->height=32;
        mouseCursor=bitmapLookup(CROSSHAIR);
        
        for(int i =45;i<269;i+=32)
        {
            cities.push_back(objectFactory(CITY,i,430));
        }
        for(int i =372;i<596;i+=32)
        {
            cities.push_back(objectFactory(CITY,i,430));
        }
        
        //! Set players settings
        totalMisslesLeft=leftMissleCapacity=20;
        totalMisslesRight=rightMissleCapacity=20;
        missleSpeedIncrement=0.07;
        missleVelocity=10;
        wave=1;
        totalBuildings=cities.size();
        playerExpRadius=25;
        money=0;
        totalMoams=0;
        
        //! Set enemy settings
        waveNextEnemies = totalEnemies=10;
        nextEnemyTick=rand()%450;
        enemyTicks=0;
        enemySpeedIncrement=0.2;
        enemyVelocity=.4;
        enemyExpRadius=15;
        
        //! Menu Related
        openMenuButton = new window(0, 100, 462, 40, 15, BLUE, BLANK_BITMAP,true,"Shop");
        toggleMoamButton = new window(0, 280, 462, 80, 15, BLUE, BLANK_BITMAP,true,"Open Base");
        launchMoamButton = new window(0, 365, 462, 50, 15, RED, BLANK_BITMAP, true, "LAUNCH");
        
        windows.push_back(new window(windows.size(),120,50,400,350,BLACK,BLANK_BITMAP,false));
        windows.push_back(new window(windows.size(),290,375,60,20,BLUE,BLANK_BITMAP,true,"Resume"));
        windows.push_back(new window(windows.size(),440,100,60,15,BLUE,BLANK_BITMAP,true,"Add"));
        windows.push_back(new window(windows.size(),440,120,60,15,BLUE,BLANK_BITMAP,true,"Add"));
        windows.push_back(new window(windows.size(),440,160,60,15,BLUE,BLANK_BITMAP,true,"Buy"));
        windows.push_back(new window(windows.size(),440,180,60,15,BLUE,BLANK_BITMAP,true,"Buy"));
        windows.push_back(new window(windows.size(),440,200,60,15,BLUE,BLANK_BITMAP,true,"Fix"));
        windows.push_back(new window(windows.size(),440,240,60,15,BLUE,BLANK_BITMAP,true,"Buy"));
        
        repair=false;
    }
    
    ~Manager()
    {
        destroyBitmaps();
        destroySamples();
        //cleanup();
    }
    
    void waitTime(int time)
    {
        waitTimer = Timer(time);
        waiting=true;
    }
    
    bool checkWaitTime()
    {
        if(waitTimer.expired())
        {
            waiting=false;
            return true;
        }
        
        return false;
    }
    
    void doLogic(int mouse_x, int mouse_y)
    {
        //! inital stuff and wait timer stuff
        if(initial)
        {
            if(checkWaitTime())
            {
                initial=false;
            }
            return;
        }
        if(waveClear)
        {
            if(checkWaitTime())
            {
                waveClear=false;
                addedCityBonus=false;
                addedMissleBonus=false;
                totalEnemies = waveNextEnemies+=2;
                nextEnemyTick=rand()%450;
                enemyTicks=0;
                enemySpeedIncrement+=.05;
                enemyVelocity+=.05;
                ++wave;
                if(mBases[0]->alive)totalMisslesLeft=leftMissleCapacity;
                if(mBases[1]->alive)totalMisslesRight=rightMissleCapacity;
                waveClearTotalCities=0;
                waveClearTotalMissles=0;
                return;
            }
            if(waveClearTotalCities==0 && waveClearTotalMissles==0)
            {
                for(unsigned int i=0;i<cities.size();++i)
                {
                    if(cities[i]->alive)
                    {
                        ++waveClearTotalCities;
                    }
                }
                waveClearTotalMissles+=totalMisslesLeft;
                waveClearTotalMissles+=totalMisslesRight;
            }
            
            if(waveClear)
            {   
                switch(waitTimer.getElapsed())
                {
                    case 2:
                        if(!addedCityBonus)
                        {
                            totalScore+=waveClearTotalCities*100;
                            addedCityBonus=true;
                        }
                        break;
                    case 5:
                        if(!addedMissleBonus)
                        {
                            totalScore+=waveClearTotalMissles*15;
                            addedMissleBonus=true;
                        }
                        break;
                    case 6:
                        checkMoney();
                        checkGameOver();
                        break;
                
                }
            }
            return;
        }
        
        // Process mouse clicks and what not
        doMouse(mouse_x, mouse_y);
        
        if(mouse_y>360)warning=true;
        else warning=false;
        
        // Shoot those missles if any clicks
        if(leftClick && mouse_y<460)
        {
            if(totalMisslesLeft>0&&turrets[0]->alive)
            {
                pMissles.push_back(objectFactory(PMISSLE,turrets[0]->x+6,turrets[0]->y+12,
                    turrets[0]->angle, missleVelocity, missleSpeedIncrement, mouse_x, mouse_y));
                --totalMisslesLeft;
                playSample(LAUNCHSND);
            }
        }
        if(rightClick && mouse_y<460)
        {
            if(totalMisslesRight>0&&turrets[1]->alive)
            {
                pMissles.push_back(objectFactory(PMISSLE,turrets[1]->x+6,turrets[1]->y+12,
                    turrets[1]->angle, missleVelocity, missleSpeedIncrement, mouse_x, mouse_y));
                --totalMisslesRight;
                playSample(LAUNCHSND);
            }
        }
        if(midClick)
        {
            moamBase1->specialMethod(moamBase::SETANIMFRAME);
            if(!((moamBase *)moamBase1)->animForward)
            {
                toggleMoamButton->displayText="Open Base";
                moamBase1->collidable=true;
            }
            else 
            {
                toggleMoamButton->displayText="Close Base";
                moamBase1->collidable=false;
            }
        }
        
        if(leftClick || rightClick)
        {
            if(openMenuButton->checkClick(mouse_x, mouse_y))
            {
                inMenu=true;
                gameTime.pause(true);
                playSample(TICKSND);
            }
            
            if(toggleMoamButton->checkClick(mouse_x, mouse_y))
            {
                moamBase1->specialMethod(moamBase::SETANIMFRAME);
                if(!((moamBase *)moamBase1)->animForward)
                {
                    toggleMoamButton->displayText="Open Base";
                    moamBase1->collidable=true;
                    playSample(TICKSND);
                }
                else if(((moamBase *)moamBase1)->animForward)
                {
                    toggleMoamButton->displayText="Close Base";
                    moamBase1->collidable=false;
                    playSample(TICKSND);
                }
            }
            if(launchMoamButton->checkClick(mouse_x, mouse_y)&&totalMoams>0)
            {
                if(((moam *)moams)->loaded && ((moamBase *)moamBase1)->animForward)
                {
                    ((moam *)moams)->launched=true;
                    playSample(LAUNCHSND,500);
                }
            }
        }
        
        // Create enemies and check wave clear
        if(totalEnemies>0)
        {
            if(enemyTicks<nextEnemyTick)++enemyTicks;
            else 
            {
                if(totalEnemies>0)
                {
                    createEnemyMissle(rand()%640,rand()%640);
                    --totalEnemies;
                }
                enemyTicks=0;
                nextEnemyTick=rand()%450;
            }
        }
        else
        {   
            if(effects.size()==0&&pMissles.size()==0)
            {
                //waitTime(13);
                waveClear=true;
                warning=false;
            }
        }   
        
        // Update all objects
        if(totalMoams>0)
        {
            if(moams->alive)
            {
                moams->update(mouse_x, mouse_y);
                if(((moam *)moams)->loaded&&!((moam *)moams)->launched&&((moamBase *)moamBase1)->animForward)
                {
                    moams->doCollisions(eMissles,scoreManager);
                    moams->doCollisions(pMissles,scoreManager);
                }
                if(((moam *)moams)->launched)
                {
                    if(rand()%200>100&&!toggleGfxThrottle)
                    {
                        effects.push_back(objectFactory(SMOKE,moams->x+2,moams->y+30,0,.5,.2,5));
                        effects.push_back(objectFactory(SMOKE,moams->x+24,moams->y+30,0,.5,.2,5));
                    }
                    moams->doCollisions(eMissles,scoreManager,true);
                    moams->doCollisions(pMissles,scoreManager,true);
                }
            }
            else
            {
                if(!((moam *)moams)->launched)
                {
                    effects.push_back(objectFactory(PEXPLOSIONS,moams->x+10,moams->y+10,0,10,1,100));
                    cities[5]->alive=cities[6]->alive=cities[7]->alive=cities[8]->alive=false;
                    playSample(BIGEXPLOSIONSND,1000);
                }
                else 
                {
                    effects.push_back(objectFactory(PEXPLOSIONS,moams->x+10,moams->y+10,0,1,.2,25));
                    effects.push_back(objectFactory(PEXPLOSIONS,moams->x+10,moams->y+10,0,10,1,300));
                    playSample(BIGEXPLOSIONSND,500);
                }
                delete moams;
                if(totalMoams>1)moams = objectFactory(MOAM,297,455);
                --totalMoams;
            }
        }
        
        moamBase1->update(mouse_x, mouse_y);
        moamBase1->doCollisions(eMissles,scoreManager);
        moamBase1->doCollisions(pMissles,scoreManager);
        column1->doCollisions(eMissles,scoreManager);
        column1->doCollisions(pMissles,scoreManager);
        column2->doCollisions(eMissles,scoreManager);
        column2->doCollisions(pMissles,scoreManager);
        
        for(unsigned int i=0;i<cities.size();++i)
        {
            cities[i]->update(mouse_x, mouse_y);
        }
        for(unsigned int i=0;i<mBases.size();++i)
        {
            mBases[i]->update(mouse_x, mouse_y);
        }
        for(unsigned int i=0;i<turrets.size();++i)
        {
            turrets[i]->update(mouse_x, mouse_y);
            if(mBases[i]->alive&&!turrets[i]->alive)
            {
                effects.push_back(objectFactory(EEXPLOSIONS,turrets[i]->x,turrets[i]->y+2,0,.5,.2,40));
                mBases[i]->alive=false;
                if(i==0)totalMisslesLeft=0;
                else totalMisslesRight=0;
                playSample(BIGEXPLOSIONSND,1500);
            }
        }
        for(unsigned int i=0;i<pMissles.size();++i)
        {
            pMissles[i]->update(mouse_x, mouse_y);
            // Smoke effect
            if(pMissles[i]->alive&&!toggleGfxThrottle)effects.push_back(objectFactory(SMOKE,pMissles[i]->x,pMissles[i]->y,0,.5,.2,5));
            pMissles[i]->doCollisions(eMissles,scoreManager);
            pMissles[i]->doCollisions(cities,scoreManager,false,true);
            if(!pMissles[i]->alive)
            {
                effects.push_back(objectFactory(PEXPLOSIONS,pMissles[i]->x,pMissles[i]->y,0,.5,.2,playerExpRadius));
                vector<object *>::iterator iter = pMissles.begin();
                iter+=i;
                pMissles.erase(iter);
                playSample(REGEXPLOSIONSND);
            }
        }
        for(unsigned int i=0;i<eMissles.size();++i)
        {
            eMissles[i]->update(mouse_x, mouse_y);
            // Smoke effect
            if(rand()%200>100&&!toggleGfxThrottle)if(eMissles[i]->alive)effects.push_back(objectFactory(SMOKE,eMissles[i]->x,eMissles[i]->y,0,.5,.2,5));
            // Split missle into 2-3 others
            if(eMissles[i]->y<280 && rand()%5000>4998)
            {
                int extraMissle=rand()%2+1;
                for(int j=0;j<extraMissle;++j)
                {
                    createEnemyMissle(eMissles[i]->x,rand()%640,eMissles[i]->y);
                }
                effects.push_back(objectFactory(SMOKINGEXPLOSION,eMissles[i]->x,eMissles[i]->y+12,0,.5,.2,15));
            }
                
            if(eMissles[i]->y>440)eMissles[i]->alive=false;
            eMissles[i]->doCollisions(turrets,scoreManager,true,true);
            eMissles[i]->doCollisions(cities,scoreManager,false,true);
            if(!eMissles[i]->alive)
            {
                effects.push_back(objectFactory(EEXPLOSIONS,eMissles[i]->x,eMissles[i]->y,0,.5,.2,enemyExpRadius));
                vector<object *>::iterator iter = eMissles.begin();
                iter+=i;
                eMissles.erase(iter);
                playSample(REGEXPLOSIONSND,1200);
            }
        }
        for(unsigned int i=0;i<effects.size();++i)
        {
            effects[i]->update(mouse_x, mouse_y);
            effects[i]->doCollisions(eMissles,scoreManager,true);
            effects[i]->doCollisions(pMissles,scoreManager,true);
            effects[i]->doCollisions(turrets,scoreManager,true,true);
            if(!effects[i]->alive)
            {
                vector<object *>::iterator iter = effects.begin();
                iter+=i;
                effects.erase(iter);
            }
        }
        
        //! scorer
        scoreManager.doPoints(totalScore);
        checkMoney();
        
        if(effects.size()<(unsigned)maxGfxThrottle)toggleGfxThrottle=false;
        else toggleGfxThrottle=true;
        
    }
    
    void doDrawing(const Graphics::Bitmap & dest)
    {
        // Blit background
        //blit(backBMP,dest,0,0,0,0,SCREEN_W,SCREEN_H);
        backBMP.drawStretched(dest);
        // Blit moams and its base
        if(totalMoams>0)moams->draw(dest);
        moamBase1->draw(dest);
        // Blit missles
        for(unsigned int i=0;i<eMissles.size();++i)
        {
            eMissles[i]->draw(dest);
        }
        for(unsigned int i=0;i<pMissles.size();++i)
        {
            pMissles[i]->draw(dest);
        }
        // Blit all other objects
        for(unsigned int i=0;i<cities.size();++i)
        {
            cities[i]->draw(dest);
        }
        for(unsigned int i=0;i<mBases.size();++i)
        {
            mBases[i]->draw(dest);
        }
        for(unsigned int i=0;i<turrets.size();++i)
        {
            turrets[i]->draw(dest);
        }
        for(unsigned int i=0;i<effects.size();++i)
        {
            effects[i]->draw(dest);
        }
        
        // Draw game menu
        //rectfill(dest,1,461,638,478,BLACK);
        //rect(dest,0,460,639,479,WHITE);
        dest.rectangleFill(1,461,638,478,BLACK);
        dest.rectangle(0,460,639,479,WHITE);
        // Draw time score and whatever else fits
        //draw_sprite(dest,bitmapLookup(PLAYERMISSLE),2,462);
        bitmapLookup(PLAYERMISSLE).draw(2,462, dest);
        /*textprintf_ex(dest,font,10,465,WHITE,-1, "%d", totalMisslesLeft);
        
        textprintf_centre_ex(dest,font,60,462,WHITE,-1,"Money");
        textprintf_centre_ex(dest,font,60,471,WHITE,-1,"$%d", money);
        
        textprintf_centre_ex(dest,font,210,462,WHITE,-1, "Score");
        textprintf_centre_ex(dest,font,210,471,WHITE,-1, "%d", totalScore );
        
        textprintf_centre_ex(dest,font,445,462,WHITE,-1, "Wave");
        textprintf_centre_ex(dest,font,445,471,WHITE,-1, "%d", wave );
        textprintf_centre_ex(dest,font,540,462,WHITE,-1, "Game Time");*/
        gameTime.print(Timer::Total,522,471,WHITE,dest,"");
        //draw_sprite(dest,bitmapLookup(PLAYERMISSLE),630,462);
        bitmapLookup(PLAYERMISSLE).draw(630,462, dest);
        //textprintf_right_ex(dest,font,628,465,WHITE,-1, "%d", totalMisslesRight);
        
        // Draw buttons
        openMenuButton->draw(dest);
        toggleMoamButton->draw(dest);
        if(((moamBase *)moamBase1)->animForward && totalMoams>0)
        {
            if(((moam *)moams)->loaded)launchMoamButton->draw(dest);
        }
        
        //! Show warning if near buildings or turrets
        //if(warning&&!initial)textprintf_centre_ex(dest,font,320,220,RED,-1, "WARNING!");
        
        if(initial)
        {
            switch(waitTimer.getElapsed())
            {
                case 0: 
                case 2:
                case 4:
                    //textprintf_centre_ex(dest,font,320,220,RED,-1, "READY!");
                    break;
                case 6:
                case 7:
                    //textprintf_centre_ex(dest,font,320,220,GREEN,-1, "GO!");
                    break;
            }
        }
        if(waveClear)
        {
            switch(waitTimer.getElapsed())
            {
                case 0:
                case 1:
                    //textprintf_centre_ex(dest,font,320,180,RANDOMCOLOR,-1, "WAVE CLEARED!");
                    break;
                case 2:
                case 3:
                case 4:
                    //textprintf_centre_ex(dest,font,320,180,BLUE,-1, "WAVE CLEARED!");
                    //textprintf_centre_ex(dest,font,320,220,WHITE,-1, "TOTAL CITIES SAVED: %d",waveClearTotalCities);
                    break;
                case 5:
                case 6:
                case 7:
                    //textprintf_centre_ex(dest,font,320,180,BLUE,-1, "WAVE CLEARED!");
                    //textprintf_centre_ex(dest,font,320,220,WHITE,-1, "TOTAL CITIES SAVED: %d",waveClearTotalCities);
                    //textprintf_centre_ex(dest,font,320,260,WHITE,-1, "TOTAL MISSLES RETAINED: %d",waveClearTotalMissles);
                    break;
                case 8:
                case 9:
                    //textprintf_centre_ex(dest,font,320,220,WHITE,-1, "WAVE %d START",wave+1);
                    break;
                case 10:
                    //textprintf_centre_ex(dest,font,320,220,RED,-1, "READY!");
                    break;
                case 12:
                case 13:
                    //textprintf_centre_ex(dest,font,320,220,GREEN,-1, "GO!");
                    break;
            }
        }
    }
    
    void doMenu(const Graphics::Bitmap & dest)
    {
#if 0
        for(unsigned int i=0;i<windows.size();++i)
        {
            windows[i]->draw(dest);
        }
        textprintf_centre_ex(dest,font,320,55,WHITE,-1, "SHOP");
        textprintf_centre_ex(dest,font,320,75,PURPLE,-1, "TOTAL BONUS MONEY: $%d",money);
        textprintf_ex(dest,font,125,105,YELLOW,-1,"ACCELERATION");
        textprintf_ex(dest,font,125,125,YELLOW,-1,"EXPLOSION RADIUS");
        textprintf_ex(dest,font,125,165,YELLOW,-1,"INCREASE CAPACITY FOR LEFT TURRET");
        textprintf_ex(dest,font,125,185,YELLOW,-1,"INCREASE CAPACITY FOR RIGHT TURRET");
        textprintf_ex(dest,font,125,205,YELLOW,-1,"RESTORE TURRETS OR CITIES"); 
        textprintf_ex(dest,font,125,245,YELLOW,-1,"MOTHER OF ALL MISSLES");     
        textprintf_centre_ex(dest,font,320,265,PURPLE,-1, "LEFT TURRET CAPACITY: %d",leftMissleCapacity);
        textprintf_centre_ex(dest,font,320,285,PURPLE,-1, "RIGHT TURRET CAPACITY: %d",rightMissleCapacity);
        textprintf_centre_ex(dest,font,320,305,PURPLE,-1, "TOTAL MOAMS: %d",totalMoams);
            
        
        int temploc=280,tempinc=0, tempcolor=GREEN;
        for(int i=0;i<((missleSpeedIncrement-0.07)/.01);++i)
        {
            if(i>10)tempcolor=YELLOW;
            if(i>13)tempcolor=RED;
            rectfill(dest,temploc+tempinc,100,temploc+tempinc+10,115,tempcolor);
            tempinc+=10;
        }
        temploc=280;
        tempinc=0;
        tempcolor=GREEN;
        for(int i=0;i<playerExpRadius-25;++i)
        {
            if(i>10)tempcolor=YELLOW;
            if(i>13)tempcolor=RED;
            rectfill(dest,temploc+tempinc,120,temploc+tempinc+10,135,tempcolor);
            tempinc+=10;
        }
        rect(dest,280,100,430,115,WHITE);
        rect(dest,280,120,430,135,WHITE);
        
        if(repair)
        {
            textprintf_centre_ex(dest,font,320,225,PURPLE,-1, "SELECT CITY/TURRET TO REPAIR");
            for(unsigned int i=0;i<cities.size();++i)
            {
                if(!cities[i]->alive && cities[i]->isHover())
                {
                    rect(dest,(int)cities[i]->x,(int)cities[i]->y,(int)cities[i]->x+(int)cities[i]->width,(int)cities[i]->y+(int)cities[i]->height,RED);
                }
            }
            for(unsigned int i=0;i<mBases.size();++i)
            {
                if(!mBases[i]->alive && mBases[i]->isHover())
                {
                    rect(dest,(int)mBases[i]->x,(int)mBases[i]->y,(int)mBases[i]->x+(int)mBases[i]->width,(int)mBases[i]->y+(int)mBases[i]->height,RED);
                }
            }
        }
#endif
    }
    
    void doMenuLogic(int mouse_x, int mouse_y)
    {
        //! Check mouse clicks
        doMouse(mouse_x, mouse_y);
        if(money==0)
        {
            windows[2]->color=GREY;
            windows[3]->color=GREY;
            windows[4]->color=GREY;
            windows[5]->color=GREY;
            windows[6]->color=GREY;
            windows[7]->color=GREY;
        }
        else
        {
            if(missleSpeedIncrement<.21)windows[2]->color=BLUE;
            else 
            {
                windows[2]->color=GREY;
                windows[2]->displayText="MAX";
            }
            if(playerExpRadius<40)windows[3]->color=BLUE;
            else 
            {
                windows[3]->color=GREY;
                windows[3]->displayText="MAX";
            }
            windows[4]->color=BLUE;
            windows[5]->color=BLUE;
            windows[6]->color=BLUE;
            windows[7]->color=BLUE;
        }
        
        if(leftClick || rightClick)
        {
            if(windows[1]->checkClick(mouse_x, mouse_y))
            {
                inMenu=false;
                gameTime.pause(false);
                repair=false;
                playSample(TICKSND);
            }
            if(windows[2]->checkClick(mouse_x, mouse_y))
            {
                if(windows[2]->color!=GREY)
                {
                    missleSpeedIncrement+=.01;
                    --money;
                    playSample(TICKSND);
                }
            }
            if(windows[3]->checkClick(mouse_x, mouse_y))
            {
                if(windows[3]->color!=GREY)
                {
                    ++playerExpRadius;
                    --money;
                    playSample(TICKSND);
                }
            }
            if(windows[4]->checkClick(mouse_x, mouse_y))
            {
                if(windows[4]->color!=GREY)
                {
                    totalMisslesLeft=leftMissleCapacity+=5;
                    --money;
                    playSample(TICKSND);
                }
            }
            if(windows[5]->checkClick(mouse_x, mouse_y))
            {
                if(windows[5]->color!=GREY)
                {
                    totalMisslesRight=rightMissleCapacity+=5;
                    --money;
                    playSample(TICKSND);
                }
            }
            if(windows[6]->checkClick(mouse_x, mouse_y))
            {
                if(windows[6]->color!=GREY)
                {
                    repair=!repair;
                    playSample(TICKSND);
                }
            }
            if(windows[7]->checkClick(mouse_x, mouse_y))
            {
                if(windows[7]->color!=GREY)
                {
                    if(totalMoams==0)
                    {
                        moams = objectFactory(MOAM,297,455);
                    }
                    ++totalMoams;
                    --money;
                    playSample(TICKSND);
                }
            }
            if(repair)
            {
                for(unsigned int i=0;i<cities.size();++i)
                {
                    if(!cities[i]->alive && cities[i]->checkClick(mouse_x, mouse_y))
                    {
                        cities[i]->alive=true;
                        cities[i]->collidable=true;
                        --money;
                        if(money==0)repair=false;
                        playSample(TICKSND);
                    }
                }
                for(unsigned int i=0;i<mBases.size();++i)
                {
                    if(!mBases[i]->alive && mBases[i]->checkClick(mouse_x, mouse_y))
                    {
                        turrets[i]->alive=true;
                        turrets[i]->collidable=true;
                        mBases[i]->alive=true;
                        if(i==0)totalMisslesLeft=leftMissleCapacity;
                        else totalMisslesRight=rightMissleCapacity;
                        --money;
                        if(money==0)repair=false;
                        playSample(TICKSND);
                    }
                }
            }
                
        }
    }
    
    //! Game Data
    scoreHandler scoreManager;
    int totalScore;
    int totalBuildings;
    int totalMisslesLeft;
    int totalMisslesRight;
    int leftMissleCapacity;
    int rightMissleCapacity;
    int wave;
    int waveClearTotalCities;
    int waveClearTotalMissles;
    bool addedCityBonus;
    bool addedMissleBonus;
    int nextMoneyBonus;
    int gameBonus;
    
    bool warning;
    bool paused;
    bool waiting;
    bool initial;
    bool waveClear;
    bool gameOver;
    
    //! Player data
    double missleSpeedIncrement;
    double missleVelocity;
    double playerExpRadius;
    int money;
    int totalMoams;
    
    //! Enemy data
    int nextEnemyTick;
    int enemyTicks;
    int totalEnemies;
    int waveNextEnemies;
    double enemySpeedIncrement;
    double enemyVelocity;
    double enemyExpRadius;
    
    //! Game objects
    vector<object *>cities;
    vector<object *>mBases;
    vector<object *>turrets;
    vector<object *>eMissles;
    vector<object *>pMissles;
    vector<object *>effects;
    object *moamBase1;
    object *moams;
    building *column1;
    building *column2;
    
    //! Game bitmaps
    map<int, Util::ReferenceCount<Graphics::Bitmap> >gameBitmaps;
    Graphics::Bitmap backBMP;
    Graphics::Bitmap mouseCursor;
    
    //! Game Samples
    //map<int,SAMPLE *>gameSamples;
    
    //! Timers
    // main timer
    Timer gameTime;
    Timer waitTimer;
    
    //! a vector to hold the elapsed time of each level so that we can reward the player later on
    vector<int>levelTimeTotal;
    
    //! Menu related
    vector<window *>windows;
    window *openMenuButton;
    window *toggleMoamButton;
    window *launchMoamButton;
    
    bool repair;
    
    bool inMenu;    
    
    //! Mouse stuff
    int mouse_x;
    int mouse_y;
    bool mouseRight;
    bool mouseLeft;
    bool mouseMid;
    bool rightClick;
    bool leftClick;
    bool midClick;
    enum
    {
        //Bitmaps
        BACKGROUND=0,
        BUILDING1,
        BUILDING2,
        PLAYERMISSLE,
        ENEMYMISSLE1,
        ENEMYMISSLE2,
        ENEMYMISSLE3,
        ENEMYMISSLE4,
        MOAM,
        MISSLEBASE,
        MOAMBASE,
        TURRET,
        DEADBUILDING1,
        DEADBUILDING2,
        DEADMISSLEBASE,
        CLOSEDMOAMBASE,
        MOAMBASEANIM1,
        MOAMBASEANIM2,
        MOAMBASEANIM3,
        MOAMBASEANIM4,
        FIREBALL,
        SMOKE,
        CROSSHAIR,
        HAND,
        LAUNCHSND,
        BIGEXPLOSIONSND,
        REGEXPLOSIONSND,
        DINGSND,
        TICKSND,
        //Others
        CITY,
        EMISSLE,
        PMISSLE,
        EEXPLOSIONS,
        PEXPLOSIONS,
        SMOKINGEXPLOSION
        
        
    };
};

class MissileDefendGame
{
    public:
        
    Manager gameManager;
        
    MissileDefendGame(){
    }
    
    ~MissileDefendGame(){
    }
    
    std::string GetName() 
    {
        return "Missle Defend";
    }
    
    std::string GetAuthor() 
    {
        return "Miguel A. Gavidia";
    }
    
    std::string GetDescription() 
    {
        return "Similar to the atari classic missle command.\nDifferences include:\n1: Missle speed and explosion size adjustments\n2: Mother of All Missles (Something like a flash bomb)\n3: Repairable cities\nAll purchaseable with bonus points.";
    }
    
    std::string GetVersion() 
    {
        return "1.0";
    }

    std::string GetIconPath() 
    {
        return "mdefend.bmp";       
    }

    bool Init() {
#if 0
        if (!IGame::Init()) {
            return false;
        }

        // custom initialization goes here
        // ...
        
        int tempSettings = mdefendSettings[0].value;
        
        //Get particle configuration
        switch(tempSettings)
        {
            case 0:
                tempSettings=10;
                break;
            case 1:
                tempSettings=20;
                break;
            case 2: 
                tempSettings=30;
                break;
            case 3:
                tempSettings=50;
                break;
            case 4:
                tempSettings=80;
                break;
            case 5: 
                tempSettings=120;
                break;
            case 6:
                tempSettings=200;
                break;
            case 7:
                tempSettings=400;
                break;
            case 8: 
                tempSettings=600;
                break;
            case 9:
                tempSettings=1000;
                break;
            case 10:
                tempSettings=2000;
                break;
        }
            
        maxGfxThrottle=tempSettings;
        toggleGfxThrottle=false;
        
        tempSettings = mdefendSettings[1].value;
        
        //Get Sound settings
        switch(tempSettings)
        {
            case 0:
                tempSettings=0;
                break;
            case 1:
                tempSettings=60;
                break;
            case 2: 
                tempSettings=180;
                break;
            case 3:
                tempSettings=255;
                break;
        }
        maxVolume = tempSettings;
        
        tempSettings = mdefendSettings[2].value;
        
        //Get bonus settings
        switch(tempSettings)
        {
            case 0:
                tempSettings=500;
                break;
            case 1:
                tempSettings=1000;
                break;
            case 2:
                tempSettings=1500;
                break;
            case 3: 
                tempSettings=2000;
                break;
            case 4:
                tempSettings=2500;
                break;
            case 5:
                tempSettings=3000;
                break;
        }
        
        gameManager->nextMoneyBonus = gameManager->gameBonus = tempSettings; 

        return true;
#endif
        return true;
    }

    void Deinit() 
    {
        // custom deinitialization goes here
        // ...
        
        // no more Allegro calls after this line!
        //IGame::Deinit();
    }

    void draw(int mouse_x, int mouse_y, const Graphics::Bitmap & canvas) 
    {
        // Do all the drawing here. Draw only to the canvas bitmap.
        // ...
        
        gameManager.doDrawing(canvas);
        if(gameManager.inMenu){
            gameManager.doMenu(canvas);
        }
        if(!gameManager.paused){
            gameManager.mouseCursor.draw(mouse_x,mouse_y, canvas);
        }
    }

    bool logic(int mouse_x, int mouse_y) 
    {
        if(!gameManager.paused){
            if(!gameManager.inMenu){
                gameManager.doLogic(mouse_x, mouse_y);
            }
            else {
                gameManager.doMenuLogic(mouse_x, mouse_y);
            }
        }
        if(gameManager.gameOver){
            return true;
        }
        
        return false;
    }

    int Score() 
    {
        // Return -1 if this game doesn't have points, otherwise the last
        // valid score where more points is better.
        return gameManager.totalScore;
    }

    bool UseSystemMouse() 
    {
        // Return false if the game draws its own cursor or doesn't use
        // the mouse at all.
        return false;
    }
    
    void Pause() 
    {
        gameManager.paused=true;
        gameManager.gameTime.pause(true);
    }
    void Resume() 
    {
        gameManager.paused=false;
        gameManager.gameTime.pause(false);
    }

    // Should return true if keys can be redefined (otherwise
    // global keys are used)
    bool RedefinesKeys() {
        return false;
    }
    // Should return names of redefinable keys or NULL if
    // key can't be redefined. i = [0,7]
    const char *KeyName(int i) {
        switch (i) {
            default: return 0;
        };
    }
    // Should return to a NULL terminated array of GameSettings structures.
    // For example you could define the array like this:
    //
    //  GameSettings my_settings[] = {
    //      { "name1", "option 1", "first|second|third", 1 },
    //      { "name2", "option 2", "first|second|third", 0 },
    //      { "name3", "option 3", "first|second|third", 2 },
    //      { 0, 0, 0, 0 }
    //  };
    //
    // and then return my_settings in this function.
    /*GameSettings* GetSettings() {
        return mdefendSettings;
    }*/
    
};


enum GameInput{
    Quit,
    Up,
    Down,
    Left,
    Right,
    Shoot,
};

class Game: public Util::Logic, public Util::Draw {
public:
    Game():
    quit(false),
    mouse_x(0),
    mouse_y(0){
        input.set(Keyboard::Key_ESC, Quit);
        input.set(Keyboard::Key_UP, Up);
        input.set(Keyboard::Key_DOWN, Down);
        input.set(Keyboard::Key_LEFT, Left);
        input.set(Keyboard::Key_RIGHT, Right);
        input.set(Keyboard::Key_SPACE, Shoot);
        
        input.set(Configuration::config(0).getJoystickUp(), Up);
        input.set(Configuration::config(0).getJoystickDown(), Down);
        input.set(Configuration::config(0).getJoystickLeft(), Left);
        input.set(Configuration::config(0).getJoystickRight(), Right);
        input.set(Configuration::config(0).getJoystickAttack1(), Shoot);
        input.set(Configuration::config(0).getJoystickAttack2(), Shoot);
        input.set(Configuration::config(0).getJoystickAttack3(), Shoot);
        input.set(Configuration::config(0).getJoystickAttack4(), Shoot);
    }

    MissileDefendGame defend;
    InputMap<GameInput> input;
    InputSource source;

    void run(){
        quit = defend.logic(mouse_x, mouse_y);
        handleInput();
    }

    void handleInput(){
        class Handler: public InputHandler<GameInput> {
        public:
            Handler(Game & game):
                game(game){
                }

            Game & game;

            void release(const GameInput & input, Keyboard::unicode_t unicode){
            }

            void press(const GameInput & input, Keyboard::unicode_t unicode){
                switch (input){
                    case Quit: {
                        game.quit = true;
                        break;
                    }
                    case Up: {
                        game.up();
                        break;
                    }
                    case Down: {
                        game.down();
                        break;
                    }
                    case Left: {
                        game.left();
                        break;
                    }
                    case Right: {
                        game.right();
                        break;
                    }
                    case Shoot: {
                        game.shoot();
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        Handler handler(*this);
        InputManager::handleEvents(input, source, handler);
    }

    double ticks(double system){
        return system * Global::ticksPerSecond(40);
    }

    bool done(){
        return quit;
    }

    void draw(const Graphics::Bitmap & buffer){
        Graphics::StretchedBitmap work(GFX_X, GFX_Y, buffer);
        work.start();
        work.clear();
        defend.draw(mouse_x, mouse_y, work);
        work.finish();
        buffer.BlitToScreen();
    }
    
    void up(){
        if (mouse_y > 0){
            mouse_y--;
        }
    }
    
    void down(){
        if (mouse_y < GFX_Y){
            mouse_y++;
        }
    }
    
    void left(){
        if (mouse_x > 0){
            mouse_x--;
        }
    }
    
    void right(){
        if (mouse_x < GFX_X){
            mouse_x++;
        }
    }
    
    void shoot(){
    }

protected:
    bool quit;
    int mouse_x;
    int mouse_y;
};

void run(){
    Global::debug(0) << "Starting Missile Defend!" << std::endl;
    Music::changeSong();
    Keyboard::pushRepeatState(false);
    Game game;
    Util::standardLoop(game, game);
    Keyboard::popRepeatState();
}

}
