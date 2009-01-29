#ifndef mugen_background_h
#define mugen_background_h

#include <string>
#include <vector>
#include <map>
#include "mugen_animation.h"

class MugenSprite;
class MugenBackground;
class Bitmap;

enum ControlType{
    Null = 0,
    Visible,
    Enabled,
    Velset,
    Veladd,
    Posset,
    PosAdd,
    Animation,
    Sinx,
    Siny
};

enum BackgroundType{
    Normal = 0,
    Parallax,
    Anim,
    Dummy
};

class BgController{
    public:
	BgController( ControlType ctrl, std::vector<int>ids );
	virtual ~BgController();
	const inline ControlType &getType() { return type; }
	virtual void act( std::map<int, MugenBackground *> &bgs );
    private:
	ControlType type;
	int timestart;
	int endtime;
	int looptime;
	std::vector<int> ctrlID;
};

class MugenBgController{
    public:
	MugenBgController( std::string &n, std::vector<int>ids );
	~MugenBgController();
	
	void addControl( BgController *ctrl );
	
	const inline std::string &getName() { return name; }
	
	void act( std::map<int, MugenBackground *> &bgs );
	
    protected:
	std::string name;
	int looptime;
	std::vector<int> ctrlID;
	std::vector<BgController *> controls;
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
    
    std::string name;
    
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
    double siny_amp;
    double siny_period;
    double siny_offset;
};

#endif
