#ifndef _mugen_character_hud_h
#define _mugen_character_hud_h

#include <string>
#include <map>

#include "mugen/mugen_util.h"
#include "mugen/mugen_exception.h"

class Bitmap;
class MugenSprite;
class MugenAnimation;
class MugenFont;
class MugenSound;

namespace Ast{
    class AstParse;
}

namespace Mugen{

class Character;

/*! *TODO implement display time and ticker */
class FightElement : public Element{
    public:
	FightElement();
	virtual ~FightElement();
	
	virtual void act();
	virtual void render(int x, int y, const Bitmap &);
	
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
        virtual void setSound(MugenSound *);
	virtual inline void setOffset(int x, int y){ 
            offset = Mugen::Point(x,y); 
        }
	virtual inline void setDisplayTime(int t){ 
            displaytime = t; 
        }
	virtual inline void setFacing(int f){ 
            effects.facing = f; 
        }
	virtual inline void setVFacing(int f){ 
            effects.vfacing = f; 
        }
	virtual inline void setScale(double x, double y){ 
            effects.scalex = x, effects.scaley = y; 
        }
	virtual inline void setText(const std::string &t){ 
            text = t; 
        }
	
    private:
	ElementType type;
	MugenAnimation *action;
	MugenSprite *sprite;
	MugenFont *font;
        MugenSound *sound;
	Mugen::Point offset;
	int displaytime;
	Effects effects;
	std::string text;
};

//! Base Bar made up of different components
class Bar{
    public:
        //! Pass in the fight.def and the character so that we can initialize to it's max health
	Bar();
	virtual ~Bar();
	
	virtual void act(const Character &);
	virtual void render(Element::Layer layer, const Bitmap &);
	
	virtual inline void setPosition(int x, int y){
	    this->position.x = x;
	    this->position.y = y;
	}
	
	virtual inline void setRange(int x, int y){
	    this->range.x = x;
	    this->range.y = y;
	}
	
        virtual inline FightElement & getBack0(){
            return this->back0;
        }
        virtual inline FightElement & getBack1(){
            return this->back1;
        }
        virtual inline FightElement & getMiddle(){
            return this->middle;
        }
        virtual inline FightElement & getFront(){
            return this->front;
        }
	
    private:
        //! Position of this Bar
	Mugen::Point position;
	
        //! Background 0 of the Bar background behind mid and front
        FightElement back0;
	
        //! Second Background of the Bar usually a container around the bars
	FightElement back1;
	
        //! Third Background of the Bar which is the second bar that decreases by tick
	FightElement middle;

        //! Fourth Background of the Bar 
        FightElement front;
	
        //! Range of the actual bar (range.x * range.y / hitPoints)
	Mugen::Point range;

        //! Max Health
        int maxHealth;

        //! Current Set Health
        int currentHealth;
        
        //! Damage that has been done, will be reduced to 0 and subtract from current hit points
        int damage;
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

/*! Player HUD *TODO Need to compensate for team stuff later */
class PlayerInfo{
    public:
	PlayerInfo(const std::string & fightFile);
	virtual ~PlayerInfo();

        virtual void act(const Mugen::Character & player1, const Mugen::Character & player2);
        virtual void render(Element::Layer layer, const Bitmap &);
    
    private:
        
        void parseAnimations(Ast::AstParse & parsed);

	Bar player1LifeBar;
        Bar player2LifeBar;
	Bar player1PowerBar;
        Bar player2PowerBar;
	Face player1Face;
        Face player2Face;
	Name player1Name;
        Name player2Name;
	
	Mugen::SpriteMap sprites;
        std::map<int, MugenAnimation *> animations;
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

