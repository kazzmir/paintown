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

class FightElement : public Element{
    public:
	FightElement();
	virtual ~FightElement();
	
	virtual void act();
	virtual void render(int x, int yaxis, Bitmap &);
	
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
	virtual void render(const int xaxis, const int yaxis, Bitmap &);
	virtual void setPosition(const int x, const int y);
	virtual void setSpacing(const int x, const int y);
	virtual void setBackground(Element *);
	virtual void setFace(Element *);
	
    private:
	Mugen::Point position;
	Mugen::Point spacing;
	Element *background;
	Element *face;
};

class Name{
    public:
	Name();
	Name(const int x, const int y);
	virtual ~Name();
	
	virtual void act();
	virtual void render(const int xaxis, const int yaxis, Bitmap &);
	virtual void setPosition(const int x, const int y);
	virtual void setFont(Element *);
	
    private:
	Mugen::Point position;
	Element *font;
};

/*! HUD collection for character data */
class PlayerInfo{
    public:
	PlayerInfo();
	virtual ~PlayerInfo();
	
	virtual void setLifeBar(Bar *);
	virtual void setPowerbar(Bar *);
	virtual void setFace(Face *);
	virtual void setName(Name *);
    
    private:
	Bar *lifeBar;
	Bar *powerBar;
	Face *face;
	Name *name;
	
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
	std::vector<PlayerInfo *> regularTeam1;
	std::vector<PlayerInfo *> regularTeam2;
	std::vector<PlayerInfo *> simultaneousTeam1;
	std::vector<PlayerInfo *> simultaneousTeam2;
	std::vector<PlayerInfo *> turnsTeam1;
	std::vector<PlayerInfo *> turnsTeam2;
	
	/* Time Display */
	
	/* Combo Display */
	
	/* Round and Fight Display */
	
	/* KO Display */
	
	/* Win Icon... I guess this is the markers for wins */
	
	
};

}

#endif

