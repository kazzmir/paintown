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
class FightElement: public Element {
    public:
	FightElement();
	virtual ~FightElement();
	
	virtual void act();
        virtual void render(int x, int y, const Bitmap &);
	virtual void render(const Element::Layer & layer, int x, int y, const Bitmap &, int width);
	virtual void play();
	
	enum ElementType{
	    IS_NOTSET =0,
	    IS_ACTION,
	    IS_SPRITE,
	    IS_FONT,
	    IS_SOUND,
	};
	
	enum DisplayState{
	    NoDisplayTimer,
	    DisplayNotStarted,
	    DisplayStarted,
	    DisplayEnded,
	};
	
	enum SoundState{
	    NoSoundTimer,
	    SoundNotStarted,
	    Waiting,
	    Played,
	};
	
	virtual inline void setType(ElementType t){ type = t; }
	virtual inline bool isSet() const {
	    return (type != IS_NOTSET);
	}

	virtual void setAction(MugenAnimation *);
        virtual inline void setSpriteData(int g, int s){
            this->spriteData.x = g;
            this->spriteData.y = s;
        }

        virtual inline const Mugen::Point & getSpriteData() const {
            return this->spriteData;
        }

	virtual void setSprite(MugenSprite *);
	virtual void setFont(MugenFont *, int bank, int position);
        virtual void setSound(MugenSound *);
	virtual int getWidth();
	virtual int getHeight();
	virtual inline void setOffset(int x, int y){ 
            offset = Mugen::Point(x,y); 
        }

	virtual inline void setDisplayTime(int time){ 
            this->displaytime = time;
	    this->displayState = DisplayNotStarted;
        }
	
	virtual inline bool notStarted(){
	    return (displayState == DisplayNotStarted);
	}
	
	virtual inline bool isDone(){
	    switch (displayState){
		case DisplayNotStarted:
		case NoDisplayTimer:
		case DisplayStarted:
		    return false;
		    break;
		case DisplayEnded:
		default:
		    return true;
	    }
	}

	virtual inline void setSoundTime(int time){
	    if (time == 0){
		return;
	    }
	    this->soundtime = time;
	    this->soundState = SoundNotStarted;
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
        Mugen::Point spriteData;
	MugenSprite *sprite;
	MugenFont *font;
        MugenSound *sound;
	Mugen::Point offset;
	int displaytime;
	int soundtime;
	Effects effects;
	std::string text;
	int bank;
	int position;
	DisplayState displayState;
	SoundState soundState;
	int ticker;
	int soundTicker;
};

//! Base Bar made up of different components
class Bar{
    public:
        //! Pass in the fight.def and the character so that we can initialize to it's max health
	Bar();
	virtual ~Bar();
	
	virtual void act(Character &);
	virtual void render(Element::Layer layer, const Bitmap &);

        enum Type{
            None,
            Health,
            Power,
        };
	
	enum PowerState{
	    Level0,
	    Level1,
	    Level2,
	    Level3,
	};

        virtual inline void setType(Type type){
            this->type = type;
        }
	
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

        virtual inline FightElement & getCounter(){
            return this->counter;
        }
	
	virtual inline FightElement & getLevel1Sound(){
	    return this->level1Sound;
	}
	
	virtual inline FightElement & getLevel2Sound(){
	    return this->level2Sound;
	}
	
	virtual inline FightElement & getLevel3Sound(){
	    return this->level3Sound;
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

        //! Counter for Powerbars
        FightElement counter;
	
	//! Level 1 Sound
	FightElement level1Sound;
	
	//! Level 2 Sound
	FightElement level2Sound;
	
	//! Level 3 Sound
	FightElement level3Sound;

        //! Type of bar defaults to health
        Type type;

        //! Max Health
        int maxHealth;

        //! Current Set Health
        int currentHealth;
        
        //! Damage that has been done, will be reduced to 0 and subtract from current hit points
        int damage;
	
	//! Power Level
	PowerState powerLevel;
};

class Face{
    public:
	Face();
	virtual ~Face();
	
	virtual void act(Character &);
	virtual void render(const Element::Layer & layer, const Bitmap & bmp);
	virtual inline void setPosition(int x, int y){
            position.x = x;
            position.y = y;
        }
	virtual inline void setSpacing(int x, int y){
            spacing.x = x;
            spacing.y = y;
        }
	virtual inline FightElement & getBackground(){
            return this->background;
        }
	virtual inline FightElement & getFace(){
            return this->face;
        }
	
    private:
	Mugen::Point position;
	Mugen::Point spacing;
	FightElement background;
	FightElement face;
};

class Name{
    public:
	Name();
	virtual ~Name();
	
	virtual void act(Mugen::Character & character);
	virtual void render(const Element::Layer &, const Bitmap &);
	virtual void setPosition(int x, int y){
            this->position.x = x;
            this->position.y = y;
        }
	virtual inline FightElement & getBackground(){
	    return this->background;
	}
	virtual inline FightElement & getFont(){
            return this->font;
        }
	
    private:
	Mugen::Point position;
	FightElement background;
	FightElement font;
};

class GameTime{
    public:
	GameTime();
	virtual ~GameTime();
	virtual void act();
	virtual void render(const Element::Layer &, const Bitmap &);
	virtual void start();
	virtual void stop();
	virtual void setPosition(int x, int y){
            this->position.x = x;
            this->position.y = y;
        }
	virtual void setFrameCount(int x){
	    this->frameCount = x;
	}
	virtual inline FightElement & getBackground(){
	    return this->background;
	}
	virtual inline FightElement & getTimer(){
	    return this->timer;
	}
	virtual inline bool hasExpired(){
	    if (time <=0){
		return true;
	    }
	    return false;
	}
    private:
	Mugen::Point position;
	FightElement background;
	FightElement timer;
	int frameCount;
	int time;
	int ticker;
	bool started;
};

class Combo{
    public:
	Combo();
	virtual ~Combo();
	virtual void act(Mugen::Character & character);
	virtual void render(const Element::Layer &, const Bitmap &);
	
	enum Side{
	    Left,
	    Right,
	};
	virtual inline void setSide(Side side){
	    this->side = Left;
	}
	virtual inline void setPosition(int x, int y){
	    position.set(x,y);
	}
	virtual inline void setStartOffset(int x){
	    this->startOffset = x;
	}
	virtual inline void setDisplayTime(int time){
	    this->displayTime = time;
	}
	virtual inline void setShake(bool shake){
	    this->shake = shake;
	}
	virtual inline FightElement & getCombo(){
	    return this->combo;
	}
	virtual inline FightElement & getText(){
	    return this->text;
	}
	virtual inline void setMessage(const std::string & str){
	    this->message = str;
	}
    private:
	enum State{
	    Disabled,
	    Forward,
	    Wait,
	    Retracting,
	};
	Mugen::Point position;
	Mugen::Point currentPosition;
	Side side;
	int startOffset;
	int displayTime;
	int ticker;
	bool shake;
	int shakeTime;
	int total;
	FightElement combo;
	FightElement text;
	std::string message;
	State state;
};

class Round{
    public:
	Round();
	virtual ~Round();
	
	void act(Mugen::Character & player1, Mugen::Character & player2);
	void render(const Element::Layer &, const Bitmap &);
	
	enum State{
	    WaitForIntro,
	    DisplayIntro,
	    WaitForRound,
	    DisplayRound,
	    WaitForFight,
	    DisplayFight,
	    WaitForControl,
	    WaitForOver,
	    DisplayKO,
	    DisplayDoubleKO,
	    DisplayTimeOver,
	};
	
	virtual void setRound(int round){
	    this->currentRound = round;
	}
	
	virtual int getRound() const {
	    return this->currentRound;
	}
	
	virtual void setState(const State & state, Mugen::Character & player1, Mugen::Character & player2);
	
	virtual inline void setMatchWins(int wins){
	    this->matchWins = wins;
	}
	
	virtual inline void setMatchMaxDrawGames(int max){
	    this->matchMaxDrawGames = max;
	}
	
	virtual inline void setStartWaitTime(int time){
	    this->startWaitTime = time;
	}
	
	virtual inline void setPosition(int x, int y){
	    this->position.set(x,y);
	}
	
	virtual inline void setRoundDisplayTime(int time){
	    this->roundDisplayTime = time;
	}
	
	virtual inline FightElement & getDefaultRound(){
	    return this->defaultRound;
	}
	
	virtual inline void setDefaultText(const std::string & text){
	    this->defaultText = text;
	}
	
	virtual inline FightElement & getRoundSound(unsigned int number){
	    return *this->roundSounds[number];
	}
	
	virtual inline FightElement & getRound(unsigned int number){
	    // Max 9 so 0-8
	    return *this->rounds[number];
	}
	
	virtual inline void setFightDisplayTime(int time){
	    this->fightDisplayTime = time;
	}
	
	virtual inline FightElement & getFight(){
	    return this->fight;
	}
	
	virtual inline void setControlTime(int time){
	    this->controlTime = time;
	}
	
    private:
	
	// Current State
	State state;
	
	// Current round
	int currentRound;
	
	int matchWins;
	int matchMaxDrawGames;
	int startWaitTime;
	
	Mugen::Point position;
	//! How long before displaying round
	int roundDisplayTime;
	FightElement defaultRound;
	std::string defaultText;
	//! Round Sounds
	std::vector< FightElement * > roundSounds;
	//! Up to 9 only
	std::vector< FightElement * > rounds;
	
	//! How long before displaying Fight!
	int fightDisplayTime;
	FightElement fight;
	
	//! Time before handling off control to players
	int controlTime;
	
	//! ticker
	int ticker;
};

/*! Player HUD *TODO Need to compensate for team stuff later */
class GameInfo{
    public:
	GameInfo(const std::string & fightFile);
	virtual ~GameInfo();

        virtual void act(Mugen::Character & player1, Mugen::Character & player2);
        virtual void render(Element::Layer layer, const Bitmap &);
	
	enum State{
	    NotStarted,
	    Intro,
	    RoundIndicator,
	    StartGame,
	    KO,
	    Draw,
	    EndGame,
	};
	
	virtual void setState(const State & state, Character & player1, Character & player2);
	
	virtual inline const State & getState(){
	    return this->state;
	}
	
    private:
        
        void parseAnimations(Ast::AstParse & parsed);
	
	//! Player Data
	Bar player1LifeBar;
        Bar player2LifeBar;
	Bar player1PowerBar;
        Bar player2PowerBar;
	Face player1Face;
        Face player2Face;
	Name player1Name;
        Name player2Name;
	
	//! Game Timer
	GameTime timer;
	
	//! Combo
	Combo team1Combo;
	Combo team2Combo;
	
	//! Round
	Round roundControl;
	
	Mugen::SpriteMap sprites;
        std::map<int, MugenAnimation *> animations;
        std::vector<MugenFont *> fonts;
	Mugen::SoundMap sounds;
	
	//! Game State
	State state;
};

}

#endif

