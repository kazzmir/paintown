#ifndef mugen_background_h
#define mugen_background_h

#include <string>
#include <vector>
#include <map>
#include "mugen_animation.h"
#include "mugen_util.h"

class MugenSection;
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
    inline const int getActionNumber() const { return actionno; }
    inline const int getLayerNumber() const { return layerno; }
    
    MugenBackground & operator=( const MugenBackground &copy );
    
    void logic( const double x, const double y, const double placementx, const double placementy );
    
    void render( const double windowx, const double windowy, const int totalLength, const int totalHeight, Bitmap *work );
    
    void setAnimation(MugenAnimation *animation);
    
    // Do any extra stuff so that it can be used
    void preload( const int xaxis, const int yaxis );
    
    // Stage be our friend This all goes away once I get the manager working
    friend class MugenStage;
    friend class BackgroundController;
    friend class MugenUtil;
    friend class MugenMenu;
    friend class MugenBackgroundManager;
    
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
    Mugen::Effects effects;
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
    
    // animation
    MugenAnimation *action;
    
    // Position link item
    MugenBackground *linked;
    bool runLink;
    
    // Do draw stuff
    void draw( const int ourx, const int oury, Bitmap &work );
    
    // Lets do our positionlink stuff
    void setPositionLink(MugenBackground *bg);
    
};


/* Background controllers 
We'll seperate this from the main FSM of characters since we aren't looking for
complete compatability */

enum ControlType{
    Ctrl_Null = 0,
    Ctrl_Visible,
    Ctrl_Enabled,
    Ctrl_VelSet,
    Ctrl_VelAdd,
    Ctrl_PosSet,
    Ctrl_PosAdd,
    Ctrl_Animation,
    Ctrl_Sinx,
    Ctrl_Siny
};

class BackgroundController{
    public:
	BackgroundController();
	virtual ~BackgroundController();
	virtual void act(const std::map< int, MugenAnimation * > &animations);
	void reset();
	std::string name;
	ControlType type;
	int timestart;
	int endtime;
	int looptime;
	int ownticker;
        /* what do these values represent? lets change the variable name
         * or at least explain with a comment here.
         */
	double value1;
	double value2;
	double value3;
	std::vector<MugenBackground *> backgrounds;
};

class MugenBackgroundController{
    public:
	MugenBackgroundController( const std::string &n );
	~MugenBackgroundController();
	
	void addControl( BackgroundController *ctrl );
	
	void act(const std::map< int, MugenAnimation * > &animations);
	
	std::string name;
	int id;
	int looptime;
	int ticker;
	std::vector<MugenBackground *> backgrounds;
	std::vector<BackgroundController *> controls;
};

/*! Holds mugen backgrounds and foregrounds and controllers */
class MugenBackgroundManager{
    public:
	/* It takes in a collection and reads the necessary crap
	if sprites = 0 then it has it's own sprite collection and won't be bothered to use the external one
	*/
	MugenBackgroundManager(const std::string &baseDir, const std::vector< MugenSection * > &collection, unsigned int &index,const unsigned long int &ticker, 
				std::map< unsigned int, std::map< unsigned int, MugenSprite * > > *sprites=0);
	~MugenBackgroundManager();
	void logic( const double x, const double y, const double placementx, const double placementy );
	void renderBack( const double windowx, const double windowy, const int totalLength, const int totalHeight, Bitmap *work );
	void renderFront( const double windowx, const double windowy, const int totalLength, const int totalHeight, Bitmap *work );
	MugenBackground *getBackground( int ID );
	
	void preload( const int startx, const int starty );
	
	void reset( const int startx, const int starty, bool resetBG );
	
	inline const std::string &getName() const { return name; }
	inline const bool getDebugStatus() const { return debugbg; }
	inline const std::map<int, MugenAnimation*> & getAnimations() const {
            return animations;
        }
    private:
	// Name minus the Def part so we can grab other similar items
	std::string name;
	
	// Debug
	bool debugbg;
	
	// Background clear color
	int clearColor;
	
	// If it has it's own spritefile .. else it'll just use the one by it's parent
	std::string spriteFile;
	std::map< unsigned int, std::map< unsigned int, MugenSprite * > > sprites;
	
	/* Animation Lists stored by action number, ie [Begin Action 500] */
	std::map< int, MugenAnimation * > animations;
	
	/* Backgrounds */
	std::vector< MugenBackground * > backgrounds;
	
	/* Foregrounds */
	std::vector< MugenBackground * > foregrounds;
	
	// Controllers
	std::vector<MugenBackgroundController *> controllers;
	
	// This is for controllers as sometimes backgrounds share IDs for this purpose
	void getBackgrounds( std::vector<MugenBackground *> &bgs, int ID );
};

#endif
