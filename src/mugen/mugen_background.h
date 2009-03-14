#ifndef mugen_background_h
#define mugen_background_h

#include <string>
#include <vector>
#include <map>
#include "mugen_animation.h"

class MugenSprite;
class MugenBackground;
class MugenStage;
class Bitmap;
class BackgroundController;

enum BackgroundType{
    Normal = 0,
    Parallax,
    Anim,
    Dummy
};

enum TransType{
    None = 0,
    Add,
    Add1,
    Sub,
    Addalpha
};

// mugen background
class MugenBackground{
public:
    MugenBackground( const unsigned long int &ticker );
    MugenBackground( const MugenBackground &copy );
    ~MugenBackground();
    
    inline const std::string &getName() { return name; }
    inline void setVisible(const bool vis){ visible = vis; }
    inline const bool getVisible(){ return visible; }
    inline void setEnabled(const bool en){ enabled = visible = en; }
    inline const bool getEnabled(){ return enabled; }
    
    MugenBackground & operator=( const MugenBackground &copy );
    
    void logic( const double x, const double y, const double placementx, const double placementy );
    
    void render( const int totalLength, const int totalHeight, Bitmap *work );
    
    // Stage be our friend
    friend class MugenStage;
    friend class BackgroundController;
    
    protected:
    std::string name;
    
    BackgroundType type;
    // If -1 then use the animation
    int groupNumber;
    int imageNumber;
    int actionno;
    int id;
    int layerno;
    int startx;
    int starty;
    double deltax;
    double deltay;
    TransType trans;
    int alphalow;
    int alphahigh;
    bool mask;
    int tilex;
    int tiley;
    int tilespacingx;
    int tilespacingy;
    MugenArea window;
    int windowdeltax;
    int windowdeltay;
    double xscaletop;
    double xscalebot;
    int yscalestart;
    double yscaledelta;
    bool positionlink;
    double velocityx;
    double velocityy;
    double sinx_amp;
    double sinx_period;
    double sinx_offset;
    double sinx_angle;
    double siny_amp;
    double siny_period;
    double siny_offset;
    double siny_angle;
    int xoffset;
    int yoffset;
    double movex;
    double movey;
    double velx;
    double vely;
    const unsigned long int &stageTicker;
    int x;
    int y;
    bool visible;
    bool enabled;
    double controller_offsetx;
    double controller_offsety;
    
    // sprite 
    MugenSprite *sprite;
    
    Bitmap *spriteBmp;
    
    // animation
    MugenAnimation *action;
    
    // Position link item
    MugenBackground *linked;
    bool runLink;
    
    // Do any extra stuff so that it can be used
    void preload( const int xaxis, const int yaxis );
    
    // Do draw stuff
    void draw( const int ourx, const int oury, Bitmap &work );
    
    // Lets do our positionlink stuff
    void setPositionLink(MugenBackground *bg);
    
};

#endif
