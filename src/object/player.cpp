#include <iostream>
#include <sstream>

#include "animation.h"
#include "util/bitmap.h"
#include "character.h" 
#include "util/font.h"
#include "util/funcs.h"
#include "factory/font_render.h"
#include "configuration.h"
#include "globals.h"
#include "nameplacer.h"
#include "util/load_exception.h"
#include "world.h"
#include "util/file-system.h"
#include "object.h"
#include "object_messages.h"
#include "player.h"
#include "game/input.h"
#include "game/input-manager.h"
#include "draw-glow-effect.h"
#include "draw-countdown-effect.h"

// how many ticks to wait before the key cache is cleared.
// this can probably be user defined in the future
static const int GLOBAL_KEY_DELAY = 15;
static const unsigned int KEY_CACHE_SIZE = 100;
static const char * PLAYER_FONT = "/fonts/arial.ttf";

using namespace std;

#define DEFAULT_LIVES 4

Player::Player( const char * filename, int config ) throw( LoadException ): 
PlayerCommon(filename),
acts(0),
name_id(-1),
attack_bonus(0),
invincible( false ),
config(config),
ignore_lives(false){
	lives = DEFAULT_LIVES;
	
	/*
	if ( getMovement( "grab" ) == NULL ){
		throw LoadException("No 'grab' movement");
	}
	*/

	show_life = getHealth();

	int x, y;
	NamePlacer::getPlacement( x, y, name_id );

        initializeAttackGradient();
}
	
Player::Player( const string & filename, int config ) throw( LoadException ):
PlayerCommon(filename),
acts(0),
name_id(-1),
attack_bonus(0),
invincible( false ),
config(config),
ignore_lives(false){

	lives = DEFAULT_LIVES;
	
	// if ( movements[ "grab" ] == NULL ){
	/*
	if ( getMovement( "grab" ) == NULL ){
		throw LoadException("No 'grab' movement");
	}
	*/

	show_life = getHealth();

	int x, y;
	NamePlacer::getPlacement( x, y, name_id );
        initializeAttackGradient();
}
	
Player::Player(const Character & chr) throw( LoadException ):
PlayerCommon(chr),
acts(0),
name_id(-1),
attack_bonus(0),
invincible( false ),
config(0),
ignore_lives(false){
	show_life = getHealth();
	lives = DEFAULT_LIVES;
        initializeAttackGradient();
}

Player::Player(const Player & pl) throw( LoadException ):
PlayerCommon( pl ),
acts( 0 ),
name_id(-1),
attack_bonus(0),
invincible( false ),
ignore_lives(false){
	show_life = getHealth();
        ignore_lives = pl.ignore_lives;
        initializeAttackGradient();
        config = pl.config;
}

void Player::initializeAttackGradient(){
    Util::blend_palette(attack_gradient, num_attack_gradient / 2, Bitmap::makeColor(255,255,255), Bitmap::makeColor(255,255,0));
    Util::blend_palette(attack_gradient + num_attack_gradient / 2, num_attack_gradient / 2, Bitmap::makeColor(255,255,0), Bitmap::makeColor(255,0,0));
}

Player::~Player(){
}

void Player::loseLife( int l ){
    if (!ignoringLives()){
	lives -= l;
    }
}

void Player::gainLife( int l ){
    if (!ignoringLives()){
	lives += l;
    }
}

void Player::debugDumpKeyCache(int level){
    if (key_cache.size() > 0){
        ostream & out = Global::debug(level);
        deque<PaintownInput>::iterator cur = key_cache.begin();
        out << "[player] Key cache" << endl;
        for ( cur = key_cache.begin(); cur != key_cache.end(); cur++ ){
            PaintownInput key = *cur;
            out << "[player]  " << keyToName(key) << endl;
        }
        out << "[player] end key cache" << endl;
    }
}

vector<PaintownInput> Player::fillKeyCache(){

    /* get the latest key presses */

    /* pull off a key every once in a while */
    if ( acts++ > GLOBAL_KEY_DELAY ){
        // key_cache.clear();
        
        if (!key_cache.empty()){
            key_cache.pop_front();
        }

        acts = 0;
    }

    vector<PaintownInput> real_input = InputManager::getInput(Configuration::config(config), getFacing());
    if (real_input.size() > 0){

        map<PaintownInput, bool > new_last;
        for ( vector<PaintownInput>::iterator it = real_input.begin(); it != real_input.end(); it++ ){
            PaintownInput n = *it;

            Global::debug(1) << "Checking key " << n << endl;

            /* dont repeat keys */
            if ( ! last_key[n] ){
                key_cache.push_back(n);
                acts = 0;
            }
            new_last[ n ] = true;

            /* as soon as back is hit the player will turn around
             * and then forward will be recognized resulting in
             * the immediate sequence back - forward. this
             * messes up the combos, so explicitly disallow
             * forward from following back and vice-versa
             */
            switch (n){
                case Forward : {
                    new_last[Back] = true;
                    break;
                }
                case Back : {
                    new_last[Forward] = true;
                }
                default : break;
            }
            // }
        }

        /* stores the keys pressed in the last frame */
        last_key = new_last;
    } else {
        last_key.clear();
    }

    while ( key_cache.size() > KEY_CACHE_SIZE ){
        key_cache.pop_front();
    }

    return real_input;
}
        
Network::Message Player::getCreateMessage(){
    Network::Message message = Character::getCreateMessage();
    int player = World::IS_PLAYER;
    message << player;
    return message;
}

Network::Message Player::scoreMessage(){
    Network::Message m;

    m.id = getId();
    m << PlayerMessages::Score;
    m << getScore();
    m << (int)(attack_bonus * 10);

    return m;
}
        
void Player::attacked( World * world, Object * something, vector< Object * > & objects ){
    Character::attacked(world, something, objects);
    increaseScore((int)(85 * (1 + attack_bonus)));
    attack_bonus += 1;
    Global::debug(1) << "Attack bonus : " << attack_bonus << endl;
    world->addMessage(scoreMessage());
}
	
void Player::drawLifeBar( int x, int y, Bitmap * work ){
	drawLifeBar( x, y, show_life, work );
}

void Player::drawFront( Bitmap * work, int rel_x ){

	int x1, y1;
	NamePlacer::getPlacement( x1, y1, name_id );
	
	if ( icon )
		icon->draw( x1, y1, *work );

	int hasIcon = icon ? icon->getWidth() : 0;
	

	if ( show_life < 0 ){
		show_life = 0;
	}

	// Font * player_font = FontFactory::getFont( NAME_FONT );
	// const Font & player_font = Font::getFont( NAME_FONT );
	const Font & player_font = Font::getFont(Filesystem::find(PLAYER_FONT), 20, 20 );
	const string & name = getName();
	int nameHeight = player_font.getHeight( name ) / 2;
	nameHeight = 20 / 2;
	// cout << "Height: " << player_font.getHeight( name ) << endl;
	// Font * player_font = FontFactory::getFont( "bios" );

	// work->printf( ky + x1, y1, Bitmap::makeColor(255,255,255), player_font, getName() );
	FontRender * render = FontRender::getInstance();
	render->addMessage( player_font, (hasIcon + x1) * 2, y1 * 2, Bitmap::makeColor(255,255,255), -1, name );

        ostringstream score_s;
        score_s << getScore();
        int attack_color = (int)(attack_bonus * 10);
        if (attack_color > num_attack_gradient - 1){
            attack_color = num_attack_gradient - 1;
        }

        // int attack_x = (hasIcon + x1) * 2 + player_font.textLength(name.c_str()) + 5;
        int attack_x = (int)((hasIcon + x1) * 2 + 100 * Bitmap::getScale() - player_font.textLength(score_s.str().c_str()));

        render->addMessage(player_font, attack_x, y1 * 2, attack_gradient[attack_color], -1, score_s.str().c_str());

	// cout << "Draw name at " << y1 * 2 << endl;
	// player_font.printf( (hasIcon + x1) * 2, y1, Bitmap::makeColor(255,255,255), *work, getName() );
	// drawLifeBar( hasIcon + x1, y1 + player_font.getHeight() / 2, show_life, work );
	drawLifeBar( hasIcon + x1, y1 + nameHeight, work );
	// cout << "Y1: " << y1 << " Height: " << player_font.getHeight() << " new y1: " << (y1 + player_font.getHeight() / 2) << endl;
	// work->printf( hasIcon + x1 + getMaxHealth() + 5, y1 + player_font->getHeight(), Bitmap::makeColor(255,255,255), player_font, "x %d", 3 );
        if (!ignoringLives()){
            int max = getMaxHealth() < 100 ? getMaxHealth() : 100;
            render->addMessage( player_font, (x1 + hasIcon + max + 5) * 2, y1 + nameHeight, Bitmap::makeColor(255,255,255), -1, "x %d", getLives() );
        }

	// work->rectangle( x1, y1, x1 + 100, y1 + nameHeight + 1, Bitmap::makeColor( 255, 255, 255 ) );
}

/* animation keys are lists of lists of keys where inner lists
 * are a set of keys that all must pressed simultaneously.
 * (f f (a1 a2)) means that the user must press
 * forward, forward and then a1 and a2 at the same time.
 */
bool Player::combo( Animation * ani, deque<PaintownInput>::iterator cache_cur_key, deque<PaintownInput>::iterator end ){
    // cout << "Testing " << ani->getName() << " facing = " << startFacing << ". current facing = " << getFacing() << endl;
    const vector< KeyPress > & keys = ani->getKeys();
    if ( keys.empty() ){
        return false;
    }

    for ( vector<KeyPress>::const_iterator k = keys.begin(); k != keys.end(); k++ ){
        if ( cache_cur_key == end ){
            return false;
        }

        const KeyPress & kp = *k;
        bool all_pressed = true;
        for ( vector<PaintownInput>::const_iterator cur_key = kp.combo.begin(); cur_key != kp.combo.end(); cur_key++ ){
            PaintownInput find_key = *cur_key;
            PaintownInput key = *cache_cur_key;
            if ( find_key != key ){
                all_pressed = false;
            }
        }
        if ( !all_pressed ){
            return false;
        }

        cache_cur_key++;
    }
    return true;
}

bool Player::combo( Animation * ani ){
    deque<PaintownInput>::iterator cur = key_cache.begin();
    for ( cur = key_cache.begin(); cur != key_cache.end(); cur++ ){
        if ( combo( ani, cur, key_cache.end() ) ){
            return true;
        }
    }
    return false;
}

int Player::getKey(PaintownInput motion, int facing){
    return Configuration::config(config).getKey( motion, facing );
}
        
const char * Player::keyToName(PaintownInput key){
    switch (key){
        case Forward : return "forward";
        case Back : return "back";
        case Up : return "up";
        case Down : return "down";
        case Attack1 : return "attack1";
        case Attack2 : return "attack2";
        case Attack3 : return "attack3";
        case Jump : return "jump";
        case Grab : return "grab";
        case Unknown : return "unknown";
    }
    return "key-to-name-error";
}
	
int Player::getKey(PaintownInput x){
	return this->getKey( x, getFacing() );
}
	
Object * Player::copy(){
	return new Player( *this );
}
	
void Player::hurt( int x ){
    if (! isInvincible()){
        Character::hurt(x);
    }
}
        
void Player::setInvincible(const bool b){
    this->invincible = b;
    if (b){
        addEffect(new DrawGlowEffect(this, Bitmap::makeColor(10,10,250), Bitmap::makeColor(190, 190, 255), 75));
    }
}
        
const int Player::getInvincibility() const {
    if (isInvincible()){
        return 1;
    }
    return Character::getInvincibility();
}

void Player::takeDamage( World * world, ObjectAttack * obj, int x ){
	if ( getLink() != NULL ){
		getLink()->unGrab();
		unGrab();
	}

        attack_bonus = 0;

	Character::takeDamage( world, obj, x );

	// cout<<"Player taking "<<x<<" damage"<<endl;
}

/* check to see if the player is close enough to an enemy */
bool Player::canGrab( Object * enemy ){
	// return false;
        if (!enemy->isGrabbable(this)){
            return false;
        }

	if ( !enemy->isCollidable( this ) ){
            return false;
	}

	if ( enemy->isGrabbed() ){
		return false;
	}

	if ( getMovement( "grab" ) == NULL ){
		return false;
	}

	/*
	bool d = ZDistance( enemy ) < MIN_RELATIVE_DISTANCE;
	bool c = realCollision( enemy );
	cout<<enemy<<": distance = "<<d<<" collision = "<<c<<endl;
	*/
	// if ( ZDistance( enemy ) < MIN_RELATIVE_DISTANCE && enemy->collision( this ) ){

	// animation_current = movements[ "grab" ];
	// animation_current = getMovement( "grab" );

	// if ( ZDistance( enemy ) < MIN_RELATIVE_DISTANCE && realCollision( enemy ) ){
	if ( ZDistance( enemy ) < MIN_RELATIVE_DISTANCE && XDistance( enemy ) < 30 ){
		return true;
	}
	return false;
}

void Player::grabEnemy( Object * enemy ){
	// enemy->setStatus( Status_Grabbed );
	enemy->grabbed( this );
	setLink( enemy );
}
			
static Animation * hasGetAnimation( const map< Animation *, int > & animations ){
	for ( map< Animation *, int >::const_iterator it = animations.begin(); it != animations.end(); it++ ){
		Animation * a = (*it).first;
		if ( a->getName() == "get" ){
			return a;
		}
	}

	return NULL;
}

/* things to do when the player dies */
void Player::deathReset(){
	setY( 200 );
	setMoving( true );
	setStatus( Status_Falling );
	setHealth( getMaxHealth() );
        if (! isInvincible()){
            setInvincibility( 400 );
        }
	setDeath( 0 );
	animation_current = getMovement( "idle" );
	loseLife();
}
        
void Player::interpretMessage(Network::Message & message){
    int type;
    message >> type;

    /* reset because Object::interpretMessage() is going to read
     * the type as well
     */
    message.reset();
    Character::interpretMessage( message );
    switch (type){
        case PlayerMessages::Score : {
            unsigned int s;
            int attack;
            message >> s >> attack;
            setScore(s);
            attack_bonus = (double) attack / 10.0;
            break;
        }
        case CharacterMessages::Health : {
            int health;
            message.reset();
            message >> type >> health;
            if (health < getHealth()){
                attack_bonus = 0;
            }
            break;
        }
    }
}
	
Network::Message Player::thrownMessage( unsigned int id ){
	Network::Message message;

	message.id = 0;
	message << World::THROWN;
	message << getId();
	message << id;

	return message;
}

void Player::act( vector< Object * > * others, World * world, vector< Object * > * add ){

    /* this is just for score */
    if (attack_bonus > 0){
        attack_bonus -= 0.02;
    } else {
        attack_bonus = 0;
    }

	if ( show_life < getHealth() ){
		show_life++;
	} else if ( show_life > getHealth() ){
		show_life--;
	}
	
	/* cheat */
	/*
	if ( getInvincibility() < 500 ){
		setInvincibility( 1000 );
	}
	*/

	/* isInvincible is a cheat so always set invinciblility to a positive value */
        /*
	if ( isInvincible() && getInvincibility() < 1 ){
		setInvincibility( 100 );
	}
        */

	/* Character handles jumping and possibly other things */
	Character::act( others, world, add );

	vector<PaintownInput> input = fillKeyCache();

        /*
        JoystickInput joyinput;
        if (joystick != NULL){
	    joyinput = joystick->readAll();
	}
        */

        /*
        bool key_forward = keyboard[getKey(Forward)] || (getFacing() == FACING_RIGHT && joyinput.right) || (getFacing() == FACING_LEFT && joyinput.left);
        bool key_backward = keyboard[getKey(Back)] || (getFacing() == FACING_RIGHT && joyinput.left) || (getFacing() == FACING_LEFT && joyinput.right);
        bool key_up = keyboard[getKey(Up)] || joyinput.up;
        bool key_down = keyboard[getKey(Down)] || joyinput.down;
        */
        bool key_forward = false;
        bool key_backward = false;
        bool key_up = false;
        bool key_down = false;
        for (vector<PaintownInput>::iterator it = input.begin(); it != input.end(); it++){
            PaintownInput & key = *it;
            switch (key){
                case Forward : key_forward = true; break;
                case Back : key_backward = true; break;
                case Up : key_up = true; break;
                case Down : key_down = true; break;
                default : break;
            }
        }

	/* special cases... */
	if ( getStatus() == Status_Hurt || getStatus() == Status_Fell || getStatus() == Status_Rise || getStatus() == Status_Get || getStatus() == Status_Falling )
		return;

	/*
	if ( getStatus() == Status_Grab ){
		return;
	}
	*/
	
	/* Now the real meat */

	/*
	if ( !key_cache.empty() ){
		for ( deque<int>::iterator dit = key_cache.begin(); dit != key_cache.end(); dit++ ){
			cout<< *dit <<" ";
		}
		cout<<endl;
	}
	*/
	
	bool reset = animation_current->Act();

	/* cant interrupt an animation unless its walking or idling */
	// if ( animation_current != movements[ "walk" ] && animation_current != movements[ "idle" ]  && animation_current != movements[ "jump" ] ){
	if ( animation_current != getMovement( "walk" ) && animation_current != getMovement( "idle" ) && animation_current != getMovement( "jump" ) ){
	// if ( animation_current != movements[ "walk" ] && animation_current != movements[ "idle" ] ){
		// if ( !animation_current->Act() ) return;
		// animation_current = movements[ "idle" ];
		if ( !reset ) return;
	} else {
		/*
		if ( animation_current->Act() )
			animation_current->reset();
		*/
	}
	string current_name = animation_current->getName();

	// bool status = animation_current->getStatus();

	if ( reset ){
		if ( getStatus() != Status_Grab ){
			animation_current->reset();
		}

		const vector<string> & allow = animation_current->getCommisions();
		for ( vector<string>::const_iterator it = allow.begin(); it != allow.end(); it++ ){
			// cout<<"Commision: "<<*it<<endl;
			// Animation * ok = movements[ *it ];
			Animation * ok = getMovement( *it );
			if ( ok != NULL ){
				ok->setCommision( true );
			}
		}
		
		const vector<string> & disallow = animation_current->getDecommisions();
		for ( vector<string>::const_iterator it = disallow.begin(); it != disallow.end(); it++ ){
			// cout<<"Decommision: "<<*it<<endl;
			// Animation * ok = movements[ *it ];
			Animation * ok = getMovement( *it );
			if ( ok != NULL ){
				ok->setCommision( false );
			}
		}
		
		/* leave this line here, ill figure out why its important later
		 * if you need the animation_current->getName(), use current_name
		 */
		// cout << "Set current animation to null" << endl;
		animation_current = NULL;
	}
	// animation_current = NULL;

	if ( true ){
		Animation * final = NULL;
		// unsigned int num_keys = 0;
		map<Animation *, int > possible_animations;

                debugDumpKeyCache(2);

		/*
		vector< Animation * > xv;
		for ( map<string,Animation*>::const_iterator it = movements.begin(); it != movements.end(); it++ ){
			xv.push_back( (*it).second );
		}
		for ( vector< Animation * >::iterator it = xv.begin(); it != xv.end(); ){
			Animation * b = *it;
			if ( combo( b ) ){
				it++;
				if ( b->getStatus() == getStatus() && (b->getPreviousSequence() == "none" || current_name == b->getPreviousSequence() ) ){
					possible_animations[ b ] = b->getKeys().size();
				}
			} else it = xv.erase( it );
		}
		*/

		for ( map<string,Animation *>::const_iterator it = getMovements().begin(); it != getMovements().end(); it++ ){
			if ( (*it).second == NULL ) continue;
			Animation * b = (*it).second;
			if ( b->isCommisioned() && combo(b) && b->getStatus() == getStatus() ){
				// cout<<b->getName() << " has sequences"<<endl;
				/*
				for ( vector<string>::const_iterator it = b->getSequences().begin(); it != b->getSequences().end(); it++ ){
					cout<< *it << endl;
				}
				*/
				// cout<<"Testing proper sequence with "<<b->getName()<<endl;
				if ( b->properSequence( current_name ) ){
				// if ( b->getPreviousSequence() == "none" || current_name == b->getPreviousSequence() ){
					// cout<<"Possible sequence " <<b->getName()<<endl;
					possible_animations[ b ] = b->getKeys().size();
				}
			} 
		}

		/*
		for ( map<string,Animation*>::const_iterator it = movements.begin(); it != movements.end(); it++ ){

			// word up
			debug
			Animation *const & ani = (*it).second;
		
			if ( animation_current != movements[ "walk" ] && animation_current != movements[ "idle" ] ){
				debug
				if ( current_name != ani->getPreviousSequence() )
					continue;
			}

			const vector< KeyPress > & keys = ani->getKeys();
			for ( vector<KeyPress>::const_iterator k = keys.begin(); k != keys.end(); k++ ){
				const KeyPress & kp = *k;
				bool all_pressed = false;
				debug
				for ( vector<int>::const_iterator cur_key = kp.combo.begin(); cur_key != kp.combo.end(); cur_key++ ){
		
					int find_key = getKey(*cur_key);

					/ *
					if ( key[ *cur_key ] )
						all_pressed = true;
					else {
						all_pressed = false;
						break;
					}
					* /
					bool found_it = false;
					for ( deque<int>::iterator dit = key_cache.begin(); dit != key_cache.end(); dit++ ){
						if ( find_key == *dit ) 
							found_it = true;
					}

					if ( found_it ){
						all_pressed = true;
					} else {
						all_pressed = false;
						break;
					}
				}

				debug
				// if ( all_pressed && num_keys < keys.size() ){
				if ( all_pressed ){
					debug
					if ( getStatus() == ani->getStatus() ){
						debug
						// cout<<"Valid movement "<<(*it).first<<" Current = "<<animation_current->getName()<<". Previous = "<<ani->getPreviousSequence()<<endl;
						if ( ani->getPreviousSequence() == "none" || current_name == ani->getPreviousSequence() ){
							debug
							final = ani;
							num_keys = keys.size();
							// cout<< "map: " << possible_animations[ ani ] << " " << kp.combo.size() << endl;
							if ( possible_animations[ ani ] < kp.combo.size() )
								possible_animations[ ani ] = kp.combo.size();
							// cout<<"Chose "<<(*it).first<<endl;
						}
					}
					// cout<<endl;
				}
			}
			debug
		}
		*/

		int max = -1;
		if ( ! possible_animations.empty() ){

                        /* if its a get animation then it takes precedence
                         * over the rest
                         */
			Animation * get = hasGetAnimation( possible_animations );
			if ( get != NULL ){
				for ( vector< Object * >::iterator it = others->begin(); it != others->end(); it++ ){
					Object * o = *it;
					if ( o->isGettable() && fabs((double)(o->getRX() - getRX())) < 25 && ZDistance( o ) <= get->getMinZDistance() ){
						final = get;
						setStatus( Status_Get );
						world->addMessage( movedMessage() );
					}
				}
				possible_animations.erase( get );
			}

                        /* if its not a get animation then choose the animation
                         * with the longest sequence of keys required to invoke
                         * the animation
                         */
			if ( getStatus() != Status_Get ){

				for ( map<Animation *, int>::iterator mit = possible_animations.begin(); mit != possible_animations.end(); mit++ ){
					int & cur = (*mit).second;
					Animation * blah = (*mit).first;
					Global::debug( 3 ) << blah->getName() << "? ";
					// if ( cur > max || blah->getPreviousSequence() == current_name ){
					// cout<<"Testing "<<blah->getName()<<endl;
					if ( blah->hasSequence( current_name ) ){
						// cout<<blah->getName() << " has "<< current_name << endl;
					}
					if ( cur > max || blah->hasSequence( current_name ) ){
						// cout<<"Current choice = "<<blah->getName() <<endl;
						final = blah;
						max = cur;
					}
				}
				Global::debug( 3 ) << endl;
			}
		}
		
                /* special cases when no animation has been chosen */
		if ( final == NULL /* && animation_current == NULL ){ */ && getStatus() != Status_Grab ){
			bool moving = key_forward || key_up || key_down;
			if ( getMovement( "jump" ) == NULL || animation_current != getMovement( "jump" ) ){
				if ( !moving ){
					if ( animation_current != getMovement( "idle" ) ){
						animation_current = getMovement( "idle" );
						world->addMessage( animationMessage() );
					}
				} else	{
					vector< Object * > my_enemies;
					filterEnemies( my_enemies, others );
					bool cy = false;
					for ( vector< Object * >:: iterator it = my_enemies.begin(); it != my_enemies.end(); it++ ){
						Object * guy = *it;

						if ( canGrab( guy ) ){
							cy = true;
							Global::debug( 2 ) << getId() << " grabbing " << guy->getId() << endl;
							grabEnemy(guy);
							world->addMessage( grabMessage( getId(), guy->getId() ) );
							setZ( guy->getZ()+1 );
							animation_current = getMovement( "grab" );
							animation_current->reset();
							world->addMessage( animationMessage() );
							setStatus( Status_Grab );
						}
						if ( cy )
							break;
					}
					if ( !cy ){
						animation_current = getMovement( "walk" );
						world->addMessage( animationMessage() );
					}
				}
			}
		} else if ( final != NULL && animation_current != final ){
			if ( final->getName() == "special" ){
				if ( getHealth() <= 10 ){
					animation_current = getMovement( "idle" );
					world->addMessage( animationMessage() );
					return;
				} else {
					hurt( 10 );
					world->addMessage( healthMessage() );
				}
			}
			nextTicket();
			animation_current = final;
			animation_current->reset();
			world->addMessage( animationMessage() );

                        /* after executing a move clear the cache so that
                         * its not repeated forever
                         */
                        key_cache.clear();
			
			if ( animation_current == getMovement("jump") ) {
				double x = 0;
				double y = 0;
				if (key_forward){
					x = getSpeed() * 1.2;
				}
				if (key_down){
					y = getSpeed() * 1.2;	
				}
				if (key_up){
					y = -getSpeed() * 1.2;	
				}

				doJump( x, y );
				world->addMessage( jumpMessage( x, y ) );
			}
		}
	}

	if ( getStatus() == Status_Grab && animation_current == NULL ){
		animation_current = getMovement( "grab" );
		world->addMessage( animationMessage() );
	}
	
	if ( (getStatus() == Status_Ground) && (animation_current == getMovement( "walk" ) || animation_current == getMovement( "idle" )) ){

		bool moved = false;
		if (key_forward){
			moveX( getSpeed() );
			moved = true;
		} else if (key_backward){
			setFacing( getOppositeFacing() );
			moved = true;
		}

		if (key_up){
			moveZ( -getSpeed() );
			moved = true;
		} else if (key_down){
			moveZ( getSpeed() );
			moved = true;
		}

		if ( moved ){
			world->addMessage( movedMessage() );
		}
	} else {
	
		if ( getMovement( "throw" ) != NULL && animation_current == getMovement( "throw" ) ){
			if ( getLink() == NULL ){
				Global::debug( 0 ) << "*BUG* Link is null. This can't happen." <<endl;
				return;
			}
			Object * link = getLink();
			link->setFacing( getOppositeFacing() );
			link->thrown();
			world->addMessage( link->movedMessage() );
			/* TODO: the fall distance could be defined in the player
			 * file instead of hard-coded.
			 */
			world->addMessage( ((Character *)link)->fallMessage( 3.2, 5.0 ) );
			world->addMessage( thrownMessage( link->getId() ) );
			link->fall( 3.2, 5.0 );
			setStatus( Status_Ground );
			world->addMessage( movedMessage() );
		}
	}
}
