#include "util/bitmap.h"
#include "util/trans-bitmap.h"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <math.h>
#include <vector>
#include <algorithm>

#include "animation.h"
#include "animation_trail.h"
#include "character.h"
#include "globals.h"
#include "util/debug.h"
#include "object.h"
#include "object_messages.h"
#include "object_attack.h"
#include "stimulation.h"
#include "draw-effect.h"
#include "gib.h"

#include "../factory/shadow.h"
#include "util/funcs.h"
#include "util/lit_bitmap.h"
#include "util/ebox.h"
#include "util/load_exception.h"
#include "util/sound.h"
#include "util/timedifference.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "util/file-system.h"
#include "util/tokenreader.h"
#include "../script/script.h"
#include "../game/world.h"
#include "../game/mod.h"

using namespace std;

namespace Paintown{
    
Remap::Remap(const Filesystem::RelativePath & from, const Filesystem::RelativePath & to):
remapFrom(from),
remapTo(to){
    colors = computeRemapColors(from, to);
}

Remap::Remap(const Remap & copy):
remapFrom(copy.remapFrom),
remapTo(copy.remapTo),
colors(copy.colors){
}

Remap::~Remap(){
}
    
Graphics::Color Remap::filter(Graphics::Color pixel) const {
    map<Graphics::Color, Graphics::Color>::const_iterator replace = colors.find(pixel);
    if (replace != colors.end()){
        return replace->second;
    }
    return pixel;
}

map<Graphics::Color, Graphics::Color> Remap::computeRemapColors(const Filesystem::RelativePath & from, const Filesystem::RelativePath & to){
    Graphics::RestoreState state;
    Graphics::Bitmap b_from(Paintown::Mod::getCurrentMod()->makeBitmap(from));
    Graphics::Bitmap b_to(Paintown::Mod::getCurrentMod()->makeBitmap(to));

    map<Graphics::Color, Graphics::Color> remap_colors;

    for (int x1 = 0; x1 < b_from.getWidth(); x1++){
        for (int y1 = 0; y1 < b_from.getHeight(); y1++){
            Graphics::Color from_col = b_from.getPixel(x1, y1);
            Graphics::Color to_col = b_to.getPixel(x1, y1);
            if (to_col != Graphics::MaskColor() &&
                from_col != to_col){
                remap_colors[from_col] = to_col;
            }
        }
    }

    return remap_colors;

    /*
    const map<string, Animation *> & old_map = original->getAnimations();
    for ( map<string,Animation*>::const_iterator it = old_map.begin(); it != old_map.end(); it++ ){
        const string & name = (*it).first;
        const Animation * old = (*it).second;

        Animation * newAnimation = old->copy(from);
        newAnimation->reMap(remap_colors);
        setAnimation(name, newAnimation);
    }
    */
}

Character::Character( int alliance ):
ObjectAttack( alliance ),
shadow( 0 ),
icon( NULL ),
animation_current(NULL),
own_stuff( false ),
x_velocity( 0 ),
y_velocity( 0 ),
z_velocity( 0 ),
status( Status_Ground ),
death( 0 ),
thrown_status( false ),
speed( 1 ),
linked( NULL ),
moving( 0 ),
current_map( 0 ),
die_sound( NULL ),
landed_sound( NULL ),
squish_sound( NULL ),
invincibility(0),
toughness( 10 ),
explode( false ),
lives( 0 ),
draw_shadow( true ),
trail_generator(0),
trail_counter(0),
trail_life(0),
spriteScale(1){
    mapper[current_map] = NULL;
}

Character::Character(const Filesystem::AbsolutePath & filename, int alliance):
ObjectAttack( 0, 0, alliance ),
type( 0 ),
shadow( 0 ),
icon( NULL ),
animation_current(NULL),
own_stuff( false ),
x_velocity( 0 ),
y_velocity( 0 ),
z_velocity( 0 ),
status( Status_Ground ),
death( 0 ),
thrown_status( false ),
speed( 1 ),
linked( NULL ),
moving( 0 ),
current_map( 0 ),
die_sound( NULL ),
landed_sound( NULL ),
squish_sound( NULL ),
invincibility( 0 ),
toughness( 10 ),
explode( false ),
lives( 0 ),
draw_shadow( true ),
trail_generator(0),
trail_counter(0),
trail_life(0),
spriteScale(1){
    name = "";
    loadSelf(filename);
}

Character::Character( const char * filename, int alliance ):
ObjectAttack( 0, 0, alliance ),
type( 0 ),
shadow( 0 ),
icon( NULL ),
animation_current(NULL),
own_stuff( false ),
x_velocity( 0 ),
y_velocity( 0 ),
z_velocity( 0 ),
status( Status_Ground ),
death( 0 ),
thrown_status( false ),
speed( 1 ),
linked( NULL ),
moving( 0 ),
current_map( 0 ),
die_sound( NULL ),
landed_sound( NULL ),
squish_sound( NULL ),
invincibility( 0 ),
toughness( 10 ),
explode( false ),
lives( 0 ),
draw_shadow( true ),
trail_generator(0),
trail_counter(0),
trail_life(0){
    name = "";

    loadSelf(Filesystem::AbsolutePath(string(filename)));
}

Character::Character( const Character & chr ):
ObjectAttack(chr),
animation_current(NULL),
own_stuff( false ),
death( 0 ),
thrown_status( false ),
moving( 0 ),
current_map( chr.current_map ),
die_sound( NULL ),
landed_sound( NULL ),
squish_sound( NULL ),
explode( false ),
draw_shadow( true ),
trail_generator(chr.trail_generator),
trail_counter(chr.trail_counter),
trail_life(chr.trail_life),
spriteScale(chr.spriteScale){

    /* these are set in object.cpp */
    // setHealth( chr.getHealth() );
    // setMaxHealth( chr.getMaxHealth() );
    // setFacing( chr.getFacing() );
    // setX( chr.getX() );
    // setY( chr.getY() );
    // setZ( chr.getZ() );

    name = chr.getName();
    lives = chr.lives;
    type = chr.type;
    shadow = chr.shadow;
    icon = chr.icon;
    setSpeed( chr.getSpeed() );

    path = chr.getPath();

    linked = NULL;
    setYVelocity( 0 );
    setXVelocity( 0 );
    setZVelocity( 0 );

    body_parts = chr.body_parts;

    setJumpingYVelocity( chr.getMaxJumpingVelocity() );
    setShadow( chr.getShadow() );
    status = chr.getStatus();
    invincibility = chr.invincibility;
    toughness = chr.getToughness();

    for (vector<DrawEffect*>::const_iterator it = chr.effects.begin(); it != chr.effects.end(); it++){
        DrawEffect * effect = *it;
        addEffect(effect->copy(this));
    }

    if ( chr.die_sound != NULL ) 
        die_sound = new Sound( *(chr.die_sound) );

    if ( chr.landed_sound != NULL ){
        landed_sound = new Sound( *(chr.landed_sound) );
    }

    if ( chr.squish_sound != NULL ){
        squish_sound = new Sound( *(chr.squish_sound) );
    }

    for ( map<string,Util::ReferenceCount<Animation> >::const_iterator it = chr.movements.begin(); it != chr.movements.end(); it++ ){
        const Util::ReferenceCount<Animation> & ani_copy = (*it).second;
        Animation * copy = new Animation(*ani_copy, this);
        setMovement(copy, copy->getName());
        // movements[ani_copy->getName()] = new Animation( *ani_copy, this );
    }

       /*
       animation_current = movements[ "idle" ];
       */

    /* Copy the movements */
    const map<int, Remap*> & his_mapper = chr.getMapper();
    for (map<int, Remap*>::const_iterator it = his_mapper.begin(); it != his_mapper.end(); it++ ){
        int id = (*it).first;
        Remap * remap = (*it).second;
        if (remap != NULL){
            mapper[id] = new Remap(*remap);
        } else {
            mapper[id] = NULL;
        }

        /*
           const map<string,Animation*> & his = (*it).second;
           map< string, Animation * > & mine = mapper[ id ];

           for ( map<string,Animation*>::const_iterator m_it = his.begin(); m_it != his.end(); m_it++ ){
           if ( (*m_it).second != NULL ){
           const Animation * const & ani_copy = (*m_it).second;
        // Animation * ani_copy = (*m_it).second;

        mine[ ani_copy->getName() ] = new Animation( *ani_copy, this );
        }
        }
        */
    }

    animation_current = getMovement("idle");

    setMap(chr.getCurrentMap());

    gibBloodImage = chr.gibBloodImage;
}
	
Network::Message Character::getCreateMessage(){
    Network::Message message;

    message.id = 0;
    message << World::CREATE_CHARACTER;
    message << getAlliance();
    message << getId();
    message << getCurrentMap();
    Global::debug( 1 ) << "Character create id " << getId() << endl;
    Global::debug( 1 ) << "Character create path: '" << getPath().path() << "'" << endl;
    Filesystem::RelativePath path = Storage::instance().cleanse(getPath());
    /*
       path.erase( 0, Util::getDataPath().length() );
       */
    Global::debug( 1 ) << "Character create sending: '" << path.path() << "'" << endl;
    message << path.path();

    return message;
}

void Character::loadSelf(const Filesystem::AbsolutePath & filename ){

    // setInvincibility( 1000 );
    TokenReader tr;

    Token * head = NULL;
    try{
        head = tr.readTokenFromFile(filename.path());
    } catch( const TokenException & ex ){
        throw LoadException(__FILE__, __LINE__, ex, string("Could not open character file: ") + filename.path());
    }
    string xls = "Load time for ";
    xls += filename.path();

    if ( *head != "character" ){
        throw LoadException(__FILE__, __LINE__, string("File is not a character: ") + filename.path());
    }

    current_map = 0;
    mapper[current_map] = NULL;

    // map<string, Filesystem::AbsolutePath> remaps;

    const Token * n = NULL;
    try{

        TokenView view = head->view();
        while (view.hasMore()){
            view >> n;

            if ( *n == "name" ){
                n->view() >> name;
                name = Util::upcase(name);
            } else if ( *n == "anim" ){

                Animation * ani = new Animation(n, this);
                setMovement(ani, ani->getName());

                /*
                if (movements[ani->getName()] != 0){
                    delete movements[ani->getName()];
                }
                movements[ani->getName()] = ani;
                */

                // Remap* remap = mapper[current_map];
                // remap->setAnimation(ani->getName(), ani);
                /*
                if ( cur[ ani->getName() ] != 0 ){
                    delete cur[ ani->getName() ];
                }
                cur[ ani->getName() ] = ani;
                */
            } else if ( *n == "health" ){
                int h;
                n->view() >> h;
                setMaxHealth( h );
                setHealth( h );
            } else if ( *n == "jump-velocity" ){
                double h;
                n->view() >> h;
                setJumpingYVelocity( h );
            } else if ( *n == "hit-sound" ){
                string _snd;
                n->view() >> _snd;
                setHit(Sound(Storage::instance().find(Filesystem::RelativePath(_snd)).path()));
            } else if ( *n == "die-sound" ){
                string _snd;
                n->view() >> _snd;
                die_sound = new Sound(Storage::instance().find(Filesystem::RelativePath(_snd)).path());
            } else if ( *n == "landed" ){
                string st;
                n->view() >> st;
                landed_sound = new Sound(Storage::instance().find(Filesystem::RelativePath(st)).path());
            } else if ( *n == "speed" ){
                n->view() >> speed;
            } else if ( *n == "type" ){
                n->view() >> type;
            } else if ( *n == "shadow" ){
                int x;
                n->view() >> x;
                setShadow( x );
            } else if (*n == "scale"){
                n->view() >> spriteScale;
                if (spriteScale < 0.01){
                    spriteScale = 0.01;
                }
            } else if ( *n == "icon" ){
                string icon_path;
                n->view() >> icon_path;
                // cout<<"Loading icon "<<icon_path<<endl;
                icon = new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath(icon_path)).path());
            } else if ( *n == "remap" ){
                string first;
                string second;
                n->view() >> first >> second;
                if (newRemap(first, second)){
                    addRemap(new Remap(Filesystem::RelativePath(first), Filesystem::RelativePath(second)));
                }
                // remaps[Filesystem::find(Filesystem::RelativePath(second)).path()] = Filesystem::find(Filesystem::RelativePath(first));
            } else {
                Global::debug(0) << "Unhandled character attribute: " << endl;
                n->print(" ");
            }

        }

        squish_sound = new Sound(Storage::instance().find(Filesystem::RelativePath("sounds/squish.wav")).path());

        if ( getMovement( "idle" ) == NULL ){
            throw LoadException(__FILE__, __LINE__, "No 'idle' movement");
        }

        if ( getMovement( "pain" ) == NULL ){
            throw LoadException(__FILE__, __LINE__, "No 'pain' movement");
        }

        if ( getMovement( "rise" ) == NULL ){
            throw LoadException(__FILE__, __LINE__, "No 'rise' movement");
        }
        /*
           if ( movements[ "grab" ] == NULL ){
           throw LoadException("No 'grab' movement");
           }
           */

        if ( getMovement( "fall" ) == NULL ){
            throw LoadException(__FILE__, __LINE__, "No 'fall' movement");
        }

    } catch (const Filesystem::NotFound & ex){
        ostringstream ss;
        ss << "Could not load character " << filename.path();
        throw LoadException(__FILE__, __LINE__, ex, ss.str());
    } catch( const TokenException & tex ){
        cout<< "TokenException: " << tex.getTrace() << endl;
        if (n != NULL){
            n->print(" ");
        }
        cout<<"* Dumping character"<<endl;
        head->print("*");
        // delete head;

        throw LoadException(__FILE__, __LINE__, tex, "Error parsing character");
    } catch (const LoadException & lex){
        ostringstream ss;
        ss << "Could not load character " << filename.path();
        throw LoadException(__FILE__, __LINE__, lex, ss.str());
    }

    // delete head;

    /*
    for ( map<string, Filesystem::AbsolutePath>::iterator it = remaps.begin(); it != remaps.end(); it++ ){
        const string & x1 = (*it).first;
        const Filesystem::AbsolutePath & alter = (*it).second;
        reMap(alter.path(), x1, getMapper().size());
    }
    */

    if (getMovement("walk") != NULL){
        if (getMovement("walk")->getKeys().size() > 0){
            Global::debug(0) << "Warning: " << getName() << " should not contain any keys for the 'walk' movement" << endl;
        }
    }

    // animation_current = movements[ "idle" ];
    animation_current = getMovement("idle");

    body_parts = getBodyParts(getMovement("idle"));
    own_stuff = true;

    addEffect(new DrawNormalEffect(this));

    path = filename;

    Graphics::RestoreState graphicsState;
    gibBloodImage = new Graphics::Bitmap(2, 2);
    gibBloodImage->clearToMask();
    gibBloodImage->circleFill(gibBloodImage->getWidth() / 2, gibBloodImage->getHeight() / 2, 1, Graphics::makeColor(255, 0, 0));
}
    
double Character::getSpriteScale() const {
    return spriteScale;
}
    
void Character::setSpriteScale(double scale){
    spriteScale = scale;
}

void Character::addRemap(Remap * remap){
    mapper[mapper.size()] = remap;
}
    
Remap * Character::getCurrentRemap() const {
    map<int, Remap*>::const_iterator find = getMapper().find(getCurrentMap());
    if (find != getMapper().end()){
        return find->second;
    }
    return NULL;
}
        
bool Character::newRemap(const std::string & from, const std::string & to){
    for (map<int, Remap*>::iterator it = mapper.begin(); it != mapper.end(); it++){
        Remap * remap = it->second;
        if (remap != NULL){
            if (remap->getFrom().path() == from &&
                remap->getTo().path() == to){
                return false;
            }
        }
    }
    return true;
}

void Character::addEffect(DrawEffect * effect){
    effects.push_back(effect);
    std::sort(effects.begin(), effects.end(), DrawEffect::compare);
}

static int nonMaskingPixels(Graphics::Bitmap * bitmap){
    int total = 0;
    for (int x = 0; x < bitmap->getWidth(); x++){
        for (int y = 0; y < bitmap->getHeight(); y++){
            if (bitmap->getPixel(x, y) != Graphics::MaskColor()){
                total += 1;
            }
        }
    }
    return total;
}

static void replacePart( vector< BodyPart > & parts, Graphics::Bitmap * bitmap ){
    if (parts.size() == 0){
        delete bitmap;
    } else {
        int i = Util::rnd(parts.size());
        delete parts[i].image;
        parts[i].image = bitmap;
    }
}
	
vector< BodyPart > Character::getBodyParts(Util::ReferenceCount<Animation> animation){
    Graphics::RestoreState state;
    vector< BodyPart > parts;

    Graphics::Bitmap * bitmap = animation->getFrame(0);
    if (bitmap == NULL){
        return parts;	
    }

    const int gib_size = 12;
    for ( int x = 0; x < bitmap->getWidth() - gib_size; x += gib_size ){
        for ( int y = 0; y < bitmap->getHeight() - gib_size; y += gib_size ){
            // Bitmap * sub = new Bitmap( *bitmap, x, y, gib_size, gib_size );
            Graphics::Bitmap * sub = new Graphics::Bitmap(gib_size, gib_size);
            sub->fill(Graphics::MaskColor());
            bitmap->Blit(x, y, 0, 0, *sub);

            for (int num = 0; num < 2; num++){
                sub->circleFill(Util::rnd(sub->getWidth()), Util::rnd(sub->getHeight()), 1, Graphics::MaskColor());
                sub->circleFill(Util::rnd(sub->getWidth()), Util::rnd(sub->getHeight()), 1, Graphics::makeColor(255,0,0));
            }

            if (100.0 * (double) nonMaskingPixels(sub) / (double) (sub->getWidth() * sub->getHeight()) < 10.0){
                delete sub;
            } else {
                parts.push_back(BodyPart( x - getWidth() / 2, getHeight() - y, sub));
            }
        }
    }

    /* TODO: cache this in a static list of bitmaps */
    const char * more[] = {
        "misc/body/arm.png",
        "misc/body/bone.png",
        "misc/body/hand.png",
        "misc/body/intestines.png",
        "misc/body/leg.png",
        "misc/body/ribcage.png",
        "misc/body/skull.png",
        "misc/body/spine.png",
        "misc/body/torso.png"
    };

    for ( unsigned int i = 0; i < sizeof(more) / sizeof(char*); i += Util::rnd(3) + 1){
        replacePart(parts, new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath(more[i])).path()));	
    }

    return parts;
}

void Character::nextMap(){
    nextMap(getCurrentMap());
}

void Character::nextMap( int x ){
    unsigned int next = x + 1;
    if (next >= mapper.size()){
        next = 0;
    }
    setMap(next);
}

int Character::getNextMap() const {
	return getNextMap( current_map );
}

int Character::getNextMap( unsigned int x ) const {
	if ( x + 1 >= mapper.size() ){
		return 0;
	}
	return x + 1;
}
	
void Character::setMap(const unsigned int x){
    current_map = x;
    if (current_map >= mapper.size()){
        current_map = mapper.size() - 1;
    }

    /*
    if (current_map < 0){
        current_map = 0;
    }
    */

    // mapper[current_map]->use(this);

    /*
       if ( current_map < 0 ){
       current_map = 0;
       }
       */

    /*
    if (animation_current != NULL){
        animation_current = getMovement(animation_current->getName());
    }
    */
}

/* swap some colors around */
/*
void Character::reMap(const string & from, const string & to, int id){
    if (mapper.find(id) != mapper.end()){
        return;
    }

    mapper[id] = new Remap(from, to, mapper[0]);
}
*/
	
bool Character::isCollidable( Object * obj ){
	if ( death >= 1 )
		return false;
	if ( obj == this )
		return false;

	if ( getThrown() ){
		// cout<<this<<": Thrown obj is "<<thrown_from<<". Obj = "<<obj<<endl;
		return thrown_from != obj;
	}

	// if ( getStatus() == Status_Fell || getStatus() == Status_Hurt )
	if ( getStatus() == Status_Fell || getStatus() == Status_Rise ){
		return false;
	}

	return obj->getAlliance() != getAlliance();
	// return true;
}
	
bool Character::isGettable(){
	return false;
}

static bool invincibility_zero(const Character * const character){
    return character->getInvincibility() <= 0;
}

void Character::setInvincibility(const int x){
    invincibility = x;
    addEffect(new DrawUntilEffect(new DrawGlowEffect(this, Graphics::makeColor(10,10,250), Graphics::makeColor(190, 190, 255), 75), invincibility_zero));
}

Util::ReferenceCount<Animation> Character::getCurrentMovement() const {
    return this->animation_current;
}
	
void Character::setMovement(Animation * animation, const string & name){
    movements[name] = animation;
}

Util::ReferenceCount<Animation> Character::getMovement(const string & name){
    map<std::string, Util::ReferenceCount<Animation> >::const_iterator find = getMovements().find(name);
    if (find != getMovements().end()){
        return find->second;
    }
    return Util::ReferenceCount<Animation>(NULL);
}

/*
Animation * Character::getMovement( const unsigned int x ){
	// cout<<"Current map = "<<current_map<<endl;
	map< string, Animation * > & xmap = mapper[ current_map ];
	// cout<<"Have "<<xmap.size()<<" animations"<<endl;

	Animation * ret = NULL;
	if ( x < xmap.size() ){
		unsigned int q = 0;
		map<string,Animation*>::iterator it;
		for ( it = xmap.begin(); q < x; q++, it++ );
		ret = (*it).second;
		// animation_current = (*it).second;
		// animation_current->reset();
	} else {
		// cout<<"Could not find animation for "<<x<<endl;
		ret = animation_current;
	}

	return ret;
}
*/

bool Character::testAnimation(){
    /*
    Global::debug(0) << "Cannot test animations anymore" << endl;
    return false;
    */
    /*
	World w;
	vector< Object * > others;
	vector< Object * > more;
	act( &others, &w, &more );
        */
    if (animation_current != NULL){
	return animation_current->Act();
    }
    return false;
}

void Character::testReset(){
	animation_current->reset();
}

void Character::testAnimation(string name){
    animation_current = getMovement(name);
    if (animation_current == NULL){
        animation_current = getMovement("idle");
    }
    animation_current->reset();
}

void Character::testAnimation( unsigned int x ){
    /*
    animation_current = getMovement(x);
    animation_current->reset();
    */

	/*
	if ( x < movements.size() ){
		int q = 0;
		map<string,Animation*>::iterator it;
		for ( it = movements.begin(); q < x; q++, it++ );
		animation_current = (*it).second;
		animation_current->reset();
	}
	*/
}
	
int Character::getAlliance() const{
	/*
	if ( getStatus() == Status_Fell && isMoving() )
		return ALLIANCE_NONE;
	*/
	if ( getThrown() )
		return ALLIANCE_NONE;

	return Object::getAlliance();
}
    
bool Character::isPlayer() const {
    return false;
}

bool Character::isGrabbable(Object * obj){
    return getStatus() == Status_Ground || getStatus() == Status_Hurt;
}

Object * Character::copy(){
	return new Character( *this );
}
	
int Character::getDamage() const{
    if ( animation_current != NULL ){
        return animation_current->getDamage();
    }
    return 0;
}
    
void Character::setSpeedBoost(double boost, int ticks){
    speedBoost.boost = boost;
    speedBoost.ticks = ticks;
}
    
double Character::getSpeed() const {
    if (speedBoost.ticks > 0){
        return speedBoost.boost * speed;
    }
    return speed;
}

double Character::getForceX() const {
    if (animation_current != NULL){
        return animation_current->getForceX();
    }
    return 0;
}

double Character::getForceY() const {
    if (animation_current != NULL){
        return animation_current->getForceY();
    }
    return 0;
}
	
void Character::setName( const string & str ){
	name = str;
}

void Character::doJump( double vx, double vz ){
    setStatus(Status_Jumping );
    setXVelocity(vx);
    setZVelocity(vz);
    initJumpingYVelocity();
    // cout<<"Jumping velocity: "<<getMaxJumpingVelocity()<<endl;
    // cout<<"Do jump: "<<getJumpingYVelocity()<<endl;
}
	
void Character::initJumpingYVelocity(){
    setYVelocity(jumping_starting_velocity);
}

/** TODO **/
void Character::jump(){
}

/** TODO **/
bool Character::isJumping(){
	return getStatus() == Status_Jumping;
}

const string Character::getName() const{
	return name;
}

void Character::fall( double x_vel, double y_vel ){
	animation_current = getMovement( "fall" );
	// animation_current = movements[ "fall" ];
	animation_current->reset();
	setStatus( Status_Fell );
	if ( getLink() != NULL ){
		getLink()->unGrab();
		setLink( NULL );
	}

	/* fall backwards */
	setXVelocity( x_vel );
	setYVelocity( y_vel );
}
	
void Character::takeDamage(World & world, ObjectAttack * obj, int damage, double forceX, double forceY){
    /* I think if death >= 1 here I should just return */
    Object::takeDamage(world, obj, damage, forceX, forceY);

    /* without this, trails might continue for all animations because the
     * stop trail event might not be seen in the current animation
     */
    trail_generator = 0;

    Global::debug( 2 ) << getName() << " has " << currentDamage() << " damage" << endl;
    if ((currentDamage() > getToughness() ||
         getHealth() <= 0 ||
         getStatus() == Status_Jumping)
        && getStatus() != Status_Fell){

        if (currentDamage() > getToughness() && getHealth() <= 0){
            setExplode(true);
            world.addMessage( explodeMessage() );
        }

        reduceDamage(currentDamage());
        /*
           animation_current = movements[ "fall" ];
           setStatus( Status_Fell );
           if ( getLink() != NULL ){
           getLink()->unGrab();
           }
           */
        
        /* fall(-1.7, 4.4 ); */

        fall(-forceX, forceY );
        world.addMessage(fallMessage(-forceX, forceY));
    } else	{
        animation_current = getMovement( "pain" );
        if ( getStatus() != Status_Grabbed ){
            setStatus( Status_Hurt );
        }
    }
    animation_current->reset();

    Global::debug(1) << this << " explode " << getExplode() << " health " << getHealth() << " death " << death << endl;
    if (! getExplode() && getHealth() <= 0 && death == 0){
        death = 1;
        setHealth(1);
        Global::debug(1) << this << " set death to 1. Health " << getHealth() << endl;

        if (die_sound){
            die_sound->play();
        }
    }

    world.addMessage(animationMessage());
    world.addMessage(healthMessage());
}
	
void Character::died( vector< Object * > & objects ){
    if (getLink() != NULL){
        getLink()->unGrab();
        setLink(NULL);
    }

    if (getExplode()){
        for ( vector< BodyPart >::iterator it = body_parts.begin(); it != body_parts.end(); it++ ){
            const BodyPart & part = *it;

            int x = (int) getX() + part.x;
            int y = (int) getY() + part.y;
            double dx = (double)(Util::rnd(11) - 5) / 4.2;
            double dy = (double)(Util::rnd(10) + 4) / 3.5;
            double dz = (double)(Util::rnd(11) - 5) / 5.0;
            objects.push_back(new Gib(x, y, (int) getZ(), dx, dy, dz, part.image, gibBloodImage));
        }

        if ( squish_sound != NULL ){
            squish_sound->play();
        }
        /*
           for ( int i = 0; i < 20 + Util::rnd( 10 ); i++ ){
        // for ( int i = 0; i < 1; i++ ){
        int x = (int) getX();
        int y = (int) (getY() + Util::rnd( getHeight() ));
        // int y = (int) (getY() - 50);

        double dx = (Util::rnd( 11 ) - 5) / 3.5;
        double dy = (Util::rnd( 10 ) + 4) / 3.0;
        objects.push_back( new Gib( x, y, (int) getZ(), dx, dy ) );
        }
        */
    }
}

/* use Utils::upcase instead */
void Character::upperCase( string & who ){
	for ( string::size_type q = 0; q < who.length(); q++ )
		if ( who[q] >= 'a' && who[q] <= 'z' )
			who[q] = (char)(who[q] - 'a' + 'A');
}
	
const string Character::getAttackName(){
	return animation_current->getName(); 
}

void Character::decreaseYVelocity(){
	y_velocity -= 0.18;
}
	
void Character::stimulate( const Stimulation & stim ){
	stim.stimulate( *(Character *) this );
}
	
void Character::decreaseXVelocity(){
	x_velocity -= 0.6;
}

void Character::decreaseZVelocity(){
	z_velocity -= 0.6;
}

void Character::landed( World * world ){
    if ( death >= 2 ) return;

    setThrown( false );

    switch (getStatus()){
        case Status_Falling : {
            if ( landed_sound ){
                landed_sound->play();
            }

            world->Quake( (int)fabs(getYVelocity()) );

            setStatus( Status_Ground );
            animation_current = getMovement( "idle" );
            animation_current->reset();
            world->addMessage( movedMessage() );
            world->addMessage( animationMessage() );
            break;
        }
        case Status_Fell : {

            if ( landed_sound ){
                landed_sound->play();
            }

            double cur = fabs(getYVelocity()) + fabs( getXVelocity());
            // cout<<getName()<<" taking "<<cur<<" from falling. Health = "<<getHealth()<<endl;
            Object::takeDamage(*world, NULL, (int)cur, 0, 0);

            world->Quake((int)fabs(getYVelocity()));

            // setStatus( Status_Hurt );

            if ( (getHealth() <= 0 && death == 0) || death == 1 ){
                Global::debug( 2 ) << this << " died from fall" << endl;
                death = 2;
                setHealth( 1 );
            } else {
                // setStatus( Status_Hurt );
                setStatus( Status_Rise );
                // animation_current = movements[ "rise" ];
                animation_current = getMovement( "rise" );
                animation_current->reset();
            }

            world->addMessage( healthMessage() );
            world->addMessage( movedMessage() );
            world->addMessage( animationMessage() );

            // cout<<getName()<<" landed: death = "<<death<<endl;

            break;
        }
        case Status_Jumping : {

            setStatus( Status_Ground );
            // animation_current = movements["idle"];
            animation_current = getMovement( "idle" );
            animation_current->reset();

            world->addMessage( movedMessage() );
            world->addMessage( animationMessage() );

            break;
        }
    }
}

void Character::setTrails(const int produce, const int life){
    trail_generator = produce;
    trail_life = life;
}

void Character::createProjectile( Projectile * projectile ){
    projectiles.push_back((Object *) projectile);
}
	
Network::Message Character::grabMessage( Object::networkid_t from, Object::networkid_t who ){
	Network::Message message;
	message.id = 0;
	message << World::GRAB;
	message << from;
	message << who;

	return message;
}

void Character::act( vector< Object * > * others, World * world, vector< Object * > * add ){

    if (speedBoost.ticks > 0){
        speedBoost.ticks -= 1;
    }

    if ( invincibility > 0 ){
        invincibility--;
    }

    for (vector<DrawEffect*>::iterator it = effects.begin(); it != effects.end(); ){
        DrawEffect * effect = *it;
        if (effect->act()){
            delete effect;
            it = effects.erase(it);
        } else {
            it++;
        }
    }

    for ( vector< Object * >::iterator it = projectiles.begin(); it != projectiles.end(); it++ ){
        Object * obj = *it;
        obj->setAlliance( getAlliance() );
        obj->setFacing( getFacing() );
    }
    add->insert( add->end(), projectiles.begin(), projectiles.end() );
    projectiles.clear();

    reduceDamage();
    // cout << getName() << " now has " << currentDamage() << endl;

    /* when the character moves not because of a move or walking */
    if ( isMoving() ){
        // cout << this << " is moving with velocities " << getXVelocity() << " " << getYVelocity() << " " << getZVelocity() << endl;

        /* force of gravity, subtract acceleration for
         * downward movement
         */
        decreaseYVelocity();
        moveX( getXVelocity() );
        moveY( getYVelocity() );
        moveZ( getZVelocity() );

        /* moveY ensures that getY() will never be below 0, but we use <=
         * to make the compiler happy
         */
        if (getY() <= 0){
            landed(world);
            setMoving(false);
        }
    }

    if ( death >= 2 ){
        // cout<<getName()<<" Death is "<<death<<endl;
        if ( ++death > 60 ){
            Global::debug( 1 ) << this << " dying" << endl;
            setHealth( 0 );
            world->addMessage( healthMessage() );
        }
    }

    if ( getStatus() == Status_Fell ){
        // setLink( NULL );

        /*
           if ( getY() == 0 ){

           double cur = fabs( getYVelocity() ) + fabs( getXVelocity() );
           cout<<getName()<<" taking "<<cur<<" from falling"<<endl;
        // Object::takeDamage( NULL, (int)cur );

        // setStatus( Status_Hurt );

        / *
        if ( getHealth() <= 0 && death == 0 ){
        death = 1;
        setHealth( 1 );
        // setStatus( Status_Dead );
        }
         * /

         setXVelocity( 0 );
         setYVelocity( 0 );
         } else 
         decreaseYVelocity();

         if ( animation_current->Act() && death < 2 ){
         if ( death == 0 ){
         if ( getY() == 0 ){
         setStatus( Status_Hurt );
         animation_current = movements[ "rise" ];
         animation_current->reset();
         }
         } else {
         death = 2;
         }
         }
         */

        animation_current->Act();

    } else if ( getStatus() == Status_Hurt ){
        if ( animation_current->Act() ){
            if ( getLink() == NULL ){
                setStatus( Status_Ground );	
            } else {
                setStatus( Status_Grabbed );
                // animation_current = movements["pain"];
                animation_current = getMovement( "pain" );
            }
        }
    } else if ( getStatus() == Status_Rise || getStatus() == Status_Get ){
        if ( animation_current->Act() ){
            animation_current = getMovement( "idle" );
            setStatus( Status_Ground );
        }
    } else if ( getStatus() == Status_Grabbed ){
        if ( ++grab_time > 120 ){
            setStatus( Status_Ground );
            if ( getLink() ){
                getLink()->unGrab();
                setLink( NULL );
            }
            world->addMessage( ungrabMessage() );
        } else {
            /* probably not necessary */
            /*
               if ( getLink() ){
               world->addMessage( grabMessage( getLink()->getId(), getId() ) );
               }
               */
        }
    }

    if (getScriptObject() != NULL){
        Script::Engine::getEngine()->objectTick(getScriptObject());
    }

    if (trail_generator > 0){
        if (trail_counter <= 1){
            if (animation_current != NULL){
                trails.push_back(animation_current->makeTrail(getRX(), getRY(), getFacing(), trail_life));
            }
            trail_counter = trail_generator;
        } else {
            trail_counter -= 1;
        }
    }

    for (vector<AnimationTrail*>::iterator it = trails.begin(); it != trails.end(); ){
        AnimationTrail * trail = *it;
        if (trail->act()){
            delete trail;
            it = trails.erase(it);
        } else {
            it++;
        }
    }


/*
   if ( isJumping() ){
// cout<<"Jumping: "<<getJumpingYVelocity()<<endl;
// moveX( getXVelocity() );
// moveY( getYVelocity() );
decreaseYVelocity();
if ( getY() == 0 ){
// setY( 0 );
setStatus( Status_Ground );
animation_current = movements["idle"];
animation_current->reset();

setXVelocity( 0 );
setYVelocity( 0 );
}
// cout<<"Y: "<<getY() <<endl;
}
*/
}
	
void Character::unGrab(){
	setStatus( Status_Ground );
	setLink( NULL );
}
	
bool Character::isGrabbed(){
	return getStatus() == Status_Grabbed;
}

void Character::grabbed( Object * obj ){
	setStatus( Status_Grabbed );

	/* make sure grabber is in front */
	// setZ( obj->getZ() - 1 );
	setX( obj->getX() );
	// moveX( obj->getFacing(), obj->getWidth() / 4 );
	moveX( obj->getFacing(), 36 );
	setZ( obj->getZ() + 1 );
	setFacing( obj->getOppositeFacing() );
	setLink( obj );
	grab_time = 0;
	setY( 0 );
	// animation_current = movements[ "pain" ];
	animation_current = getMovement( "pain" );
	animation_current->reset();
}

void Character::filterEnemies( vector< Object * > & mine, vector< Object * > * all ){
	for ( vector< Object * >::iterator it = all->begin(); it != all->end(); it++ ){
		Object * n = *it;
		int q = n->getAlliance();
		if ( q != ALLIANCE_NONE && q != getAlliance() )
			mine.push_back( n );

			/*
		if ( n->getAlliance() == ALLIANCE_PLAYER ){
			mine->push_back( n );
		}
		*/
	}
}
	
void Character::deathReset(){
}
	
bool Character::isAttacking(){	
	/*
	if ( getStatus() == Status_Fell && isMoving() )
		return true;
	*/
	if ( getThrown() )
		return true;

	if (animation_current != NULL)
		return animation_current->isAttack();
	return false;
}
	
void Character::thrown(){
	setThrown( true );
	thrown_from = getLink();
}
	
/* used to find the x/y where the attack takes place for effects */
/*
void Character::getAttackCoords( int & x, int & y){
	if ( animation_current != NULL){
		animation_current->getAttackCoords( x, y );
		if ( getFacing() == FACING_LEFT ){
			x = getRX() - x + getWidth() / 2;
		} else {
			x += getRX() - getWidth()/2;
		}
		y += getRY() - getHeight();
	}
}
*/
	
const map<string, Util::ReferenceCount<Animation> > & Character::getMovements(){
    return movements;
}
	
void Character::attacked( World * world, Object * something, vector< Object * > & objects ){
	animation_current->contacted();
        if (getScriptObject() != NULL){
            void * script = NULL;
            if (something != NULL){
                script = something->getScriptObject();
            }
            Script::Engine::getEngine()->characterAttacked(getScriptObject(), script);
        }
}

void Character::collided(World * world, ObjectAttack * obj, vector< Object * > & objects){
    /*
       last_obj = obj;
       last_collide = obj->getTicket();
       */

    // cout << this << " collided with " << obj << endl;
    Object::collided(world, obj, objects);

    bool needMove = false;
    if (obj != NULL){
        /* getting his ticket prevents being hit multiple times from the
         * same attack.
         */
        collision_objects[obj] = obj->getTicket();
        setFacing(obj->getOppositeFacing());
        needMove = true;
    }

    if (getStatus() != Status_Grabbed){
        moveX(-5);
        needMove = true;
    }

    if (needMove && world != NULL){
        world->addMessage(movedMessage());
    }
}

int Character::getRX() const {
    if (animation_current != NULL){
        if (getFacing() == FACING_LEFT){
            return Object::getRX() - animation_current->getOffsetX();
        } else {
            return Object::getRX() + animation_current->getOffsetX();
        }
    }
    return Object::getRX();
}

int Character::getRZ() const {
	/*
	if ( animation_current ){
		return Object::getZ() + animation_current->getOffsetY();
	}
	*/
	return Object::getRZ();
}

int Character::getRY() const {
    if (animation_current != NULL){
        return Object::getRY() + animation_current->getOffsetY();
    }
    return Object::getRY();
}

bool Character::realCollision( ObjectAttack * obj ){
    vector<ECollide*> myCollides = this->getNormalCollide();
    vector<ECollide*> himCollides = obj->getCollide();

    for (vector<ECollide*>::iterator mine = myCollides.begin(); mine != myCollides.end(); mine++){
        for (vector<ECollide*>::iterator him = himCollides.begin(); him != himCollides.end(); him++){

            ECollide * myCollide = *mine;
            ECollide * hisCollide = *him;
            // cout << "Obj attacking with " << obj->getAttackName() << " my collide = " << myCollide << " his collide = " << hisCollide << endl;
            if (myCollide != 0 && hisCollide){
                bool my_xflip = false;
                bool his_xflip = false;
                if (getFacing() == FACING_LEFT)
                    my_xflip = true;
                if (obj->getFacing() == FACING_LEFT)
                    his_xflip = true;

                /*
                   my_xflip = false;
                   his_xflip = false;
                   */

                int mx, my;
                int ax, ay;

                // ECollide * me = getCollide();
                // ECollide * him = obj->getCollide();
                /*
                   if ( !me ){
                // cout<<"No collide"<<endl;
                return false;
                }
                */

                mx = this->getRX() - getWidth() / 2;
                my = this->getRY() - getHeight();
                ax = obj->getRX() - obj->getWidth() / 2;
                ay = obj->getRY() - obj->getHeight();

                /*
                   mx = getRX();
                   my = getRY();
                   ax = obj->getRX();
                   ay = obj->getRY();
                   */

                // Bitmap Screen( screen );
                /*
                   myCollide->draw( *Bitmap::Screen, mx, my, Bitmap::makeColor( 255, 0, 128 ), my_xflip );
                   hisCollide->draw( *Bitmap::Screen, ax, ay, Bitmap::makeColor( 128, 255, 0 ), his_xflip );
                   */

                // cout<<"Mx: "<<mx<< " My: "<<my<<" Width: "<<myCollide->getWidth()<<" Height: "<<myCollide->getHeight()<<endl;
                // cout<<"Ax: "<<ax<< " Ay: "<<ay<<" Width: "<<hisCollide->getWidth()<<" Height: "<<hisCollide->getHeight()<<endl;

                bool b = myCollide->Collision( hisCollide, mx, my, ax, ay, my_xflip, false, his_xflip, false );
                /*
                   if ( b && false ){
                   myCollide->draw( *Bitmap::Screen, 50, 50, my_xflip );
                   hisCollide->draw( *Bitmap::Screen, 50 + (ax - mx), 50 + (ay - my), his_xflip );
                   Util::rest( 500 );
                   }
                   */
                if (b){
                    return b;
                }
            }
        }
    }
    return false;
}
	
double Character::minZDistance() const {
	if (animation_current != NULL){
		return animation_current->getMinZDistance();
	}
	return 10;
}

bool Character::collision(ObjectAttack * obj){
    if (getInvincibility() > 0){
        return false;
    }

    if (animation_current != NULL && !animation_current->canBeHit()){
        return false;
    }

    if (getThrown()){
        return false;
    }

    if (collision_objects[obj] == obj->getTicket()){
        // if ( last_obj == obj && last_collide == obj->getTicket() )
        Global::debug(3) << this << " already collided with " << obj << endl;
        return false;
    }

    return realCollision(obj);
}
	
void Character::drawLifeBar(int x, int y, Graphics::Bitmap * work){
    drawLifeBar(x, y, getHealth(), work);
}
	
/* draw a nifty translucent life bar */
/* FIXME: make this customizable */
void Character::drawLifeBar(int x, int y, int health, Graphics::Bitmap * work){
    Graphics::TranslucentBitmap translucent(*work);
    Graphics::Bitmap::transBlender( 0, 0, 0, 128 );
    const int health_height = 7;
    const int maxHealthWidth = 100;
    int max = getMaxHealth() < maxHealthWidth ? getMaxHealth() : maxHealthWidth;
    translucent.rectangleFill( x, y, x + max, y + health_height, Graphics::makeColor(192, 32, 32));
    Graphics::Bitmap::transBlender( 0, 0, 0, 64 );

    Graphics::Color colors[ 5 ] = { Graphics::makeColor(16, 162, 246),
        Graphics::makeColor(214, 184, 48),
        Graphics::makeColor(244, 16, 12),
        Graphics::makeColor(237, 173, 71),
        Graphics::makeColor(183, 217, 180)};

    int color = 0;
    for ( int s = 0; s < health; s += maxHealthWidth ){
        int e = s + maxHealthWidth < health ? s + maxHealthWidth : health;
        for ( int y1 = y; y1 <= y+health_height; y1++ ){
            translucent.rectangleFill( x, y, x + e - s, y1, colors[ color ] );
        }
        color += 1;
        if ( color > 4 ){
            color = 4;
        }
    }

    Graphics::TranslucentBitmap border(Graphics::Bitmap(*work, x, y, max+1, health_height));
    border.border(0, 1, Graphics::makeColor(255, 255, 255));
}

bool Character::touchPoint(int x, int y){
    if (animation_current != NULL){
        int relativeX = x - getRX() + getWidth() / 2;
        int relativeY = getHeight() - (getRY() - y); 
        // Global::debug(0) << "Test " << relativeX << ", " << relativeY << endl;
        const Graphics::Bitmap * frame = animation_current->getCurrentFrame();
        if (frame != NULL){
            if (frame->inRange(relativeX, relativeY) && 
                frame->getPixel(relativeX, relativeY) != Graphics::MaskColor()){
                // Global::debug(0) << " good " << endl;
                return true;
            }
        }
    }
    return false;
}

int Character::getShadowX(){
	if (animation_current != NULL){
		if ( getFacing() == FACING_LEFT ){
			return -animation_current->getShadowX();
		} else {
			return animation_current->getShadowX();
		}
	}
	return 0;
}

int Character::getShadowY(){
	if (animation_current != NULL){
		return animation_current->getShadowY();
	}
	return 0;
}
	
Network::Message Character::movedMessage(){
    Network::Message message = Object::movedMessage();
    message << getStatus();
    message << isMoving();
    return message;
}
	
Network::Message Character::explodeMessage(){
    Network::Message message;

    message.id = getId();
    message << CharacterMessages::Explode;

    return message;
}

Network::Message Character::fallMessage( double x, double y ){
    Network::Message message;

    message.id = getId();
    message << CharacterMessages::Fall;
    message << (int)(x * 100);
    message << (int)(y * 100);

    return message;
}
	
Network::Message Character::ungrabMessage(){
    Network::Message message;

    message.id = getId();
    message << CharacterMessages::Ungrab;

    return message;
}
	
Network::Message Character::showNameMessage( int amount ){
    Network::Message message;
    message.id = getId();
    message << CharacterMessages::ShowName;
    message << amount;

    return message;
}
	
Network::Message Character::healthMessage(){
    Network::Message message;

    message.id = getId();
    message << CharacterMessages::Health;
    message << getHealth();

    return message;
}

Network::Message Character::jumpMessage( double x, double z ){
    Network::Message message;
    message.id = getId();

    message << CharacterMessages::Jump;
    message << (int)(x * 100);
    message << (int)(z * 100);

    return message;
}

Network::Message Character::nameMessage() const {
    Network::Message message;
    message.id = getId();
    message << CharacterMessages::SetName;
    message << getName();

    return message;
}
	
void Character::interpretMessage(World * world, Network::Message & message ){
    int type;
    message >> type;
    namespace CM = CharacterMessages;

    /* reset because Object::interpretMessage() is going to read
     * the type as well
     */
    message.reset();
    Object::interpretMessage(world, message);
    switch (type){
        case ObjectMessages::Moved : {
            int status;
            int moving;
            message >> status;
            message >> moving;
            setStatus( status );
            setMoving( moving );
            break;
        }
        case CM::Jump : {
            int x, z;
            message >> x >> z;
            doJump(x / 100.0, z / 100.0);
            animation_current = getMovement("jump");
            break;
        }
        case CM::Health : {
            int health;
            message >> health;
            setHealth(health);
            Global::debug(1) << "Health for " << getId() << " is " << getHealth() << endl;
            break;
        }
        case CM::Explode: {
            setExplode(true);
            break;
        }
        case CM::SetName: {
            string name;
            message >> name;
            setName(name);
            break;
        }
        case CM::Fall: {
            int x, y;
            message >> x >> y;
            fall( x / 100.0, y / 100.0 );
            break;
        }
        case CM::Ungrab : {
            setStatus( Status_Ground );
            if ( getLink() ){
            getLink()->unGrab();
            setLink( NULL );
            }
            break;
        }
        case CM::Animation : {
            int map;
            message >> map;
            setMap( map );
            animation_current = getMovement( message.path );
            if ( animation_current == NULL ){
                Global::debug( 1 ) << "Could not find animation for '" << message.path << "'" << endl;
                animation_current = getMovement( "idle" );
            }
            if ( message.path != "walk" && message.path != "idle" ){
                animation_current->reset();
                nextTicket();
            }
            break;
        }
    }
}
	
Network::Message Character::animationMessage(){
    Network::Message m;

    m.id = getId();
    m << CharacterMessages::Animation;
    m << getCurrentMap();
    m.path = getCurrentMovement()->getName();

    return m;
}
	
int Character::getInvincibility() const {
    return invincibility;
}

void Character::draw(Graphics::Bitmap * work, int rel_x, int rel_y){

    /* this makes a character blink when they die. death increases
     * so after 15 game ticks the character will start blinking.
     */
    if (death >= 15){
        if ((death/8) % 2 == 0){
            return;
        }
    }

    if (animation_current != NULL){
        /* trails and shadows can be effects too at some point */

        /* draw trails */
        for (vector<AnimationTrail*>::iterator it = trails.begin(); it != trails.end(); it++){
            AnimationTrail * trail = *it;
            trail->draw(rel_x, getCurrentRemap(), work);
        }

        if (drawShadow()){
            drawShade(work, rel_x, 128, Graphics::makeColor(0,0,0), -.2, 2, 1);
            /*
            Graphics::Bitmap const * shadow = Shadow::getShadow(getShadow());
            Graphics::Bitmap::multiplyBlender(0, 0, 0, 164);
            shadow->translucent().draw( getRX() - shadow->getWidth() / 2 - rel_x + getShadowX(), (int) Object::getZ() - shadow->getHeight() / 2 + getShadowY(), *work );
            */
        }

        for (vector<DrawEffect*>::iterator it = effects.begin(); it != effects.end(); it++){
            DrawEffect * effect = *it;
            effect->draw(rel_x, getCurrentRemap(), work);
        }

        if (Global::getDebug() > 5){
            int x = (int)(getX() - rel_x);
            int y = (int) getRY();
            int x2 = x + animation_current->getRange();
            if (getFacing() == Object::FACING_LEFT){
                x2 = x - animation_current->getRange();
            }
            work->rectangle( x, y, x2, y + 1, Graphics::makeColor(255,255,255) );
        }
    }
}

const Graphics::Bitmap * Character::getCurrentFrame() const {
    if (animation_current != NULL){
        return animation_current->getCurrentFrame();
    }
    return NULL;
}

void Character::drawReflection(Graphics::Bitmap * work, int rel_x, int rel_y, int intensity){
    const Graphics::Bitmap * frame = this->getCurrentFrame();
    if (frame){
        Graphics::Bitmap::transBlender(0, 0, 0, intensity);
        int x = (int)((getRX() - rel_x) - frame->getWidth()/2);
        int y = (int)(getRZ() + getY());
        if (getFacing() == FACING_RIGHT){ 
            frame->translucent().drawVFlip(x , y, getCurrentRemap(), *work);
        } else { 
            frame->translucent().drawHVFlip(x, y, getCurrentRemap(), *work );
        }
    }
}

/* FIXME: abstract this and drawReflection so they can use similar code */
void Character::drawOutline(Graphics::Bitmap * work, int rel_x, int rel_y, int red, int green, int blue, int intensity){
    const Graphics::Bitmap * frame = this->getCurrentFrame();
    if (frame){
        Graphics::Bitmap::transBlender(red, green, blue, intensity);
        int x = (int)((getRX() - rel_x) - frame->getWidth()/2);
        int y = (int)(getRZ() + getY());
        if (getFacing() == FACING_RIGHT){ 
            frame->lit().drawVFlip(x , y, getCurrentRemap(), *work);
        } else { 
            frame->lit().drawHVFlip(x, y, getCurrentRemap(), *work );
        }
    }
}

/* draws a real looking shadow */
void Character::drawShade(Graphics::Bitmap * work, int rel_x, int intensity, Graphics::Color color, double scale, int fademid, int fadehigh){
    if (animation_current != NULL){
        const Graphics::Bitmap *bmp = animation_current->getCurrentFrame();
        const double newheight = bmp->getHeight() * scale;
        Graphics::Bitmap shade = Graphics::Bitmap::temporaryBitmap(bmp->getWidth(), (int) fabs(newheight));
        bmp->Stretch(shade);

        /* Could be slow, but meh, lets do it for now to make it look like a real shadow */
        for (int h = 0; h < shade.getHeight(); ++h){
            for (int w = 0; w < shade.getWidth(); ++w){
                Graphics::Color pix = shade.getPixel(w,h);
                if (pix != Graphics::MaskColor()){
                    shade.putPixel(w,h, Graphics::makeColor(0,0,0));
                }
            }
        }

        Graphics::Bitmap::transBlender(0, 0, 0, intensity);

        if (scale > 0){
            int x = (int)(getRX() - rel_x - bmp->getWidth()/2);
            int y = (int)(getRZ() + getY() * scale);
            if (getFacing() == FACING_RIGHT){ 
                shade.translucent().drawVFlip( x, y, *work );
            } else { 
                shade.translucent().drawHVFlip(x, y, *work );
            }
        } else if (scale < 0){
            int x = (int)((getRX() - rel_x) - bmp->getWidth()/2);
            int y = (int)((getRZ() - fabs(newheight)) + (getY() * scale));
            if (getFacing() == FACING_RIGHT){ 
                shade.translucent().draw(x + 3, y, *work );
            } else { 
                shade.translucent().drawHFlip(x - 3, y, *work );
            }
        }
    }
}

void Character::drawMugenShade(Graphics::Bitmap * work, int rel_x, int intensity, Graphics::Color color, double scale, int fademid, int fadehigh){
    if (animation_current != NULL){
        const Graphics::Bitmap *bmp = animation_current->getCurrentFrame();
        const double newheight = bmp->getHeight() * scale;
        Graphics::Bitmap shade = Graphics::Bitmap::temporaryBitmap(bmp->getWidth(), (int) fabs(newheight));
        bmp->Stretch(shade);

        /* Could be slow, but meh, lets do it for now to make it look like a real shadow */
        for (int h = 0; h < shade.getHeight(); ++h){
            for (int w = 0; w < shade.getWidth(); ++w){
                Graphics::Color pix = shade.getPixel(w,h);
                if (pix != Graphics::MaskColor()){
                    shade.putPixel(w,h, Graphics::makeColor(0,0,0));
                }
            }
        }

        int i = ((Graphics::getRed(color) * 77 + intensity) + (Graphics::getGreen(color) * 154 + intensity) + (Graphics::getBlue(color) * 25 + intensity))/256;
        i = 255 - i;
        // Bitmap::transBlender(Bitmap::getRed(color), Bitmap::getGreen(color), Bitmap::getBlue(color), i);
        Graphics::Bitmap::multiplyBlender((Graphics::getRed(color) * 77 + intensity), (Graphics::getGreen(color) * 154 + intensity), (Graphics::getBlue(color) * 25 + intensity), i);

        if (scale > 0){
            int x = (int)(getRX() - rel_x - bmp->getWidth()/2);
            int y = (int)(getRZ() + getY() * scale);
            if (getFacing() == FACING_RIGHT){ 
                shade.translucent().drawVFlip( x, y, *work );
            } else { 
                shade.translucent().drawHVFlip(x, y, *work );
            }
        } else if (scale < 0){
            int x = (int)((getRX() - rel_x) - bmp->getWidth()/2);
            int y = (int)((getRZ() - fabs(newheight)) + (getY() * scale));
            if (getFacing() == FACING_RIGHT){ 
                shade.translucent().draw(x + 3, y, *work );
            } else { 
                shade.translucent().drawHFlip(x - 3, y, *work );
            }
        }
    }
}

/*
bool Character::collision( Object * obj ){
	cout<<"X: "<<getRX()<< " Y: "<<getRY()<<endl;
	Object::collision( obj );
	return false;
}
*/
	
int Character::getWidth() const{
	if (animation_current != NULL){
		return animation_current->getWidth();
	}
	return 0;
}

int Character::getHeight() const{
	if (animation_current != NULL){
		return animation_current->getHeight();
	}
	return 0;
}

vector<ECollide*> Character::getCollide() const {
    if (animation_current != NULL){
        return animation_current->getCollide(getFacing());
    }
    Global::debug(0) << "No animation collide"<<endl;
    return vector<ECollide*>();
}
	
vector<ECollide*> Character::getNormalCollide() const {
    if (animation_current != NULL){
        return animation_current->getNormalCollide();
    }
    return vector<ECollide*>();
}

void Character::print() const{
    Global::debug(0) <<"Name: "<<name<<endl;
    Global::debug(0) <<"Health: "<<getHealth()<<endl;
    Global::debug(0) <<"Speed: "<<speed<<endl;
    Global::debug(0) <<"Type: "<<type<<endl;
    Global::debug(0) <<"Shadow: "<<shadow<<endl;
    if ( icon )
        Global::debug(0) <<"Icon: "<<icon->getPath()<<endl;
}

Character::~Character(){
    if (own_stuff){
        if (icon != NULL){
            delete icon;
        }
        for ( vector< BodyPart >::iterator it = body_parts.begin(); it != body_parts.end(); it++ ){
            BodyPart & b = *it;
            delete b.image;
        }
    }

    for ( map< int, Remap*>::iterator it = mapper.begin(); it != mapper.end(); it++ ){
        Remap * remap = (*it).second;
        delete remap;
    }

    for (vector<AnimationTrail*>::iterator it = trails.begin(); it != trails.end(); it++){
        delete (*it);
    }

    for (vector<DrawEffect*>::iterator it = effects.begin(); it != effects.end(); it++){
        delete (*it);
    }

    delete die_sound;
    delete landed_sound;
    delete squish_sound;

    /*
       for ( map<string,Animation*>::iterator it = movements.begin(); it != movements.end(); it++ ){
       Animation *& aa = (*it).second;
       delete aa;
       }
       */

    /*
       for ( vector< Animation * >::iterator it = animation_attacks.begin(); it != animation_attacks.end(); it++ )
       delete *it;
       */
}

}
