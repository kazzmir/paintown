#ifndef _mugen_character_hud_h
#define _mugen_character_hud_h

#include <string>

#include "mugen/mugen_util.h"

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
	virtual inline void setDisplayTime(const int t) { displaytime = t; }
	virtual inline void setFacing(const int f) { facing = f; }
	virtual inline void setVFacing(const int f) { vfacing = f; }
	virtual inline void setLayer(const int i) { layerno = i; }
	virtual inline void setScale(const double x, const double y) { scalex = x, scaley = y; }
	virtual inline void setText(const std::string &t) { text = t; }
	
	virtual inline int getLayer() { return layerno; }
	
    private:
	ElementType type;
	MugenAnimation *action;
	MugenSprite *sprite;
	MugenFont *font;
	Mugen::Point offset;
	int displaytime;
	int facing;
	int vfacing;
	int layerno;
	double scalex;
	double scaley;
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
	
	virtual void act();
	virtual void render(const int xaxis, const int yaxis, Bitmap &);
	
    private:
	Mugen::Point position;
	Element *back0;
	Element *back1;
	Element *back2;
	Element *middle;
	Element *front;
};

/*! Character HUD ... lifebar, face, etc */
class CharacterHUD
{
    public:
	CharacterHUD();
	virtual ~CharacterHUD();
	
    private:
	/*
	[Lifebar]
;Player 1
p1.pos    = 140,12
p1.bg0.anim = 10
p1.bg1.spr = 11,0
p1.mid.spr = 12,0
p1.front.spr = 13,0
p1.range.x  = 0,-127
;Player 2
p2.pos    = 178,12
p2.bg0.anim = 10
p2.bg0.facing = -1
p2.bg1.spr = 11,0
p2.bg1.facing = -1
p2.mid.spr = 12,0
p2.mid.facing = -1
p2.front.spr = 13,0
p2.front.facing = -1
p2.range.x = 0,127

[Begin Action 10] ;Darkened lifebar background 
10,0, 0,0, 1, , S

[Simul Lifebar]
;Player 1
p1.pos    = 140,12
p1.bg0.anim = 20
p1.bg1.spr = 21,0
p1.mid.offset = 0,-1
p1.mid.spr = 22,0
p1.front.offset = 0,-1
p1.front.spr = 23,0
p1.range.x  = 0,-127
;Player 2
p2.pos    = 178,12
p2.bg0.anim = 20
p2.bg0.facing = -1
p2.bg1.spr = 21,0
p2.bg1.facing = -1
p2.mid.offset = 0,-1
p2.mid.spr = 22,0
p2.mid.facing = -1
p2.front.offset = 0,-1
p2.front.spr = 23,0
p2.front.facing = -1
p2.range.x = 0,127
;Player 3
p3.pos    = 140,15
p3.mid.spr = 22,0
p3.front.spr = 23,0
p3.range.x  = 0,-127
;Player 4
p4.pos    = 178,15
p4.mid.spr = 22,0
p4.mid.facing = -1
p4.front.spr = 23,0
p4.front.facing = -1
p4.range.x = 0,127

[Begin Action 20] ;Darkened lifebar background (team)
20,0, 0,0, 1, , S

[Turns Lifebar]
;Player 1
p1.pos    = 140,12
p1.bg0.anim = 10
p1.bg1.spr = 11,0
p1.mid.spr = 12,0
p1.front.spr = 13,0
p1.range.x  = 0,-127
;Player 2
p2.pos    = 178,12
p2.bg0.anim = 10
p2.bg0.facing = -1
p2.bg1.spr = 11,0
p2.bg1.facing = -1
p2.mid.spr = 12,0
p2.mid.facing = -1
p2.front.spr = 13,0
p2.front.facing = -1
p2.range.x = 0,127

;-----------------------------------------------------------
[Powerbar]
;Player 1
p1.pos    = 140,22
p1.bg0.anim = 40
p1.bg1.spr = 41,0
;p1.mid.spr = 42,0
p1.front.spr = 43,0
p1.range.x = 0,-107
p1.counter.offset = -108,6
p1.counter.font = 1,0, 0

;Player 2
p2.pos    = 178,22
p2.bg0.anim = 40
p2.bg0.facing = -1
p2.bg1.spr = 41,0
p2.bg1.facing = -1
;p2.mid.spr = 42,0
;p2.mid.facing = -1
p2.front.spr = 43,0
p2.front.facing = -1
p2.range.x  = 0,107
p2.counter.offset = 109,6
p2.counter.font = 1,0, 0

; --level sounds---
level1.snd = 21,0
level2.snd = 21,0
level3.snd = 21,0

[Begin Action 40] ;Darkened powerbar background 
40,0, 0,0, 1, , S


;-----------------------------------------------------------
[Face]
;Player 1
p1.pos    = 2,12
p1.bg.spr = 50,0
p1.face.spr = 9000,0
p1.face.facing = 1
p1.face.offset = 0,10
;Player 2
p2.pos    = 316,12
p2.bg.spr = 50,0
p2.bg.facing = -1
p2.face.spr = 9000,0
p2.face.facing = -1
p2.face.offset = 0,10

[Simul Face]
;Player 1
p1.pos    = 1,12
p1.bg.spr = 60,0
p1.face.spr = 9000,0
p1.face.scale = 0.6,0.6
p1.face.facing = 1
p1.face.offset = 0,10
;Player 2
p2.pos    = 317,12
p2.bg.spr = 60,0
p2.bg.facing = -1
p2.face.spr = 9000,0
p2.face.scale = 0.6,0.6
p2.face.facing = -1
p2.face.offset = 0,10
;Player 3
p3.pos    = 2,12
p3.face.spr = 9000,0
p3.face.scale = 0.6,0.6
p3.face.facing = 1
p3.face.offset = 11,10
;Player 4
p4.pos    = 316,12
p4.bg.facing = -1
p4.face.spr = 9000,0
p4.face.scale = 0.6,0.6
p4.face.facing = -1
p4.face.offset = -11,10

[Turns Face]
;Player 1
p1.pos    = 2,12
p1.bg.spr = 50,0
p1.face.spr = 9000,0
p1.face.facing = 1
p1.face.offset = 0,10
p1.teammate.pos = 1,49
p1.teammate.spacing = 13,0
p1.teammate.bg.spr = 70,0
p1.teammate.ko.spr = 71,0
p1.teammate.face.spr = 9000,0
p1.teammate.face.scale = 0.5,0.53
p1.teammate.face.facing = 1
p1.teammate.face.offset = 1,0
;Player 2
p2.pos    = 316,12
p2.bg.spr = 50,0
p2.bg.facing = -1
p2.face.spr = 9000,0
p2.face.facing = -1
p2.face.offset = 0,10
p2.teammate.pos = 317,49
p2.teammate.spacing = -13,0
p2.teammate.bg.spr = 70,0
p2.teammate.bg.facing = -1
p2.teammate.ko.spr = 71,0
p2.teammate.ko.facing = -1
p2.teammate.face.spr = 9000,0
p2.teammate.face.scale = 0.5,0.53
p2.teammate.face.facing = -1
p2.teammate.face.offset = -1,0

;-----------------------------------------------------------
[Name]
p1.pos = 14,10
;p1.bg. =
p1.name.font = 3,0, 1
;p2.bg. =
p2.pos = 305,10
p2.name.font = 3,0, -1

[Simul Name]
p1.pos = 14,10
p1.name.font = 3,0, 1
p2.pos = 305,10
p2.name.font = 3,0, -1
p3.pos = 138,10
p3.name.font = 3,0, -1
p4.pos = 181,10
p4.name.font = 3,0, 1

[Turns Name]
p1.pos = 14,10
;p1.bg. =
p1.name.font = 3,0, 1
;p2.bg. =
p2.pos = 305,10
p2.name.font = 3,0, -1
	*/
	    
};

}

#endif

