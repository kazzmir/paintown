#ifndef mugen_storyboard_h
#define mugen_storyboard_h

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "mugen/mugen_exception.h"
#include "mugen/mugen_fadetool.h"
#include "mugen/mugen_util.h"

class Bitmap;
class MugenItemContent;
class MugenSprite;
class MugenSound;
class MugenAnimation;
class MugenBackground;
class MugenBackgroundManager;

class MugenLayer{
    public:
	MugenLayer();
	~MugenLayer();
	Mugen::Point offset;
	int actionno;
	int startTime;
	MugenAnimation *animation;
	void act();
	void draw(const int xaxis, const int yaxis, Bitmap *bmp);
	void reset();
};

class MugenScene {
    public:
	MugenScene();
	~MugenScene();
	
	void act();
	
	void draw(Bitmap *bmp);
	
	bool isDone();
	
	void reset();
	
	MugenFadeTool fader;
	
	int clearColor;
	int ticker;
	int endTime;
	
	Mugen::Point defaultAxis;
	
	std::string backgroundName;
	
	MugenBackgroundManager *background;
	
	// layers
	std::vector<MugenLayer *>layers;
};

class MugenStoryboard {
public:
	// Location at dataPath() + "mugen/stages/"
	MugenStoryboard( const std::string & s );

	virtual ~MugenStoryboard();
	
	void load() throw (MugenException);
	// give it the default bitmap to blit to the screen
	void run(Bitmap *bmp, bool repeat = false);
	
    private:
	
	std::string location;
	
	std::string spriteFile;
	
	int startscene;
	
	/* Sprites */
	std::map< unsigned int, std::map< unsigned int, MugenSprite * > > sprites;
	
	/* Animation Lists stored by action number */
	std::map< int, MugenAnimation * > animations;
	
	// Scenes
	std::vector<MugenScene *>scenes;
	
	void cleanup();
};

#endif
