#ifndef mugen_animation_h
#define mugen_animation_h

#include <string>
#include <vector>

class Bitmap;
class MugenSprite;

enum ColorType {
    NO = 0,
    ADD,
    SUB
};

/*
Collision Area
*/
class MugenArea{
public:
    MugenArea();
    MugenArea( const MugenArea &copy );
    ~MugenArea();
    
    MugenArea & operator=( const MugenArea &copy );
    
    int x1,y1,x2,y2;
};

/*
Frame
*/
class MugenFrame{
    public:
	MugenFrame();
	MugenFrame( const MugenFrame &copy );
	virtual ~MugenFrame();
	
	MugenFrame & operator=( const MugenFrame &copy );

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
	// Flip horizontal?
	bool flipHorizontal;
	// Flip Vertical
	bool flipVertical;
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
	ColorType colorAdd;
	int colorSource;
	int colorDestination;
	
	Bitmap *bmp;
};


/* Our animation definitions */
enum MugenAnimationType{
    STANDING =	0, 	//         Standing
    STAND_TURNING = 5, 	//        Stand turning                Must have finite looptime
    CROUCH_TURNING = 6,  //       Crouch turning               Must have finite looptime
    STAND_TO_CROUCH =10, //       Stand to crouch              Finite looptime
    CROUCHING =11,	//        Crouching
    CROUCH_TO_STAND =12, //       Crouch to stand              Finite looptime
    WALKING_FORWARDS =20, //       Walking forwards
    WALKING_BACKWARDS =21, //       Walking backwards
    JUMP_START =40,	//        Jump start (on ground)       Shown just before player leaves ground
    JUMP_NEUTRAL_UP =41, //Jump neutral (upwards)       Shown when player is going up
    JUMP_FORWARDS_UP = 42, //Jump forwards (upwards)      Shown when player is going up-towards
    JUMP_BACK_UP =43, //Jump back (upwards)          Shown when player is going up-back
    JUMP_NEUTRAL_DOWN =44, //  opt   Jump neutral (downwards)     Activated when Y-velocity > -2
    JUMP_FORWARDS_DOWN=45, //  opt   Jump fwd (downwards)         Same as above
    JUMP_BACK_DOWN=46, //  opt   Jump back (downwards)        Same as above
    JUMP_LANDING=47, //Jump landing                 Shown as player lands on ground
    RUN_FORWARDS=100, //       Run fwd/hop forward
    HOP_BACK=105, //       Hop back	
    START_GUARD_STAND=120, //       Start guarding (stand), //       Finite looptime
    START_GUARD_CROUCH=121, //       Start guarding (crouch)      Finite looptime
    START_GUARD_AIR=122, //       Start guarding (air), //         Finite looptime
    GUARD_STAND=130, //       Guard (stand)
    GUARD_CROUCH=131, //       Guard (crouch)
    GUARD_AIR=132, //       Guard (air)
    STOP_GUARD_STAND=140, //       Stop guarding (stand)        Finite looptime
    STOP_GUARD_CROUCH=141, //       Stop guarding (crouch)       Finite looptime
    STOP_GUARD_AIR=142, //       Stop guarding (air)          Finite looptime
    GUARD_HIT_STAND=150, //       Guarding a hit (stand)       Finite looptime
    GUARD_HIT_CROUCH=151, //       Guarding a hit (crouch)      Finite looptime
    GUARD_HIT_AIR=152, //       Guarding a hit (air)         No loop
    LOSE=170, // opt   Lose                         (See Note 1)
    DRAW=175, // opt   Time Over drawgame           (See Note 1)
    WIN=180, // opt   Win                          No loop (181-189 for multiple) (See Note 1)
    INTRO=190, // opt   Intro                        No loop (See Note 1)
    TAUNT=195, // opt   Taunt                        Finite looptime (See Note 1)
    HIT_HIGH_LIGHT=5000, //       Stand/Air Hit high (light)   Looptime around 10-20
    HIT_HIGH_MEDIUM=5001, //      Stand/Air Hit high (medium)    "     (See Note 2)
    HIT_HIGH_HARD=5002, //       Stand/Air Hit high (hard), //       "
    STAND_RECOVER_HIGH_LIGHT=5005, //       Stand Recover high (light)   No loop (See Note 3)
    STAND_RECOVER_HIGH_MEDIUM=5006, //       Stand Recover high (medium)    "
    STAND_RECOVER_HIGH_HARD=5007, //       Stand Recover high (hard), //       "
    HIT_LOW_LIGHT=5010, //       Stand/Air Hit low (light)    Looptime around 10-20
    HIT_LOW_MEDIUM=5011, //       Stand/Air Hit low (medium)     "
    HIT_LOW_HARD=5012, //       Stand/Air Hit low (hard), //        "
    STAND_RECOVER_LOW_LIGHT=5015, //       Stand Recover low (light)    No loop
    STAND_RECOVER_LOW_MEDIUM=5016, //       Stand Recover low (medium)     "
    STAND_RECOVER_LOW_HARD=5017, //       Stand Recover low (hard), //        "
    CROUCH_HIT_LIGHT=5020, //       Crouch Hit (light), //            Looptime around 10-20
    CROUCH_HIT_MEDIUM=5021, //       Crouch Hit (medium), //       , //       "
    CROUCH_HIT_HARD=5022, //       Crouch Hit (hard), //       , //         "
    CROUCH_RECOVER_LIGHT=5025, //       Crouch Recover (light), //        No loop
    CROUCH_RECOVER_MEDIUM=5026, //       Crouch Recover (medium), //         "
    CROUCH_RECOVER_HARD=5027, //       Crouch Recover (hard), //           "
    HIT_BACK=5030, //       Stand/Air Hit back, //            Looptime around 10-20
    HIT_TRANSITION=5035, // opt  Stand/Air Hit transition     Looptime around 5-15 (See Note 3)
    AIR_RECOVER=5040, //       Air Recover, //       , //       , //       No loop
    AIR_FALL=5050, //       Air Fall, //       , //       , //          No loop
    AIR_FALL_COMING_DOWN=5060, // opt  Air Fall (coming down), //        No loop
    TRIPPED=5070, //       Tripped
    LIEDOWN_HIT_STAYDOWN=5080, //       LieDown Hit (stay down), //       (See Note 4)
    LIEDOWN_HIT_INTOAIR=5090, //       LieDown Hit (hit up into air)
    HIT_GROUND_FROM_FALL=5100, //       Hitting ground from fall     Looptime around 3
    BOUNCE_INTO_AIR=5160, //       Bounce into air
    HIT_GROUND_FROM_BOUNCE=5170, //       Hit ground from bounce, //        Looptime around 3 or 4
    LIEDOWN=5110, //       LieDown
    GET_UP_FROM_LIEDOWN=5120, //       Get up from LieDown
    LIEDEAD_FIRST_ROUNDS=5140, // opt  LieDead (first rounds)
    LIEDEAD_FINAL_ROUND=5150, // opt  LieDead (final round)
    FALL_RECOVERY_NEAR_GROUND=5200, //       Fall-recovery near ground
    FALL_RECOVERY_MIDAIR=5210, //       Fall-recovery in mid-air
    DIZZY=5300, //       Dizzy
    CONTINUE=5500, // opt  "Continue?" screen           If omitted, plays dizzy anim
    YES_CONTINUE=5510, // opt  "Yes" to "Continue"          If omitted, plays first win anim (not yet implemented)
    NO_CONTINUE=5520, // opt  "No" to "Continue"           (not yet implemented)
    AIR_FALL_HIT_UP=5051, // opt  Air fall -- hit up
    FALL_FROM_HIT_UP=5061, // opt  Coming down from hit up
    LIEDOWN_HIT_STAYDOWN2=5081, // opt  LieDown Hit (stay down) A
    BOUNCE_FROM_GROUND_INTO_AIR=5101, // opt  Bounce from ground into air  Looptime around 3
    BOUNCE_INTO_AIR2=5161, // opt  Bounce into air
    HIT_GROUND_FROM_BOUNCE2=5171, // opt  Hit ground from bounce       Looptime around 3 or 4
    LIEDOWN2=5111, // opt  LieDown
    GET_UP_FROM_LIEDOWN2=5121, // opt  Get up from LieDown
    LIEDEAD_FIRST_ROUNDS2=5151, // opt  LieDead (first rounds)
    LIEDEAD_FINAL_ROUND2=5156, // opt  LieDead (final round)
    AIR_FALL_HIT_UP2=5052, // opt  Air fall -- hit up
    FALL_FROM_HIT_UP2=5062, // opt  Coming down from hit up
    LIEDOWN_HIT_STAYDOWN3=5082, // opt  LieDown Hit (stay down)
    BOUNCE_FROM_GROUND_INTO_AIR2=5102, // opt  Bounce from ground into air  Looptime around 3
    BOUNCE_INTO_AIR3=5162, // opt  Bounce into air
    HIT_GROUND_FROM_BOUNCE3=5172, // opt  Hit ground from bounce       Looptime around 3 or 4
    LIEDOWN3=5112, // opt  LieDown
    GET_UP_FROM_LIEDOWN3=5122, // opt  Get up from LieDown
    LIEDEAD_FIRST_ROUNDS3=5152, // opt  LieDead (first rounds)
    LIEDEAD_FINAL_ROUND3=5157, // opt  LieDead (final round)
    UNKNOWN=99999999
};

/*
 * Holds mugen animations, ie: player.air
 */
class MugenAnimation{
    public:
	MugenAnimation();
	MugenAnimation( const MugenAnimation &copy );
	~MugenAnimation();
	
	// Get next Frame
	const MugenFrame *getNext();
	
	// Reset
	inline void reset(){ position = 0; }
	
	// Add a frame
	void addFrame( MugenFrame * );
	
	// Get name of type of animation
	static const std::string getName(const MugenAnimationType t);
	
	// Update logic | go through frames
	void logic();
	
	// Render current frame
	void render( int xaxis, int yaxis, Bitmap &work );
	
	// Go forward a frame 
	void forwardFrame();
	// Go back a frame
	void backFrame();
	
	inline unsigned int getCurrentPosition() { return position; }
	inline MugenFrame *getCurrentFrame(){ return frames[position]; }
	
	// Set type number
	inline void setType(const MugenAnimationType t){ type = t; }
	
	// Get type number
	inline const MugenAnimationType getType(){ return type; }
	
	// Show offense | defense Clsn framework
	inline void toggleDefense(){ showDefense = !showDefense; }
	inline void toggleOffense(){ showOffense = !showOffense; }
	

        inline const std::vector<MugenFrame*> & getFrames() const {
            return frames;
        }
	
    private:
	
	std::vector< MugenFrame * > frames;
	
	unsigned int loopPosition;
	unsigned int position;
	
	MugenAnimationType type;
	
	bool showDefense;
	bool showOffense;
	
	int ticks;
};

#endif
