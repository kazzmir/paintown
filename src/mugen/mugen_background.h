#ifndef mugen_background_h
#define mugen_background_h

#include <string>
#include <vector>

class MugenSprite;
class MugenAnimation;

enum ControlType{
    Null = 0,
    Visible,
    Enabled,
    Velset,
    Veladd,
    Posset,
    PosAdd,
    Anim,
    Sinx,
    Siny
};

enum BackgroundType{
    Normal = 0,
    Parallax,
    Anim,
    Dummy
};

class BackgroundController{
    int type;
    int timestart;
    int endtime;
    int looptime;
    std::vector<int> ctrlID;
};

struct MugenBackgroundController{
    MugenBackgroundController( std::string &n ){ name = n; }
    std::string name;
    int looptime
    std::vector<int> ctrlID;
    std::vector<BackgroundController> controls;
};

// mugen background
class MugenBackground{
public:
    MugenBackground();
    MugenBackground( const MugenBackground &copy );
    ~MugenBackground();
    
    MugenBackground & operator=( const MugenBackground &copy );
    
    void logic();
    
    void render( int xaxis, int yaxis, Bitmap *bmp );
    

protected:
    BackgroundType type;
    // If -1 then use the animation
    int groupNumber;
    int imageNumber;
    int actionno;
    MugenAnimation *action;
    int id;
    int layerno;
    int startx;
    int starty;
    double deltax;
    double deltay;
    std::string trans;
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
    double siny_amp;
    double siny_period;
    double siny_offset;
};

#endif
