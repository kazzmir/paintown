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
#include "../game/world.h"
#include "util/file-system.h"
#include "object.h"
#include "object_messages.h"
#include "player.h"
#include "util/input/input.h"
#include "util/input/input-manager.h"
#include "util/input/input-source.h"
#include "draw-effect.h"
#include <math.h>

/* how many ticks to wait before keys are removed from the key cache.
 * this can probably be user defined in the future.
 */
#ifdef ANDROID
/* be a little more forgiving on touch screen devices */
static const int GLOBAL_KEY_DELAY = 40;
#else
static const int GLOBAL_KEY_DELAY = 20;
#endif
static const unsigned int KEY_CACHE_SIZE = 100;

using namespace std;

namespace Paintown{

#define DEFAULT_LIVES 4

Player::Player(const char * filename):
PlayerCommon(filename),
acts(0),
name_id(-1),
attack_bonus(0),
invincible(false),
ignore_lives(false),
source(new InputSource()){
	lives = DEFAULT_LIVES;
	
	/*
	if ( getMovement( "grab" ) == NULL ){
		throw LoadException("No 'grab' movement");
	}
	*/

	show_life = getHealth();

	int x, y;
	NamePlacer::getPlacement(x, y, name_id);

        commonInitialize();
}
	
Player::Player(const Filesystem::AbsolutePath & filename, Util::ReferenceCount<InputSource> source):
PlayerCommon(filename),
acts(0),
name_id(-1),
attack_bonus(0),
invincible( false ),
ignore_lives(false),
source(source){

	lives = DEFAULT_LIVES;
	
	// if ( movements[ "grab" ] == NULL ){
	/*
	if ( getMovement( "grab" ) == NULL ){
		throw LoadException("No 'grab' movement");
	}
	*/

	show_life = getHealth();

	int x, y;
	NamePlacer::getPlacement(x, y, name_id);
        commonInitialize();
}
	
Player::Player(const Character & chr):
PlayerCommon(chr),
acts(0),
name_id(-1),
attack_bonus(0),
invincible( false ),
ignore_lives(false),
source(new InputSource()){
    show_life = getHealth();
    lives = DEFAULT_LIVES;
    commonInitialize();
}

Player::Player(const Player & pl):
PlayerCommon(pl),
acts(0),
name_id(-1),
attack_bonus(pl.attack_bonus),
invincible(pl.invincible),
ignore_lives(false),
source(pl.source){
    show_life = getHealth();
    ignore_lives = pl.ignore_lives;
    commonInitialize();
}

void Player::commonInitialize(){
    Graphics::blend_palette(attack_gradient, num_attack_gradient / 2, Graphics::makeColor(255,255,255), Graphics::makeColor(255,255,0));
    Graphics::blend_palette(attack_gradient + num_attack_gradient / 2, num_attack_gradient / 2, Graphics::makeColor(255,255,0), Graphics::makeColor(255,0,0));
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
        Global::stream_type & out = Global::debug(level);
        deque<Input::PaintownInput>::iterator cur = key_cache.begin();
        out << "[player] Key cache" << endl;
        for ( cur = key_cache.begin(); cur != key_cache.end(); cur++ ){
            Input::PaintownInput key = *cur;
            out << "[player]  " << keyToName(key) << endl;
        }
        out << "[player] end key cache" << endl;
    }
}

void Player::resetInput(){
    keyHold = Held();
}

bool Player::isPlayer() const {
    return true;
}
    
const InputSource & Player::getInput() const {
    if (source != NULL){
        return *source;
    }

    /* we failed.. */
    throw std::exception();
}

vector<Input::PaintownInput> Player::fillKeyCache(){
    /*
    acts += 1;
    if (acts > GLOBAL_KEY_DELAY){
        if (!key_cache.empty()){
            key_cache.pop_front();
        }

        acts = 0;
    }
    */

    InputMap<Input::PaintownInput> input;
    InputSource useSource;
    if (source != NULL){
        useSource = *source;
    }

    int facing = getFacing();
    /* set up keyboard */
    if (useSource.useKeyboard()){
        Configuration & configuration = Configuration::config(useSource.getKeyboard());
        enum Input::PaintownInput all[] = {Input::Forward, Input::Back, Input::Up, Input::Down, Input::Attack1, Input::Attack2, Input::Attack3, Input::Attack4, Input::Attack5, Input::Attack6, Input::Jump, Input::Grab};
        for (unsigned int i = 0; i < sizeof(all) / sizeof(Input::PaintownInput); i++){
            input.set(configuration.getKey(all[i], facing), 0, false, all[i]);
        }
    }

    /* set up joystick */
    if (useSource.useJoystick()){
        Configuration & configuration = Configuration::config(useSource.getJoystick());
        enum Input::PaintownInput all[] = {Input::Forward, Input::Back, Input::Up, Input::Down, Input::Attack1, Input::Attack2, Input::Attack3, Input::Attack4, Input::Attack5, Input::Attack6, Input::Jump, Input::Grab};
        for (unsigned int i = 0; i < sizeof(all) / sizeof(Input::PaintownInput); i++){
            input.set(configuration.getJoystickKey(all[i], facing), 0, false, all[i]);
        }
    }

    acts += 1;

    // keyHold.back = false;

    vector<InputMap<Input::PaintownInput>::InputEvent> events = InputManager::getEvents(input, useSource);
    for (vector<InputMap<Input::PaintownInput>::InputEvent>::iterator it = events.begin(); it != events.end(); it++){
        InputMap<Input::PaintownInput>::InputEvent event = *it;

        /*
        switch (event.out){
            case Input::Forward: keyHold.forward = event.enabled; break;
            case Input::Back: keyHold.back = event.enabled; break;
            case Input::Up: keyHold.up = event.enabled; break;
            case Input::Down: keyHold.down = event.enabled; break;
            default: break;
        }
        */

        if (!event.enabled){
            continue;
        }

        key_cache.push_back(event.out);
        
        acts = 0;
    }

    while (key_cache.size() > KEY_CACHE_SIZE){
        key_cache.pop_front();
    }

    if (acts > GLOBAL_KEY_DELAY){
        key_cache.clear();
    }
        
    InputMap<Input::PaintownInput> inputHold;

    /* normalize such that forward is left */
    int facingHold = FACING_LEFT;
    enum Input::PaintownInput allHold[] = {Input::Forward, Input::Back, Input::Up, Input::Down};
    for (unsigned int i = 0; i < sizeof(allHold) / sizeof(Input::PaintownInput); i++){
        if (useSource.useKeyboard()){
            inputHold.set(Configuration::config(useSource.getKeyboard()).getKey(allHold[i], facingHold), 0, false, allHold[i]);
        }
        if (useSource.useJoystick()){
            inputHold.set(Configuration::config(useSource.getJoystick()).getJoystickKey(allHold[i], facingHold), 0, false, allHold[i]);
        }
    }

    vector<InputMap<Input::PaintownInput>::InputEvent> eventsHold = InputManager::getEvents(inputHold, useSource);
    for (vector<InputMap<Input::PaintownInput>::InputEvent>::iterator it = eventsHold.begin(); it != eventsHold.end(); it++){
        InputMap<Input::PaintownInput>::InputEvent event = *it;

        switch (event.out){
            case Input::Forward: keyHold.left = event.enabled; break;
            case Input::Back: keyHold.right = event.enabled; break;
            case Input::Up: keyHold.up = event.enabled; break;
            case Input::Down: keyHold.down = event.enabled; break;
            default: break;
        }
    }

    vector<Input::PaintownInput> real_input;
    if (keyHold.up){
        real_input.push_back(Input::Up);
    }
    if (keyHold.down){
        real_input.push_back(Input::Down);
    }
    if (keyHold.left){
        if (getFacing() == FACING_LEFT){
            real_input.push_back(Input::Forward);
        } else {
            real_input.push_back(Input::Back);
        }
    }
    if (keyHold.right){
        if (getFacing() == FACING_LEFT){
            real_input.push_back(Input::Back);
        } else {
            real_input.push_back(Input::Forward);
        }
        // keyHold.forward = true;
    }

    return real_input;
}

#if 0
vector<Input::PaintownInput> Player::fillKeyCache(){

    /* get the latest key presses */

    /* pull off a key every once in a while */
    if ( acts++ > GLOBAL_KEY_DELAY ){
        // key_cache.clear();
        
        if (!key_cache.empty()){
            key_cache.pop_front();
        }

        acts = 0;
    }
   
    /* I tried to convert this logic to just use an InputMap but was unable because
     * a) turning around means flipping the meaning of the left and right keys
     *   which is annoying to do with InputMap's
     * b) keys should not be put in the key_cache twice, so it looks like the
     *   InputMap can enable blocking but for walking to work the forward/back keys
     *   need to be repeated as fast as possible. Just allowing forward/back to
     *   be non-blocking doesn't work because forward/back are legal keys in the
     *   key_cache. That is, holding down forward should not put more than one
     *   forward key into the key_cache.
     */
    vector<Input::PaintownInput> real_input = InputManager::getInput(Configuration::config(config), getFacing());
    if (real_input.size() > 0){

        map<Input::PaintownInput, bool> new_last;
        for ( vector<Input::PaintownInput>::iterator it = real_input.begin(); it != real_input.end(); it++ ){
            Input::PaintownInput n = *it;

            Global::debug(3) << "Checking key " << n << endl;

            /* dont repeat keys */
            if (! last_key[n]){
                key_cache.push_back(n);
                acts = 0;
            }
            new_last[n] = true;

            /* as soon as back is hit the player will turn around
             * and then forward will be recognized resulting in
             * the immediate sequence back - forward. this
             * messes up the combos, so explicitly disallow
             * forward from following back and vice-versa
             */
            switch (n){
                case Input::Forward : {
                    new_last[Input::Back] = true;
                    break;
                }
                case Input::Back : {
                    new_last[Input::Forward] = true;
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
#endif
        
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
    /* maybe factor score/attack bonus into a function */
    increaseScore((int)(85 * (1 + attack_bonus)));
    attack_bonus += 1;
    Global::debug(1) << "Attack bonus : " << attack_bonus << endl;
    world->addMessage(scoreMessage());
}
	
void Player::drawLifeBar( int x, int y, Graphics::Bitmap * work ){
	drawLifeBar( x, y, show_life, work );
}

/* present the current input to the user */
void Player::drawButtons(Graphics::Bitmap * work, int x, int y){
    Graphics::Color color = Graphics::makeColor(255, 255, 255);
    const Font & font = Font::getFont(Global::DEFAULT_FONT, 10, 10);
    y -= 10;
    for (deque<Input::PaintownInput>::iterator it = key_cache.begin(); it != key_cache.end(); it++){
        Input::PaintownInput key = *it;
        /* FIXME: replace letters with nicer looking symbols / graphics */
        switch (key){
            case Input::Unknown: font.printf(x, y, color, *work, "?", 0); break;
            case Input::Forward: font.printf(x, y, color, *work, "F", 0); break;
            case Input::Back: font.printf(x, y, color, *work, "B", 0); break;
            case Input::Up: font.printf(x, y, color, *work, "U", 0); break;
            case Input::Down: font.printf(x, y, color, *work, "D", 0); break;
            case Input::Attack1: font.printf(x, y, color, *work, "1", 0); break;
            case Input::Attack2: font.printf(x, y, color, *work, "2", 0); break;
            case Input::Attack3: font.printf(x, y, color, *work, "3", 0); break;
            case Input::Attack4: font.printf(x, y, color, *work, "4", 0); break;
            case Input::Attack5: font.printf(x, y, color, *work, "5", 0); break;
            case Input::Attack6: font.printf(x, y, color, *work, "6", 0); break;
            case Input::Jump: font.printf(x, y, color, *work, "J", 0); break;
            case Input::Grab: font.printf(x, y, color, *work, "G", 0); break;
        }
        x += 10 + 5;
    }
}

void Player::drawFront(Graphics::Bitmap * work, int rel_x){

	int x1, y1;
	NamePlacer::getPlacement(x1, y1, name_id);
	
	if ( icon )
		icon->draw( x1, y1, *work );

	int hasIcon = icon ? icon->getWidth() : 0;

	if ( show_life < 0 ){
		show_life = 0;
	}

        /* TODO: make 'show_buttons' an option somewhere */
        bool show_buttons = false;
        if (show_buttons){
            drawButtons(work, 5, work->getHeight() - 5);
        }

	// Font * player_font = FontFactory::getFont( NAME_FONT );
	// const Font & player_font = Font::getFont( NAME_FONT );
	const Font & player_font = Font::getFont(Global::DEFAULT_FONT, 20, 20 );
	const string & name = getName();
	int nameHeight = player_font.getHeight(name) / 2;
	nameHeight = 20 / 2;
	// cout << "Height: " << player_font.getHeight( name ) << endl;
	// Font * player_font = FontFactory::getFont( "bios" );

	// work->printf( ky + x1, y1, Bitmap::makeColor(255,255,255), player_font, getName() );
	FontRender * render = FontRender::getInstance();
	render->addMessage( player_font, (hasIcon + x1) * 2, y1 * 2, Graphics::makeColor(255,255,255), Graphics::MaskColor(), name );

        ostringstream score_s;
        score_s << getScore();
        int attack_color = (int)(attack_bonus * 10);
        if (attack_color > num_attack_gradient - 1){
            attack_color = num_attack_gradient - 1;
        }

        // int attack_x = (hasIcon + x1) * 2 + player_font.textLength(name.c_str()) + 5;
        int attack_x = (int)((hasIcon + x1) * 2 + 100 * Graphics::Bitmap::getScale() - player_font.textLength(score_s.str().c_str()));

        render->addMessage(player_font, attack_x, y1 * 2, attack_gradient[attack_color], Graphics::MaskColor(), score_s.str().c_str());

	// cout << "Draw name at " << y1 * 2 << endl;
	// player_font.printf( (hasIcon + x1) * 2, y1, Bitmap::makeColor(255,255,255), *work, getName() );
	// drawLifeBar( hasIcon + x1, y1 + player_font.getHeight() / 2, show_life, work );
	drawLifeBar( hasIcon + x1, y1 + nameHeight + 1, work );
	// cout << "Y1: " << y1 << " Height: " << player_font.getHeight() << " new y1: " << (y1 + player_font.getHeight() / 2) << endl;
	// work->printf( hasIcon + x1 + getMaxHealth() + 5, y1 + player_font->getHeight(), Bitmap::makeColor(255,255,255), player_font, "x %d", 3 );
        if (!ignoringLives()){
            int max = getMaxHealth() < 100 ? getMaxHealth() : 100;
            render->addMessage( player_font, (x1 + hasIcon + max + 5) * 2, y1 + nameHeight, Graphics::makeColor(255,255,255), Graphics::MaskColor(), "x %d", getLives() );
        }

	// work->rectangle( x1, y1, x1 + 100, y1 + nameHeight + 1, Graphics::Bitmap::makeColor( 255, 255, 255 ) );
}

/* animation keys are lists of lists of keys where inner lists
 * are a set of keys that all must pressed simultaneously.
 * (f f (a1 a2)) means that the user must press
 * forward, forward and then a1 and a2 at the same time.
 */
bool Player::combo(Util::ReferenceCount<Animation> ani, deque<Input::PaintownInput>::iterator cache_cur_key, deque<Input::PaintownInput>::iterator end ){
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
        for ( vector<Input::PaintownInput>::const_iterator cur_key = kp.combo.begin(); cur_key != kp.combo.end(); cur_key++ ){
            Input::PaintownInput find_key = *cur_key;
            Input::PaintownInput key = *cache_cur_key;
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

bool Player::combo(Util::ReferenceCount<Animation> ani){
    deque<Input::PaintownInput>::iterator cur = key_cache.begin();
    for ( cur = key_cache.begin(); cur != key_cache.end(); cur++ ){
        if ( combo( ani, cur, key_cache.end() ) ){
            return true;
        }
    }
    return false;
}

/*
int Player::getKey(Input::PaintownInput motion, int facing){
    return Configuration::config(config).getKey( motion, facing );
}
*/
        
const char * Player::keyToName(Input::PaintownInput key){
    switch (key){
        case Input::Forward : return "forward";
        case Input::Back : return "back";
        case Input::Up : return "up";
        case Input::Down : return "down";
        case Input::Attack1 : return "attack1";
        case Input::Attack2 : return "attack2";
        case Input::Attack3 : return "attack3";
        case Input::Attack4 : return "attack4";
        case Input::Attack5 : return "attack5";
        case Input::Attack6 : return "attack6";
        case Input::Jump : return "jump";
        case Input::Grab : return "grab";
        case Input::Unknown : return "unknown";
    }
    return "key-to-name-error";
}
    
const Filesystem::RelativePath & Player::getIntroFile(){
    return intro;
}

/*
int Player::getKey(Input::PaintownInput x){
	return this->getKey( x, getFacing() );
}
*/
	
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
        addEffect(new DrawGlowEffect(this, Graphics::makeColor(10,10,250), Graphics::makeColor(190, 190, 255), 75));
    }
}
        
int Player::getInvincibility() const {
    if (isInvincible()){
        return 1;
    }
    return Character::getInvincibility();
}

void Player::takeDamage(World & world, ObjectAttack * obj, int x, double forceX, double forceY){
    if (getLink() != NULL){
        getLink()->unGrab();
        unGrab();
    }

    attack_bonus = 0;

    Character::takeDamage(world, obj, x, forceX, forceY);
    if (getDeath() > 0){
        world.dyingObject(*this);
    }

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
			
static Util::ReferenceCount<Animation> hasGetAnimation( const map<Util::ReferenceCount<Animation>, int > & animations ){
    for ( map<Util::ReferenceCount<Animation>, int >::const_iterator it = animations.begin(); it != animations.end(); it++ ){
        Util::ReferenceCount<Animation> a = (*it).first;
        if ( a->getName() == "get" ){
            return a;
        }
    }

    return Util::ReferenceCount<Animation>(NULL);
}

/* things to do when the player dies */
void Player::deathReset(){
    loseLife();
    if (getLives() > 0){
        setY(200);
        setMoving(true);
        setStatus(Status_Falling);
        setHealth(getMaxHealth());
        if (! isInvincible()){
            setInvincibility(400);
        }
        setTrails(0, 0);
        setDeath(0);
        animation_current = getMovement("idle");
    }
}
        
void Player::interpretMessage(World * world, Network::Message & message){
    int type;
    message >> type;

    /* reset because Object::interpretMessage() is going to read
     * the type as well
     */
    message.reset();
    Character::interpretMessage(world, message );
    switch (type){
        case PlayerMessages::Score : {
            unsigned int s;
            int attack;
            message >> s >> attack;
            setScore(s);
            attack_bonus = (double) attack / 10.0;
            break;
        }
        case PlayerMessages::Confirm : {
            Network::Message response;
            response.id = getId();
            response << PlayerMessages::Confirm;
            world->addMessage(response);
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
	
Network::Message Player::thrownMessage(unsigned int id){
    Network::Message message;

    message.id = 0;
    message << World::THROWN;
    message << getId();
    message << id;

    return message;
}

void Player::bindTo(Player * player){
    /* don't add duplicates */
    for (vector<Player*>::iterator it = binds.begin(); it != binds.end(); it++){
        if (*it == player){
            return;
        }
    }
    /* made it this far so add the player */
    binds.push_back(player);
}

/* get all alive players */
vector<Player*> Player::getBinds() const {
    vector<Player*> alive;
    for (vector<Player*>::const_iterator it = binds.begin(); it != binds.end(); it++){
        Player * bind = *it;
        if (bind->getHealth() > 0){
            alive.push_back(bind);
        }
    }
    return alive;
}

void Player::act(vector<Object *> * others, World * world, vector<Object *> * add){
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

	vector<Input::PaintownInput> input = fillKeyCache();

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
        for (vector<Input::PaintownInput>::iterator it = input.begin(); it != input.end(); it++){
            Input::PaintownInput & key = *it;
            switch (key){
                case Input::Forward : key_forward = true; break;
                case Input::Back : key_backward = true; break;
                case Input::Up : key_up = true; break;
                case Input::Down : key_down = true; break;
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
                    Util::ReferenceCount<Animation> ok = getMovement( *it );
			if ( ok != NULL ){
				ok->setCommision( true );
			}
		}
		
		const vector<string> & disallow = animation_current->getDecommisions();
		for ( vector<string>::const_iterator it = disallow.begin(); it != disallow.end(); it++ ){
			// cout<<"Decommision: "<<*it<<endl;
			// Animation * ok = movements[ *it ];
                    Util::ReferenceCount<Animation> ok = getMovement( *it );
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
            Util::ReferenceCount<Animation> final(NULL);
		// unsigned int num_keys = 0;
		map<Util::ReferenceCount<Animation>, int > possible_animations;

                debugDumpKeyCache(3);

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

		for ( map<string, Util::ReferenceCount<Animation> >::const_iterator it = getMovements().begin(); it != getMovements().end(); it++ ){
			if ( (*it).second == NULL ) continue;
                        Util::ReferenceCount<Animation>  b = (*it).second;
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
					possible_animations[b] = b->getKeys().size();
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
                    Util::ReferenceCount<Animation> get = hasGetAnimation( possible_animations );
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

				for ( map<Util::ReferenceCount<Animation>, int>::iterator mit = possible_animations.begin(); mit != possible_animations.end(); mit++ ){
					int & cur = (*mit).second;
                                        Util::ReferenceCount<Animation> blah = (*mit).first;
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
                        /*
                        if (animation_current->getKeys().size() < key_cache.size()){
                            key_cache.erase(key_cache.begin(), key_cache.begin() + animation_current->getKeys().size());
                        } else {
                            key_cache.clear();
                        }
                        */
			
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
	
        if ( (getStatus() == Status_Ground) &&
             (animation_current == getMovement("walk") ||
              animation_current == getMovement("idle")) ){

            bool moved = false;
            if (key_forward){
                moveX( getSpeed() );
                moved = true;
            } else if (key_backward){
                setFacing(getOppositeFacing());
                // keyHold.forward = true;
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
                increaseScore((int)(20 * (1 + attack_bonus)));
                attack_bonus += 1;
                world->addMessage(scoreMessage());
                world->addMessage(link->movedMessage());
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

PlayerFuture::PlayerFuture(const Filesystem::AbsolutePath & path, bool invincible, int lives, int remap, Util::ReferenceCount<InputSource> source):
super(),
path(path),
invincible(invincible),
lives(lives),
remap(remap),
source(source){
}

PlayerFuture::~PlayerFuture(){
    try{
        delete get();
    } catch (...){
        /* failed to load, so nothing to delete */
    }
}

void PlayerFuture::compute(){
    string look = Storage::instance().cleanse(path).path();
    Global::info("Loading " + look);
    Player * player = new Player(path, source);
    player->setInvincible(invincible);
    player->setMap(remap);
    player->setObjectId(-1);
    player->setLives(lives);
    Global::info("Loaded " + look);
    set(player);
}

}
