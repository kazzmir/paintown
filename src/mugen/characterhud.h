#ifndef _mugen_character_hud_h
#define _mugen_character_hud_h

#include <string>

#include "mugen/mugen_util.h"
#include "mugen/mugen_exception.h"

class Bitmap;
class MugenSprite;
class MugenAnimation;
class MugenFont;

namespace Mugen{

class Element{
    public:
	Element();
	virtual ~Element();
	
	virtual void act();
	virtual void render(const int xaxis, const int yaxis, Bitmap &);
	
	enum ElementType{
	    IS_NOTSET =0,
	    IS_ACTION,
	    IS_SPRITE,
	    IS_FONT,
	    IS_SOUND
	};
	
	virtual inline void setType(ElementType t){ type = t; }
	virtual void setAction(MugenAnimation *);
	virtual void setSprite(MugenSprite *);
	virtual void setFont(MugenFont *);
	virtual inline void setOffset(const int x, const int y) { offset = Mugen::Point(x,y); }
	virtual inline void setRange(const int x, const int y) { range = Mugen::Point(x,y); }
	virtual inline void setDisplayTime(const int t) { displaytime = t; }
	virtual inline void setFacing(const int f) { effects.facing = f; }
	virtual inline void setVFacing(const int f) { effects.vfacing = f; }
	virtual inline void setLayer(const int i) { layerno = i; }
	virtual inline void setScale(const double x, const double y) { effects.scalex = x, effects.scaley = y; }
	virtual inline void setText(const std::string &t) { text = t; }
	
	virtual inline int getLayer() { return layerno; }
	
    private:
	ElementType type;
	MugenAnimation *action;
	MugenSprite *sprite;
	MugenFont *font;
	Mugen::Point offset;
	Mugen::Point range;
	int displaytime;
	Effects effects;
	int layerno;
	std::string text;
};

class Bar{
    public:
	Bar();
	Bar(const int x, const int y);
	virtual ~Bar();
	
	virtual void setBack0(Element *);
	virtual void setBack1(Element *);
	virtual void setBack2(Element *);
	virtual void setMiddle(Element *);
	virtual void setFront(Element *);
	virtual void setCounter(Element *);
	virtual void setSound(Element *);
	
	virtual void act();
	virtual void render(const int xaxis, const int yaxis, Bitmap &);
	
    private:
	Mugen::Point position;
	Element *back0;
	Element *back1;
	Element *back2;
	Element *middle;
	Element *front;
	Element *counter;
	Element *sound;
};

class Face{
    public:
	Face();
	Face(const int x, const int y);
	virtual ~Face();
	
	virtual void act();
	virtual void render();
    private:
	Mugen::Point position;
	Mugen::Point spacing;
	Element background;
	Element face;
};

class Name{
    public:
	Name();
	virtual ~Name();
	
    private:
	Mugen::Point position;
	Element *font;
};

/*! Holder for character data */
class PlayerInfo{
    public:
	PlayerInfo();
	~PlayerInfo();
    
    private:
	/* HUD collection, compromised of a static amount of 4 players
	   No need to make dynamic since it models the way MUGEN does it for now,
	   might change later */
	Bar *lifeBar[4];
	Bar *powerBar[4];
	Face *face[4];
	Name *name[4];
	
};

/*! Character HUD ... lifebar, face, etc */
class CharacterHUD{
    public:
	CharacterHUD( const std::string & s );
	virtual ~CharacterHUD();
	
	virtual void load() throw (MugenException);
	
	void act();
	void render(const int xaxis, const int yaxis, Bitmap &);
	
    private:
	/* files */
	std::string location;
	std::string sffFile;
	std::string sndFile;
	std::vector<std::string>fontFiles;
	std::string fightfx_sff_File;
	std::string fightfx_air_File;
	std::string common_snd_File;
	
	/* player data */
	PlayerInfo *normal;
	PlayerInfo *simultaneous;
	PlayerInfo *team;
	
	/* Time Display */
	
	/* Combo Display */
	
	/* Round and Fight Display */
	
	/* KO Display */
	
	/* Win Icon... I guess this is the markers for wins */
	
	
};

}

#endif

