#ifndef _animation_h
#define _animation_h

#include <string>
#include <vector>
#include <map>

#include "util/load_exception.h"
#include "game/input.h"
#include "attack.h"

/* is that crazy hat for chicken? */

class AnimationEvent;
class Bitmap;
class ECollide;
class Token;
class Character;
class Sound;
class Projectile;
class AnimationTrail;

struct KeyPress{
    std::vector<PaintownInput> combo;

	/*
	KeyPress & operator= ( const KeyPress & rhs ){
		this->combo = rhs.combo;
		return *this;
	}
	*/
};

/* stores a bitmap and a collision object to go with it */
struct Frame{
	Bitmap * pic;
	ECollide * collide;

	Frame( Bitmap * pic, ECollide * e);
	Frame( const Frame & f );
	~Frame();

	bool mine;
	
};

/* stores a sequence of bitmaps along with their collision
 * detection object. 
 */
class Animation{
public:
	
	Animation( Token * tok, Character * const owner ) throw( LoadException );
	Animation( const Animation & animation, Character * const owner ) throw( LoadException );

	virtual ~Animation();

	/* Act:
	 * Progress through one logic cycle
	 */
	bool Act();

	/* getName:
	 * The name of the animation
	 */
	inline const std::string & getName() const{
		return name;
	}

	/* returns true if the animation has finished */
	const bool empty();

	/* reset:
	 * Make the bitmap pointer point to the first bitmap in the sequence
	 */
	void reset();

	/* getAttackCoords:
	 * Get the middle of the current attacking box
	 */
	void getAttackCoords( int & x, int & y );

	// inline const map<std::string,string> & getSequences() const{
	inline const std::vector<std::string> & getSequences() const {
		return sequences;
	}

	/* returns true if previous sequence is none or seq */
	bool properSequence( const std::string & seq );

	/* this animation touched something */
	void contacted();

	/* change the colors via the color mapping 
	 * This method makes this animations bitmaps original, if they 
	 * were copied
	 */
	void reMap( std::map< int, int > & colors );

	/* returns true if a previous sequence is seq */
	bool hasSequence( const std::string & seq );

	/* getWidth:
	 * Gets the width of the current frame
	 */
	const int getWidth() const;

	/* getHeight:
	 * Gets the height of the current frame
	 */
	const int getHeight() const;

	/* setX, setY, setZ:
	 * Sets the owner's( some object ) X,Y or Z coordinate
	 */
	void setX( const int x );
	void setY( const int x );
	void setZ( const int x );

	Bitmap * getFrame( int x );
	const Bitmap * getCurrentFrame() const;

	/* makes the owner jump with velocity x,y,z */
	void jump( double vx, double vy, double vz );

	/* moveX, moveY, moveZ:
	 * Moves the owner by X,Y,Z 
	 */
	void moveX( const int x );
	void moveY( const int y );
	void moveZ( const int z );

	inline void setShadowX( const int x ){
		shadowX = x;
	}
	inline void setShadowY( const int y ){
		shadowY = y;
	}

	inline const int getShadowX() const {
		return shadowX;
	}
	
	inline const int getShadowY() const {
		return shadowY;
	}

	/* offsets the frame absolutely within the scene. Only x and y matter
	 */
	inline void setOffsetX( const int x ){
		offset_x = x;
	}

	inline void setOffsetY( const int y ){
		offset_y = y;
	}

	inline const int getOffsetX(){
		return offset_x;
	}

	inline const int getOffsetY(){
		return offset_y;
	}

	/* tell the owning object to create a projectile */
	void createProjectile( int x, int y, Projectile * p );

	void addDecommision( const std::string & s );
	void addCommision( const std::string & s );

	const std::string getCurrentFramePath() const;

	inline const std::vector<std::string> & getDecommisions() const {
		return disable_animations;
	}

	inline const std::vector<std::string> & getCommisions() const {
		return enable_animations;
	}

	/* isAttack:
	 * True if this animation is an attacking sequence
	 */
	inline const bool isAttack() const{
		return is_attack;
	}

	inline const bool isCommisioned() const {
		return commision;
	}

	inline void setCommision( const bool x ) {
		commision = x;
	}

	inline const int getRange() const {
		return range;
	}

	inline void setRange( const int x ) {
		range = x;
	}

	/* getStatus:
	 * Returns the status
	 */
	inline const int getStatus() const{
		return status;
	}

	inline const std::string & getPreviousSequence() const{
		return prev_sequence;
	}

	inline const std::string & getNextSequence() const{	
		return next_sequence;
	}

	inline const std::vector<KeyPress> & getKeys() const{
		return keys;
	}

	inline void setAttack( const Attack & a ){
		attack = a;
	}

	inline const double getMinZDistance() const {
		return minZDistance;
	}

	inline void setMinZDistance( const double x ){
		minZDistance = x;	
	}

	void playSound( const std::string & path );

	void setFacing( const int direction );

	ECollide * getCollide( int facing );
	ECollide * getNormalCollide();
	void Draw( int x, int y, Bitmap * work );
	void DrawFlipped( int x, int y, Bitmap * work );
	void DrawLit( int x, int y, Bitmap * work );
	void DrawLitFlipped( int x, int y, Bitmap * work );
	void setDelay( int _delay );
	void setStatus( const int status );
	void setFrame( const std::string & path );
	void setFrame( const int fr );
	void setBBox( int x1, int y1, int x2, int y2 );
	// void setAttack( int x1, int y1, int x2, int y2 );
	void Delay();
	void nextTicket();

        virtual AnimationTrail * makeTrail(const int x, const int y, const int facing) const;

	int getDamage() const;
	void setDamage( const int d );

protected:

	void reMap( Bitmap * work, std::map< int, int > & colors );

	// int convertKeyPress( const string & key_name ) throw( LoadException );
	PaintownInput convertKeyPress( const std::string & key_name );

	// void parseObject( string str ) throw( exception );
	void upperCase( std::string & who );

	void doDraw( int x, int y, const Bitmap & frame, Bitmap * work );
	void doDrawFlipped( int x, int y, const Bitmap & frame, Bitmap * work );

protected:
        std::string name;
	bool loop;

	Character * const parent;

	Bitmap * current_frame;
	ECollide * current_collide;
	ECollide * attack_collide;
	int delay;
	int delay_counter;

	double minZDistance;

	int bbox_x1, bbox_y1, bbox_x2, bbox_y2;
	// int attack_x1, attack_y1, attack_x2, attack_y2;

	int offset_x, offset_y;
	int shadowX, shadowY;

	/* I dont think we need range_x/range_y */
	int range_x, range_y;

        std::vector< AnimationEvent * > events;
        std::vector< AnimationEvent * >::iterator current_event;
        std::map< std::string, Frame * > frames;
        std::vector< KeyPress > keys;

        std::string next_sequence, prev_sequence;

	// map< std::string, std::string > sequences;
        std::vector< std::string > sequences;

	/* range of the attack in X direction */
	int range;

	bool own_bitmaps;
	bool own_events;
	bool is_attack;
	int status;
	int damage;

	/* can we use this animation? */
	bool commision;

        std::map< std::string, Sound * > sounds;
	Sound * contact;

        std::vector< std::string > disable_animations;
        std::vector< std::string > enable_animations;

	Attack attack;

};


#endif
