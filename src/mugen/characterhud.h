#ifndef _mugen_character_hud_h
#define _mugen_character_hud_h

#include <string>

#include "mugen/mugen_util.h"

class Bitmap;
class MugenSprite;
class MugenAnimation;
class MugenFont;
class Token;

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

/*! Character HUD ... lifebar, face, etc */
class CharacterHUD
{
    public:
	CharacterHUD(Token *token);
	virtual ~CharacterHUD();
	
    private:
	/* Player 1 */
	Bar *p1LifeBar;
	Bar *p1PowerBar;
	Face *p1Face;
	Name *p1Name;
	
	/* Player 2 */
	Bar *p2LifeBar;
	Bar *p2PowerBar;
	Face *p2Face;
	Name *p2Name;
	
	/* Player 3 */
	Bar *p3LifeBar;
	Bar *p3PowerBar;
	Face *p3Face;
	Name *p3Name;
	
	/* Player 4 */
	Bar *p4LifeBar;
	Bar *p4PowerBar;
	Face *p4Face;
	Name *p4Name;
};

}

#endif

