#ifndef mugen_animation_h
#define mugen_animation_h

#include <string>
#include <vector>

#include "state.h"
#include "util.h"

namespace Graphics{
class Bitmap;
}
class MugenSprite;

/*
Collision Area
*/
class MugenArea{
public:
    MugenArea();
    MugenArea( const MugenArea &copy );
    virtual ~MugenArea();

    bool collision(int mx, int my, const MugenArea & area, int ax, int ay) const;
    
    MugenArea & operator=( const MugenArea &copy );
    
    int x1,y1,x2,y2;
};

/*
Frame
*/
class MugenFrame{
    public:
	MugenFrame(bool defaultMask);
	MugenFrame( const MugenFrame &copy );
	virtual ~MugenFrame();
	
	MugenFrame & operator=( const MugenFrame &copy );
	
	virtual void render(int x, int y, const Graphics::Bitmap & work, const Mugen::Effects & effects);

        virtual inline const std::vector<MugenArea> & getDefenseBoxes() const {
            return defenseCollision;
        }
        
        virtual inline const std::vector<MugenArea> & getAttackBoxes() const {
            return attackCollision;
        }

        virtual inline MugenSprite * getSprite() const {
            return sprite;
        }

        virtual void setSprite(MugenSprite * sprite);

	// We'll keep them, but they probably won't be used
	std::vector< MugenArea > defenseCollision;
	// This is the only one will be worried about
	std::vector< MugenArea > attackCollision;
	// Is this frame a loopstart position?
	bool loopstart;
	// This is the sprite 
	MugenSprite *sprite;
	// Additional Offsets which are in character.air (ie x + sprite.x)
	int xoffset;
	int yoffset;
	// Time in ticks to display -1 for infinite
	int time;
	// flips moved to Effects
	// Flip horizontal?
	//bool flipHorizontal;
	// Flip Vertical
	//bool flipVertical; 
	/*Color addition (need to decipher their shorthand crap like A1)
	15,4, 0,0, 5, ,A   ;<-- Color addition (flip parameter omitted)
	15,4, 0,0, 5, H, S ;<-- Flips horizontally and does color subtraction

	If you wish to specify alpha values for color addition, use the
	parameter format "AS???D???", where ??? represents the values of the
	source and destination alpha respectively. Values range from 0 (low)
	to 256 (high). For example, "AS64D192" stands for "Add Source_64 to
	Dest_192". Also, "AS256D256" is equivalent to just "A". A shorthand
	for "AS256D128" is "A1".

	15,4, 0,0, 5, ,A1  ;<-- Color addition to 50% darkened dest
	15,4, 0,0, 5, ,AS128D128 ;<-- Mix 50% source with 50% dest
	*/
	//std::string colorAdd;
	Mugen::Effects effects;
	//int colorSource;
	//int colorDestination;
};

/*
 * Holds mugen animations, ie: player.air
 */
class MugenAnimation{
    public:
	MugenAnimation();
	MugenAnimation( const MugenAnimation &copy );
	virtual ~MugenAnimation();
	
	// Get next Frame
	const MugenFrame *getNext();
	
        virtual void reset();

        virtual MugenAnimation * copy() const;
	
	// Add a frame
	void addFrame( MugenFrame * );

        virtual inline unsigned int getPosition() const {
            return position;
        }

        virtual void setPosition(int position);

        virtual inline int getTicks() const {
            return ticks;
        }
	
	// Get name of type of animation
	static const std::string getName(const Mugen::AnimationType t);
	
	// Update logic | go through frames
	void logic();
	
	// Render current frame optionally scale defaults to 1
	void render(int xaxis, int yaxis, const Graphics::Bitmap &work, double scalex=1, double scaley=1, bool ownPalette = false);
        void render(int xaxis, int yaxis, const Graphics::Bitmap & work, const Mugen::Effects & effects);
	
	// Render current frame overriding flipping
	void render( bool facing, bool vfacing, const int xaxis, const int yaxis, const Graphics::Bitmap &work, const double scalex = 1, const double scaley = 1, Graphics::Bitmap::Filter * filter = NULL, bool ownPalette = false);

        /* automatically sets the effect trans type to ADDALPHA */
	void renderReflection(bool facing, bool vfacing, int alpha, const int xaxis, const int yaxis, const Graphics::Bitmap &work, const double scalex = 1, const double scaley = 1);

        virtual const std::vector<MugenArea> getDefenseBoxes(bool reverse) const;
        virtual const std::vector<MugenArea> getAttackBoxes(bool reverse) const;
	
	// Go forward a frame 
	void forwardFrame();
	// Go back a frame
	void backFrame();
	
	// Reload bitmaps for things like palettes
	void reloadBitmaps();
	
	inline unsigned int getCurrentPosition() { return position; }
	inline MugenFrame *getCurrentFrame(){ return frames[position]; }
        Mugen::Effects getCurrentEffects(bool facing, bool vfacing, double scalex, double scaley);
	
	// Set type number
	inline void setType(const Mugen::AnimationType t){ type = t; }
	
	// Get type number
	inline Mugen::AnimationType getType(){ return type; }
	
	// Show offense | defense Clsn framework
	inline void toggleDefense(){ showDefense = !showDefense; }
	inline void toggleOffense(){ showOffense = !showOffense; }
	
	inline bool showingDefense() const { return showDefense; }
	inline bool showingOffense() const { return showOffense; }

        inline bool hasLooped() const {
            return looped;
        }
	
        inline const std::vector<MugenFrame*> & getFrames() const {
            return frames;
        }

        int animationTime() const;
        int animationElementElapsed(int position) const;
	
	virtual inline void setAsOneTime(bool time){
	    this->playOnce = time;
	}
	
	virtual inline bool hasStarted() const {
	    return (this->playOnce && this->started);
	}
	
	virtual inline void play(){
	    this->position = 0;
            this->started = true;
	}
	
	virtual inline bool isDone(){
	    return (position == frames.size() -1);
	}

    protected:

        void renderFrame(MugenFrame * frame, int xaxis, int yaxis, const Graphics::Bitmap & work, const Mugen::Effects & effects);
	
    private:
	
	std::vector< MugenFrame * > frames;
	
	unsigned int loopPosition;
	unsigned int position;
	
	bool playOnce;
        bool looped;
	bool started;
	
        Mugen::AnimationType type;
	
	bool showDefense;
	bool showOffense;
	
        /* incremented for each game tick as long as the animation is not
         * in an infinite loop time frame (-1)
         */
	int ticks;

        /* incremented when the animation specifies a looptime of -1 */
        int virtual_ticks;
};

#endif
