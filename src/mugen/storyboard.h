#ifndef _mugen_storyboard_h
#define _mugen_storyboard_h

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "mugen/mugen_exception.h"
#include "mugen/mugen_fadetool.h"
#include "mugen/mugen_util.h"

#include "input/input-map.h"

class Bitmap;
class MugenSprite;
class MugenSound;
class MugenAnimation;

namespace Mugen{
 
class Background;


class Layer{
    public:
	Layer();
	virtual ~Layer();
        virtual void act(int currentTime);
        virtual void render(int x, int y, const Bitmap &);
        virtual void reset();
        virtual inline void setStartTime(int time){
            this->startTime = time;
        }
        virtual inline void setAnimation(MugenAnimation * animation){
            this->animation = animation;
        }
        //! Override
        virtual inline void setEnabled(bool enabled){
            this->enabled = enabled;
        }
    private:
	Mugen::Point offset;
	int startTime;
        bool enabled;
	MugenAnimation *animation;
};

class Scene {
    public:
	Scene(Ast::Section * data, const std::string & storyBoardFile, SpriteMap & sprites);
	virtual ~Scene();
	
        virtual void act();
	
	virtual void render(const Bitmap &);
	
	virtual bool isDone();
	
	virtual void reset();
	
	virtual inline const MugenFadeTool & getFadeTool() const {
	    return this->fader;
	}
	
	virtual inline int getTicker() const {
	    return this->ticker;
	}
	
	virtual inline int getEndTime() const {
	    return this->endTime;
	}

    private:
	
        //! fade tool
	MugenFadeTool fader;
	
        //! Clear color for the scene
	int clearColor;
        //! Ticker for the scene
	int ticker;
        //! End time for this sceen
	int endTime;
	
        //! Default position
	Mugen::Point defaultPosition;
	
        //! Background name for given scene
	std::string backgroundName;
	
        //! Background
	Mugen::Background *background;
	
	//! layers
	std::vector<Layer *>layers;
};

class Storyboard {
    public:
	//! Location at dataPath() + "mugen/stages/"
	Storyboard( const std::string & s ) throw (MugenException);

	virtual ~Storyboard();
	
	//! give it the default bitmap to blit to the screen
	virtual void run(const Bitmap &, bool repeat = false);

        virtual void setInput(const InputMap<CharacterKeys> & input){
            this->input = input;
        }
	
    private:
        //! File
        std::string storyBoardFile;
	
        //! Beginning Scene Number
	int startscene;
	
	//! Sprites
        Mugen::SpriteMap sprites;
	
	//! Animation Lists stored by action number
	std::map< int, MugenAnimation * > animations;
	
	//! Scenes
	std::vector<Scene *>scenes;

        //! Input
        InputMap<CharacterKeys> input;
    
};

}

#endif


