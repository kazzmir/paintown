#ifndef _mugen_character_hud_h
#define _mugen_character_hud_h

#include <string>
#include <map>

#include "util.h"
#include "exception.h"
#include "util/gui/fadetool.h"
#include "character.h"
#include "behavior.h"

namespace Graphics{
class Bitmap;
}
class MugenSprite;
class MugenAnimation;
class MugenFont;
class MugenSound;

namespace Ast{
    class AstParse;
}

namespace Mugen{

class Character;
class Stage;

/*! *TODO implement display time and ticker */
class FightElement: public Element {
    public:
	FightElement();
	virtual ~FightElement();
	
	virtual void act();
        virtual void render(int x, int y, const Graphics::Bitmap &, Graphics::Bitmap::Filter * filter = NULL);
	virtual void render(const Element::Layer & layer, int x, int y, const Graphics::Bitmap &, int width);
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
        virtual inline void setPosition(int position){
            this->position = position;
        }
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
	
	virtual bool notStarted();
	
	virtual bool isDone();

        virtual void reset();

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
	virtual void render(Element::Layer layer, const Graphics::Bitmap &);

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
        /* time to wait before adjusting damage */
        int wait;
	
	//! Power Level
	PowerState powerLevel;
};

class Face{
    public:
	Face();
	virtual ~Face();
	
	virtual void act(Character &);
	virtual void render(const Element::Layer & layer, const Graphics::Bitmap & bmp);
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
	virtual void render(const Element::Layer &, const Graphics::Bitmap &);
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
	virtual void render(const Element::Layer &, const Graphics::Bitmap &);
	virtual void start();
	virtual void stop();
	virtual void reset();
	virtual inline int getElapsedTicks(){
	    return this->ticker;
	}
	virtual inline bool isStarted() const {
	    return started;
	}
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
	    if (time <=0 && !disabled){
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
        bool disabled;
};

class Combo{
    public:
	Combo();
	virtual ~Combo();
	virtual void act(Mugen::Character & player1, Mugen::Character & player2);
	virtual void render(const Element::Layer &, const Graphics::Bitmap &);
	
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
	Mugen::Point player1Position;
        Mugen::Point player2Position;
	int startOffset;
	int displayTime;
	int player1Ticker;
        int player2Ticker;
	bool shake;
	int player1ShakeTime;
        int player2ShakeTime;
	int player1Total;
        int player2Total;
	FightElement combo;
	FightElement text;
	std::string message;
	State player1State;
        State player2State;
        std::string player1Combo;
        std::string player1Text;
        std::string player2Combo;
        std::string player2Text;
};

class Round{
    public:
	Round();
	virtual ~Round();
	
	virtual void act(Mugen::Stage & stage, Mugen::Character & player1, Mugen::Character & player2);
	virtual void render(const Element::Layer &, const Graphics::Bitmap &);
        virtual void reset(Mugen::Stage & stage, Mugen::Character & player1, Mugen::Character & player2);
	
	enum State{
	    WaitForIntro,
	    DisplayIntro,
	    WaitForRound,
	    DisplayRound,
	    WaitForFight,
	    DisplayFight,
	    PlayingGame,
	    RoundOver,
	    DoTimeOver,
            DoWin,
            MatchOver,
	};
	
	virtual void setRound(int round){
	    this->currentRound = round;
	}
	
	virtual int getRound() const {
	    return this->currentRound;
	}
	
	virtual void setState(const State & state, Mugen::Stage & stage, Mugen::Character & player1, Mugen::Character & player2);
	
	virtual const State & getState() const {
	    return this->state;
	}
	
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
	
	virtual FightElement & getRoundSound(unsigned int number);

        virtual void setRoundSoundTime(int time);
	
	virtual FightElement & getRound(unsigned int number);
	
	virtual inline void setFightDisplayTime(int time){
	    this->fightDisplayTime = time;
	}
	
	virtual inline FightElement & getFight(){
	    return this->fight;
	}
	
	virtual inline FightElement & getFightSound(){
	    return this->fightSound;
	}
	
	virtual inline void setControlTime(int time){
	    this->controlTime = time;
	}
	
	virtual inline void setKODisplayTime(int time){
	    this->KODisplayTime = time;
	}
	
	virtual inline FightElement & getKO(){
	    return this->KO;
	}
	
	virtual inline FightElement & getKOSound(){
	    return this->KOSound;
	}
	
	virtual inline FightElement & getDKO(){
	    return this->DKO;
	}
	
	virtual inline FightElement & getDKOSound(){
	    return this->DKOSound;
	}
	
	virtual inline FightElement & getTO(){
	    return this->TO;
	}
	
	virtual inline FightElement & getTOSound(){
	    return this->TOSound;
	}

        virtual inline void setSlowTime(int time){
            this->slowTime = time;
        }

        virtual inline void setOverWaitTime(int time){
            this->overWaitTime = time;
        }

        virtual inline void setOverHitTime(int time){
            this->overHitTime = time;
        }

        virtual inline void setOverWinTime(int time){
            this->overWinTime = time;
        }

        virtual inline void setOverTime(int time){
            this->overTime = time;
        }

        virtual inline void setWinDisplayTime(int time){
            this->winDisplayTime = time;
        }

        virtual inline FightElement & getWin(){
	    return this->win;
	}
	
	virtual inline FightElement & getWinSound(){
	    return this->winSound;
	}

        virtual inline void setWinText(const std::string & text){
            this->winText = text;
        }

        virtual inline FightElement & getWin2(){
	    return this->win2;
	}
	
	virtual inline FightElement & getWin2Sound(){
	    return this->win2Sound;
	}
        
        virtual inline void setWin2Text(const std::string & text){
            this->win2Text = text;
        }

        virtual inline FightElement & getDraw(){
	    return this->draw;
	}
	
	virtual inline FightElement & getDrawSound(){
	    return this->drawSound;
	}

        virtual inline bool isRoundOver() const {
            return this->roundEnd;
        }

    private:

        //! Get current stage element in use
        FightElement & getRoundElement();
        FightElement & getRoundSoundElement();
	
	// Current State
	State state;
		
	// Current round
	int currentRound;

        // Total draws
        int totalDraws;
		
	int matchWins;
	int matchMaxDrawGames;
	int startWaitTime;
		
	Mugen::Point position;
	//! How long before displaying round
	int roundDisplayTime;
	FightElement defaultRound;
	FightElement defaultRoundSound;
	std::string defaultText;
		
	//! Round Sounds
	std::map< unsigned int, FightElement * > roundSounds;
	//! Up to 9 only
	std::map< unsigned int, FightElement * > rounds;
		
	//! How long before displaying Fight!
	int fightDisplayTime;
	FightElement fight;
	FightElement fightSound;
		
	//! Time before handling off control to players
	int controlTime;
		
	//! How long before display KO/DKO/TO
	int KODisplayTime;

        //! Was this time over or KO
        bool overByKO;

	//! KO Component
        FightElement KO;
	FightElement KOSound;
		
	//! DKO Component
	FightElement DKO;
	FightElement DKOSound;
		
	//! TO Component
	FightElement TO;
	FightElement TOSound;

        //! Slow time when KO'd
        int slowTime;
        
        //! How long to wait before control is removed from characters
        int overWaitTime;
        //! How long to wait to allow chars to hit each other
        int overHitTime;
        //! Time before setting win state
        int overWinTime;
        //! Time to wait before round ends
        int overTime;

        //! How Long before Displaying Win/Win2/Draw
        int winDisplayTime;

        //! Win Component
        FightElement win;
        FightElement winSound;
        std::string winText;

        //! Win2 Component
        FightElement win2;
        FightElement win2Sound;
        std::string win2Text;

        //! Draw Component
        FightElement draw;
        FightElement drawSound;
        
        //! In the Round end states
        bool roundEnd;
		
	//! In the show win state
	bool winStateSet;
		
	//! ticker
	int ticker;
        
        // Fadebetween states
        Gui::FadeTool fader;

        //! Player behaviors
        Behavior * player1Behavior;
        Behavior * player2Behavior;
        DummyBehavior dummyBehavior;

};

class WinIcon{
    public:
        WinIcon();
	virtual ~WinIcon();
	    
	virtual void act(const Mugen::Character &, const Mugen::Character &);
	virtual void render(const Element::Layer &, const Graphics::Bitmap &);
	
        virtual inline void setPlayer1Position(int x, int y){
	    this->player1Position.set(x,y);
	}
	    
	virtual inline void setPlayer2Position(int x, int y){
	    this->player2Position.set(x,y);
	}
	    
	virtual inline void setPlayer1Offset(int x, int y){
	    this->player1Offset.set(x,y);
	}
	    
	virtual inline void setPlayer2Offset(int x, int y){
	    this->player2Offset.set(x,y);
	}
	    
	virtual inline FightElement & getPlayer1Counter(){
	    return this->player1Counter;
	}
	    
	virtual inline FightElement & getPlayer2Counter(){
	    return this->player2Counter;
	}
	    
	virtual FightElement &getPlayer1Win(const WinGame::WinType &);
	   
	virtual FightElement &getPlayer2Win(const WinGame::WinType &);
	    
	virtual inline void setUseIconUpTo(int number){
	    this->useIconUpTo = number;
	}
		
    private:

        //! Current characters
	std::vector<WinGame> player1Wins;
	std::vector<WinGame> player2Wins;
	    
	//! Starting position for players
	Mugen::Point player1Position;
	Mugen::Point player2Position;
	    
	//! Incremental Offset for each icon
	Mugen::Point player1Offset;
	Mugen::Point player2Offset;
	    
	//! Counter font
	FightElement player1Counter;
	FightElement player2Counter;
	   
	//! Win icons
	std::map<WinGame::WinType, FightElement *> player1Icons;
	    
	//! Win icons
	std::map<WinGame::WinType, FightElement *> player2Icons;
	    
	//! Use icons limit, once exceeded number will be printed
	unsigned int useIconUpTo;    
};

/*! Player HUD *TODO Need to compensate for team stuff later */
class GameInfo{
    public:
	GameInfo(const Filesystem::AbsolutePath & fightFile);
	virtual ~GameInfo();

        virtual void act(Mugen::Stage & stage, Mugen::Character & player1, Mugen::Character & player2);
        virtual void render(const Element::Layer &, const Graphics::Bitmap &);

        virtual void reset(Mugen::Stage & stage, Mugen::Character & player1, Mugen::Character & player2);
	
	virtual inline int getGameTime(){
	    return this->timer.getElapsedTicks();
	}

        virtual inline const Round & getRound() const {
            return roundControl;
        }
	
    private:
        
        void parseAnimations(const PaintownUtil::ReferenceCount<Ast::AstParse> & parsed);
	
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
	Combo combo;
	
	//! Round
	Round roundControl;
	
	//! Win Icon
	WinIcon winIconDisplay;
	
	Mugen::SpriteMap sprites;
	std::map<int, MugenAnimation *> animations;
	std::vector<MugenFont *> fonts;
	Mugen::SoundMap sounds;
};

}

#endif

