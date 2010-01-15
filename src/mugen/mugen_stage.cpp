#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <ostream>
#include <sstream>
#include <iostream>

#include "mugen_stage.h"

#include "util/funcs.h"
#include "util/file-system.h"
#include "util/bitmap.h"
#include "game/console.h"
#include "object/animation.h"
#include "object/object.h"
// #include "object/character.h"
#include "object/object_attack.h"
// #include "object/player.h"
#include "globals.h"
#include "factory/font_render.h"
#include "ast/all.h"
#include "util/timedifference.h"
#include "character.h"

#include "parser/all.h"

#include "mugen_animation.h"
#include "mugen_background.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"

using namespace std;

// Some static variables
static const int CONTROLLER_VALUE_NOT_SET = -999999;
static const int DEFAULT_BACKGROUND_ID = -9999;
static const int DEFAULT_OBJECT_OFFSET = 160;
static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;
static const double DEFAULT_JUMP_VELOCITY = 7.2;
static const int CONSOLE_SIZE = 95;
static const double DEFAULT_X_JUMP_VELOCITY = 2.2;

namespace Mugen{
Spark::Spark(int x, int y, MugenAnimation * animation):
x(x), y(y), animation(animation){
}

void Spark::draw(const Bitmap & work){
    animation->render(x, y, work);
}

void Spark::logic(){
    animation->logic();
}

bool Spark::isDead(){
    return animation->getPosition() == 0;
}

Spark::~Spark(){
    delete animation;
}

}


static bool centerCollision( Mugen::Character *p1, Mugen::Character *p2 ){
    //p1->getCurrentMovement()->getCurrentFrame();
    /* FIXME! */
    /*
    const int p1width = p1->getCurrentMovement()->getCurrentFrame()->getWidth(), p1height = p1->getCurrentMovement()->getCurrentFrame()->getHeight();
    const int p2width = p2->getCurrentMovement()->getCurrentFrame()->getWidth(), p2height = p2->getCurrentMovement()->getCurrentFrame()->getHeight();
    
    const double x1 = p1->getX() + ((p1width/2)/2), y1 = 0, x2 = p1->getX() + p1width - ((p1width/2)/2), y2 = p1height, 
	      x3 = p2->getX() + ((p2width/2)/2), y3 = 0, x4 = p2->getX() + p2width - ((p2width/2)/2), y4 = p2height;
    
    if ( x1 < x3 && x1 < x4 &&
	    x2 < x3 && x2 < x4 ) return false;
    if ( x1 > x3 && x1 > x4 &&
	    x2 > x3 && x2 > x4 ) return false;
    if ( y1 < y3 && y1 < y4 &&
	    y2 < y3 && y2 < y4 ) return false;
    if ( y1 > y3 && y1 > y4 &&
	    y2 > y3 && y2 > y4 ) return false;
            */
    
    return true;
}

MugenStage::MugenStage(const string & location):
World(),
location(location),
baseDir(""),
name(""),
startx(0),
starty(0),
boundleft(-150),
boundright(150),
boundhigh(-25),
boundlow(0),
verticalfollow(.2),
floortension(0),
tension(60),
p1startx(-70),
p1starty(0),
p1startz(0),
p1facing(1),
p2startx(70),
p2starty(0),
p2startz(0),
p2facing(-1),
leftbound(-1000),
rightbound(1000),
topbound(-25),
botbound(0),
/*topz(0),
botz(50),
topscale(1),
botscale(1.2),*/
screenleft(15),
screenright(15),
zoffset(200),
zoffsetlink(DEFAULT_BACKGROUND_ID),
autoturn(true),
resetBG(true),
shadowIntensity(128),
reflect(false),
shadowColor(Bitmap::makeColor(0,0,0)),
shadowYscale(0.4),
shadowFadeRangeHigh(0),
shadowFadeRangeMid(0),
reflectionIntensity(0),
//sffFile(""),
//debugbg(false),
board(0),
xaxis(0),
yaxis(0),
camerax(0),
cameray(0),
stageStart(false),
ticker(0),
totalRounds(3),
round(1),
totalTime(99),
time(99),
p1points(0),
p2points(0),
console(new Console::Console(CONSOLE_SIZE)),
debugMode(false),
inleft(0),
inright(0),
onLeftSide(0),
onRightSide(0),
inabove(0),
loaded(false){
    initializeName();
}

MugenStage::MugenStage( const char * location ):
World(),
location( std::string(location) ),
baseDir(""),
name(""),
startx(0),
starty(0),
boundleft(-150),
boundright(150),
boundhigh(-25),
boundlow(0),
verticalfollow(.2),
floortension(0),
tension(60),
p1startx(-70),
p1starty(0),
p1startz(0),
p1facing(1),
p2startx(70),
p2starty(0),
p2startz(0),
p2facing(-1),
leftbound(-1000),
rightbound(1000),
topbound(-25),
botbound(0),
/*topz(0),
botz(50),
topscale(1),
botscale(1.2),*/
screenleft(15),
screenright(15),
zoffset(200),
zoffsetlink(DEFAULT_BACKGROUND_ID),
autoturn(true),
resetBG(true),
shadowIntensity(128),
reflect(false),
shadowColor(Bitmap::makeColor(0,0,0)),
shadowYscale(0.4),
shadowFadeRangeHigh(0),
shadowFadeRangeMid(0),
reflectionIntensity(0),
//sffFile(""),
//debugbg(false),
background(0),
board(0),
xaxis(0),
yaxis(0),
camerax(0),
cameray(0),
stageStart(false),
ticker(0),
totalRounds(3),
round(1),
totalTime(99),
time(99),
p1points(0),
p2points(0),
console(new Console::Console(CONSOLE_SIZE)),
debugMode(false),
inleft(0),
inright(0),
onLeftSide(0),
onRightSide(0),
inabove(0),
loaded(false){
    initializeName();
}

MugenStage::~MugenStage(){
    cleanup();
}

/* fix */
void MugenStage::loadSectionCamera(Ast::Section * section){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "startx"){
                (*simple) >> startx;
            } else if (*simple == "starty"){
                *simple >> starty;
            } else if (*simple == "boundleft"){
                *simple >> boundleft;
            } else if (*simple == "boundright"){
                *simple >> boundright;
            } else if (*simple == "boundhigh"){
                *simple >> boundhigh;
            } else if (*simple == "boundlow"){
                // This is always 0 so don't grab it
                // *content->getNext() >> boundlow;
            } else if (*simple == "verticalfollow"){
                *simple >> verticalfollow;
                if (verticalfollow > 1){
                    verticalfollow = 1;
                } else if (verticalfollow < 0){
                    verticalfollow = 0;
                }
            } else if (*simple == "floortension"){
                *simple >> floortension;
            } else if (*simple == "tension"){
                *simple >> tension;
            } else {
                throw MugenException( "Unhandled option in Camera Section: " + simple->toString());
            }
        }
    }
}

void MugenStage::loadSectionInfo(Ast::Section * section){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "name"){
                /* use setName() here */
                *simple >> name;
                Global::debug(1) << "Read name '" << name << "'" << endl;
            } else if (*simple == "author"){
                // Seems to be that some people think that author belongs in background defs
                std::string temp;
                *simple >> temp;
                Global::debug(1) << "Made by this guy: '" << temp << "'" << endl;
            } else {
                throw MugenException( "Unhandled option in Info Section: " + simple->toString());
            }
        }
    }
}

void MugenStage::loadSectionPlayerInfo(Ast::Section * section){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "p1startx"){
                *simple >> p1startx;
            } else if (*simple == "p1starty"){
                *simple >> p1starty;
            } else if (*simple == "p1startz"){
                *simple >> p1startz;
            } else if (*simple == "p1facing"){
                *simple >> p1facing;
            } else if (*simple == "p2startx"){
                *simple >> p2startx;
            } else if (*simple == "p2starty"){
                *simple >> p2starty;
            } else if (*simple == "p2startz"){
                *simple >> p2startz;
            } else if (*simple == "p2facing"){
                *simple >> p2facing;
            } else if (*simple == "leftbound"){
                *simple >> leftbound;
            } else if (*simple == "rightbound"){
                *simple >> rightbound;
            } else if (*simple == "topbound"){
                *simple >> topbound;
            } else if (*simple == "botbound"){
                *simple >> botbound;
            } else {
                throw MugenException("Unhandled option in PlayerInfo Section: " + simple->toString());
            }
        }
    }
}

void MugenStage::loadSectionBound(Ast::Section * section){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "screenleft"){
                *simple >> screenleft;
            } else if (*simple == "screenright"){
                *simple >> screenright;
            } else {
                throw MugenException("Unhandled option in Bound Section: " + simple->toString());
            }
        }
    }
}

void MugenStage::loadSectionStageInfo(Ast::Section * section){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "zoffset" ){
                *simple >> zoffset;
            } else if (*simple == "zoffsetlink" ){
                *simple >> zoffsetlink;
            } else if (*simple == "autoturn"){
                *simple >> autoturn;
            } else if (*simple == "resetbg"){
                *simple >> resetBG;
            } else {
                throw MugenException("Unhandled option in StageInfo Section: " + simple->toString());
            }
        }
    }
}

void MugenStage::loadSectionShadow(Ast::Section * section, cymk_holder & shadow){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "intensity"){
                *simple >> shadow.k;
            // *content->getNext() >> shadowIntensity;
            // shadow.k = shadowIntensity;
            } else if (*simple == "reflect"){
                *simple >> reflect;
            } else if (*simple == "color"){
                *simple >> shadow.c;
                *simple >> shadow.y;
                *simple >> shadow.m;
            } else if (*simple == "yscale"){
                *simple >> shadowYscale;
            } else if (*simple == "fade.range"){
                *simple >> shadowFadeRangeHigh;
                *simple >> shadowFadeRangeMid;
            } else {
                throw MugenException("Unhandled option in Shadow Section: " + simple->toString());
            }
        }
    }
}

void MugenStage::loadSectionReflection(Ast::Section * section){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "intensity"){
                *simple >> reflectionIntensity;
            } else {
                throw MugenException("Unhandled option in Reflection Section: " + simple->toString());
            }
        }
    }
}

static bool matchRegex(const string & str, const string & regex){
    return Util::matchRegex(str, regex);
}

static string regexResult(const string & str, const string & regex){
    return "";
}

void MugenStage::load(){
    if (loaded){
        return;
    }

    // Lets look for our def since some people think that all file systems are case insensitive
    baseDir = Filesystem::find("mugen/stages/");
    Global::debug(1) << baseDir << endl;
    if (location.find(".def")==std::string::npos){
	location+=".def";
    }
    // Get correct directory
    baseDir = Mugen::Util::getFileDir(baseDir + location);
    location = Mugen::Util::stripDir(location);
    const std::string ourDefFile = Mugen::Util::getCorrectFileLocation(baseDir, location);//Mugen::Util::fixFileName( baseDir, std::string(location) );
    
    if (ourDefFile.empty()){
        throw MugenException( "Cannot locate stage definition file for: " + location );
    }
    
    std::string filesdir = "";
    
    size_t strloc = location.find_last_of("/");
    if (strloc != std::string::npos){
	filesdir = location.substr(0, strloc);
	filesdir += "/";
    }
    
    Global::debug(1) << "Got subdir: " << filesdir << endl;

    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Def::main(ourDefFile));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + ourDefFile + " in" + diff.printTime("") << endl;
    // list<Ast::Section*> * sections = (list<Ast::Section*>*) Mugen::Def::main(ourDefFile);

    struct cymk_holder shadow;
    
    /* Extract info for our first section of our stage */
    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
        /* this should really be head = Mugen::Util::fixCase(head) */
	head = Mugen::Util::fixCase(head);
	if (head == "info"){
            loadSectionInfo(section);
        } else if (head == "camera"){
            loadSectionCamera(section);
	} else if (head == "playerinfo"){
            loadSectionPlayerInfo(section);
	} else if (head == "scaling"){
            /* not used anymore.. print a warning? */
        } else if (head == "bound"){
            loadSectionBound(section);
	} else if (head == "stageinfo"){
            loadSectionStageInfo(section);
	} else if (head == "shadow"){
            loadSectionShadow(section, shadow);
	} else if (head == "reflection"){
            loadSectionReflection(section);
        /* search for bgdef instead of just assuming its there */
	} else if (matchRegex(head, ".*bgdef.*")){
	    // Background management
            vector<Ast::Section*> backgroundStuff = Mugen::Util::collectBackgroundStuff(section_it, parsed.getSections()->end());
	    MugenBackgroundManager *manager = new MugenBackgroundManager(baseDir, backgroundStuff, ticker, 0);
	    background = manager;
	    Global::debug(1) << "Got background: '" << manager->getName() << "'" << endl;
	} else if (head == "music" ){
            /* Ignore for now */
        } else {
            throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
        }
    }
    
    // Setup board our worksurface to the proper size of the entire stage 320x240 :P
    Global::debug(1) << "Creating level size of Width: " << abs(boundleft) + boundright << " and Height: " << abs(boundhigh) + boundlow << endl;
    //board = new Bitmap( DEFAULT_WIDTH, DEFAULT_HEIGHT );
    // Nope we need it to be the size of the entire board... we then pan the blit so our characters will stay put without fiddling with their x coordinates
    board = new Bitmap( abs(boundleft) + boundright + DEFAULT_WIDTH, abs(boundhigh) + boundlow + DEFAULT_HEIGHT);
    camerax = startx;
    cameray = starty;
    xaxis = (abs(boundleft) + boundright + DEFAULT_WIDTH)/2;//abs(boundleft);
    yaxis = abs(boundhigh);
    
    // Preload background
    background->preload( startx, starty );
    
    // zoffsetlink
    if( zoffsetlink != DEFAULT_BACKGROUND_ID ){
	// Link zoffset to id
	zoffset = background->getBackground(zoffsetlink)->y;
    }

    int r, g, b;
    Bitmap::cymkToRGB(shadow.c, shadow.y, shadow.m, shadow.k, &r, &g, &b);
    Global::debug(1) << "Shadow c/y/m/k " << shadow.c << " " << shadow.y << " " << shadow.m << " " << shadow.k << " r/g/b " << r << " " << g << " " << b << endl;
    shadowColor = Bitmap::makeColor(r, g, b);

    /* shadowIntensity is used as the alpha value. its some combination of the
     * cymk components but I'm not sure what it is. This is relatively close
     * but its definately not 100% accurate.
     */
    shadowIntensity = Util::min((shadow.c + shadow.y + shadow.m + shadow.k * 2) / 3, 255);
    Global::debug(1) << "Shadow intensity " << shadowIntensity << endl;

    Mugen::Util::readSprites(Filesystem::find("mugen/data/fightfx.sff"), "", effects);
    sparks = Mugen::Util::loadAnimations(Filesystem::find("mugen/data/fightfx.air"), effects);

    /*
    for (Mugen::SpriteMap::iterator it = effects.begin(); it != effects.end(); it++){
        Global::debug(-1) << "Effect group " << (*it).first << endl;
    }

    spark = new MugenAnimation();
    for (Mugen::GroupMap::iterator it = effects[0].begin(); it != effects[0].end(); it++){
        MugenFrame * frame = new MugenFrame();
        frame->sprite = (*it).second;
        frame->xoffset = 0;
        frame->yoffset = 0;
        frame->time = 2;
        frame->effects.trans = Mugen::ADD;
        spark->addFrame(frame);
    }
    */
    
    // Console stuff
    console->setTextHeight(10);
    console->setTextWidth(10);
    
    // Stage is loaded
    loaded = true;
}

void MugenStage::setCamera( const double x, const double y ){ 
    camerax = x; cameray = y; 
    // Camera boundaries
    if( camerax < boundleft ) camerax = boundleft;
    else if( camerax > boundright )camerax = boundright;
    if( cameray < boundhigh ) cameray = boundhigh;
    else if( cameray > boundlow )cameray = boundlow;
}
void MugenStage::moveCamera( const double x, const double y ){ 
    camerax += x; cameray += y; 
    // Camera boundaries
    if( camerax < boundleft ) camerax = boundleft;
    else if( camerax > boundright )camerax = boundright;
    if( cameray < boundhigh ) cameray = boundhigh;
    else if( cameray > boundlow )cameray = boundlow;
}

bool MugenStage::doCollisionDetection(Mugen::Character * obj1, Mugen::Character * obj2){
    const vector<MugenArea> & attacks = obj1->getCurrentAnimation()->getAttackBoxes();
    const vector<MugenArea> & defense = obj2->getCurrentAnimation()->getDefenseBoxes();

    for (vector<MugenArea>::const_iterator attack_i = attacks.begin(); attack_i != attacks.end(); attack_i++){
        for (vector<MugenArea>::const_iterator defense_i = defense.begin(); defense_i != defense.end(); defense_i++){
            const MugenArea & attack = *attack_i;
            const MugenArea & defense = *defense_i;
            if (attack.collision(obj1->getX(), obj1->getY(), defense, obj2->getX(), obj2->getY())){
                return true;
            }
        }
    }

    return false;
}

void MugenStage::addSpark(int x, int y, int sparkNumber){
    if (sparks[sparkNumber] == 0){
        ostringstream out;
        out << "No spark number for " << sparkNumber;
        throw MugenException(out.str());
    }
    Mugen::Spark * spark = new Mugen::Spark(x, y, new MugenAnimation(*sparks[sparkNumber]));
    showSparks.push_back(spark);
}

void MugenStage::physics(Object * player){

    Mugen::Character * mugen = (Mugen::Character *) player;
    mugen->moveX(mugen->getXVelocity());
    mugen->moveY(-mugen->getYVelocity());

    if (mugen->canTurn()){
        for (vector<Object*>::iterator enem = objects.begin(); enem != objects.end(); ++enem){
            Mugen::Character * enemy = (Mugen::Character*) *enem;
            if (enemy->getAlliance() != mugen->getAlliance()){
                if ((enemy->getX() > mugen->getX() && mugen->getFacing() != Object::FACING_RIGHT) ||
                    (enemy->getX() < mugen->getX() && mugen->getFacing() != Object::FACING_LEFT)){
                    mugen->doTurn();
                }
            }
        }
    }

    if (mugen->getCurrentPhysics() == Mugen::Physics::Stand ||
        mugen->getCurrentPhysics() == Mugen::Physics::Crouch){
        /* friction */
        if (mugen->getY() == 0){
            double friction = mugen->getStandingFriction();
            /*
            if (mugen->getXVelocity() < -friction){
                mugen->setXVelocity(mugen->getXVelocity() + friction);
            } else if (mugen->getXVelocity() > friction){
                mugen->setXVelocity(mugen->getXVelocity() - friction);
            }
            */
            mugen->setXVelocity(mugen->getXVelocity() * friction);
        }
    }

    if (mugen->getMoveType() == Mugen::Move::Attack){

        for (vector<Object*>::iterator enem = objects.begin(); enem != objects.end(); ++enem){
            Mugen::Character * enemy = (Mugen::Character*) *enem;
            if (enemy->getAlliance() != mugen->getAlliance() && enemy->canBeHit(mugen)){
                if (doCollisionDetection(mugen, enemy)){
                    /* do hitdef stuff */
                    // Global::debug(0) << "Collision!" << endl;
                    /* the hit state */
                    addSpark(mugen->getHit().sparkPosition.x + enemy->getRX(), mugen->getHit().sparkPosition.y + mugen->getRY(), mugen->getHit().spark);
                    enemy->doHit(mugen->getHit());
                    // enemy->changeState(5000);
                }
            }
        }
    }

    if (mugen->getCurrentPhysics() == Mugen::Physics::Air){
        /* gravity */
        if (mugen->getY() > 0){
            double gravity = mugen->getGravity();
            mugen->setYVelocity(mugen->getYVelocity() + gravity);
        } else if (mugen->getYVelocity() > 0){
            /* change to the landing state */
            mugen->changeState(52);
        }
    }

    // Check collisions
    for (vector<Object*>::iterator enem = objects.begin(); enem != objects.end(); ++enem){
        Object *enemy = *enem;
        if (player->getAlliance() != enemy->getAlliance()){
            // Do stuff for players
            if (isaPlayer( enemy )){
                // He collides with another push him away
                if ( player->collision( (ObjectAttack*)enemy ) && centerCollision( ((Mugen::Character *)player), ((Mugen::Character *)enemy) ) ){
                    if ( enemy->getX() < player->getX() ){
                        /* FIXME! */
                        enemy->moveLeft(1);
                        // enemy->moveLeft( ((Mugen::Character *)player)->getSpeed() );
                    }
                    else if ( enemy->getX() > player->getX() ){
                        /* FIXME! */
                        enemy->moveRight(1);
                        // enemy->moveRight( ((Mugen::Character *)player)->getSpeed() );
                    }
                }
                // autoturn need to do turning actions
                if (autoturn){
                    if (isaPlayer( player )){
                        /* FIXME! */
                        /*
                           if (enemy->getX() > player->getX() && enemy->getFacing() != Object::FACING_LEFT && ((Mugen::Character *)enemy)->getStatus() == Status_Ground){
                           enemy->setFacing(Object::FACING_LEFT);
                           }

                           if (enemy->getX() < player->getX() && enemy->getFacing() != Object::FACING_RIGHT && ((Mugen::Character *)enemy)->getStatus() == Status_Ground){
                           enemy->setFacing(Object::FACING_RIGHT);
                           }
                           */
                    }
                }
            }
            // Attacking
            /*if ( player->isAttacking() ){
              ObjectAttack * playerAttack = (ObjectAttack *)player;
              if ( enemy != player && enemy->isCollidable( player ) && player->isCollidable( enemy ) ){
              if ( player->ZDistance( enemy ) <= playerAttack->minZDistance() && enemy->collision( playerAttack ) ){ 
              double x = 0, y = 0;

              x = enemy->getX();
              y = enemy->getRY() - enemy->getHeight() + enemy->getHeight() / 3;*/
        /*
           if ( bang != NULL ){
           Object * addx = bang->copy();
           addx->setX( x );
           addx->setY( 0 );
           addx->setZ( y+addx->getHeight()/2 );
           addx->setHealth( 1 );
           added_effects.push_back( addx );
           }
           */
        /*
           playerAttack->attacked(this, enemy, add );
           enemy->collided( playerAttack, add );
           enemy->takeDamage( this, playerAttack, playerAttack->getDamage() );
           }    
           }
           }*/
        }
    }
}

void MugenStage::logic( ){
    Console::ConsoleEnd & cend = Console::Console::endl;
    // camera crap
    if (quake_time > 0){
	quake_time--;
    }

    for (vector<Mugen::Spark*>::iterator it = showSparks.begin(); it != showSparks.end();){ 
        Mugen::Spark * spark = *it;
        spark->logic();

        /* if the spark looped then kill it */
        if (spark->isDead()){
            delete spark;
            it = showSparks.erase(it);
        } else {
            it++;
        }
    }
    
    // implement some stuff before we actually begin the round then start the round
    if (!stageStart){
        stageStart = true;
    }

    // Run our ticker on and on like energizer bunnies (tm)
    ticker++;
    
    const double diffx = startx - camerax;
    const double diffy = starty - cameray;
    
    // Clear console so we can see our debug
    console->clear();
    
    //zoffsetlink
    const MugenBackground *zlinkbackground = 0;
    if (zoffsetlink != DEFAULT_BACKGROUND_ID){
	zlinkbackground = background->getBackground(zoffsetlink);
	zoffset = zlinkbackground->y;
    }
    *console << "zoffsetlink ID: " << zoffsetlink << " | zoffset: " << zoffset << " | floortension: " << floortension << cend;
    
    // Backgrounds
    background->logic( diffx, diffy, xaxis + camerax, yaxis + cameray );
    
    // Players go in here
    std::vector<Object *> add;
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); ++it){
        /* use local variables more often, iterators can be easily confused */
        Object * player = *it;
        player->act( &objects, this, &add);
        physics(player);
	
	if ( isaPlayer(player)){
	    // Lets check their boundaries and camera whateva
	    updatePlayer( player );
	    
	    // Lets put their health back on the radar until we get the stages ready for real games
	    if ( player->getHealth() < 30){//player->getMaxHealth() ){
		player->setHealth( player->getMaxHealth() );
	    }
	    
	    // Update old position
	    playerInfo[player].oldx = player->getX();
	    playerInfo[player].oldy = player->getY();
		
	// Non players, objects, projectiles misc
	} else if ( !isaPlayer(player) && player->getHealth() <= 0 ){
	    delete player;
	    it = objects.erase( it );
	    if( it == objects.end() ) break;
	}

	// Debug crap put it on console
	*console << "Object: " << player << " x: " << player->getX() << " y: " << player->getY() << cend;
    }
    objects.insert(objects.end(),add.begin(),add.end());
    
    // Correct camera
    if ((verticalfollow > 0) && !inabove && (getCameraY() < 0)){
	moveCamera( 0, verticalfollow * 3.2 );
    }
    
    // Console
    *console << "Camera X: " << getCameraX() << " Camera Y: " << getCameraY() << cend;
    *console << "Frames: " << getTicks() << cend;
    console->act();
}
	
void MugenStage::render( Bitmap *work ){
    
    // Background
    background->renderBack( (xaxis + camerax) - DEFAULT_OBJECT_OFFSET, yaxis + cameray, (DEFAULT_WIDTH + (abs(boundleft) + boundright)), DEFAULT_HEIGHT + abs(boundhigh) + boundlow, board );
    
    // Players go in here
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
	Object *obj = *it;
	// Reflection
	if (reflectionIntensity)obj->drawReflection( board, 0, reflectionIntensity );
	// Shadow
	obj->drawShade( board, 0, shadowIntensity, shadowColor, shadowYscale, shadowFadeRangeMid, shadowFadeRangeHigh);
        obj->draw( board, 0);
    }

    for (vector<Mugen::Spark*>::iterator it = showSparks.begin(); it != showSparks.end(); it++){
        Mugen::Spark * spark = *it;
        spark->draw(*board);
    }

    // Foreground
    background->renderFront( (xaxis + camerax) - DEFAULT_OBJECT_OFFSET, yaxis + cameray, (DEFAULT_WIDTH + (abs(boundleft) + boundright)), DEFAULT_HEIGHT + abs(boundhigh) + boundlow, board );
    
    // Player debug
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
	if (isaPlayer(*it)){
            Mugen::Character *character = (Mugen::Character*)*it;
	    // Player debug crap
	    if (debugMode){
		// Players x positioning
		board->vLine( 150, (int)character->getX(), (int)character->getZ(), Bitmap::makeColor( 255, 0, 0));
	    }
	}
    }
    
    // Debug crap for board coordinates
    if (debugMode){
	board->hLine( 0, abs(boundhigh) + zoffset, board->getWidth(), Bitmap::makeColor( 0,255,0 ));
	board->vLine( 0, xaxis, board->getHeight(), Bitmap::makeColor(255,0,0));
    }
    
    board->Blit( (int)(abs(boundleft) + camerax) + ( quake_time > 0 ? Util::rnd( 9 ) - 4 : 0 ), (int)(yaxis + cameray) + ( quake_time > 0 ? Util::rnd( 9 ) - 4 : 0 ), DEFAULT_WIDTH, DEFAULT_HEIGHT, 0,0, *work);
    
    // Debug crap for screen coordinates
    if (debugMode){
	work->vLine( 0, tension, 240, Bitmap::makeColor( 0,255,0 ));
	work->vLine( 0, 320 - tension, 240, Bitmap::makeColor( 0,255,0 ));
    }
    
    // Life bars, will eventually be changed out with mugens interface
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
	int p1Side = 5;
	int p2Side = 5;
	if (isaPlayer(*it)){
            Mugen::Character *character = (Mugen::Character*)*it;
	    if ( character->getAlliance() == Player1Side ){
                /* FIXME! */
		// character->drawLifeBar( 5, p1Side, work );
		p1Side +=10;
	    } else if ( character->getAlliance() == Player2Side ){
                /* FIXME! */
		// character->drawLifeBar( 215, p2Side, work );
		p2Side +=10;
	    }
	}
    }

    
    // Render console
    console->draw( *work );
}

void MugenStage::reset( ){
    camerax = startx; cameray = starty;
    /*for( std::vector< MugenBackground * >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	// reset just reloads it to default
	MugenBackground *background = *i;
	if (resetBG){
	    background->preload( startx, starty );
	}
    }
    for( std::vector< MugenBackground * >::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	// reset
	MugenBackground *background = *i;
	if (resetBG){
	    background->preload( startx, starty );
	}
    }*/
    background->reset(startx,starty,resetBG);
    
    // Reset player positions
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
	Object *player = *it;
	if( player->getAlliance() == Player1Side ){
	    //((Player *)player)->deathReset();
	    player->setX( DEFAULT_OBJECT_OFFSET + abs(boundleft) + p1startx );
	    player->setY( p1starty );
	    player->setZ( zoffset );
	    player->setFacing( Object::FACING_RIGHT );
	    playerInfo[player].oldx = player->getX();
	    playerInfo[player].oldy = player->getY();
	    playerInfo[player].leftTension = false;
	    playerInfo[player].rightTension = false;
	    playerInfo[player].leftSide = false;
	    playerInfo[player].rightSide = false;
	    playerInfo[player].jumped = false;
	} else if( player->getAlliance() == Player2Side ){
	    //((Player *)player)->deathReset();
	    player->setX( DEFAULT_OBJECT_OFFSET + abs(boundleft) + p2startx );
	    player->setY( p2starty );
	    player->setZ( zoffset );
	    player->setFacing( Object::FACING_LEFT );
	    playerInfo[player].oldx = player->getX();
	    playerInfo[player].oldy = player->getY();
	    playerInfo[player].leftTension = false;
	    playerInfo[player].rightTension = false;
	    playerInfo[player].leftSide = false;
	    playerInfo[player].rightSide = false;
	    playerInfo[player].jumped = false;
	}
    }
    
    inleft = inright = onLeftSide = onRightSide = 0;
}

// Add player1 people
void MugenStage::addp1( Object * o ){
    o->setAlliance(Player1Side);
    o->setX( DEFAULT_OBJECT_OFFSET + abs(boundleft) + p1startx );
    o->setY( p1starty );
    o->setZ( zoffset );
    o->setFacing( Object::FACING_RIGHT );
    objects.push_back(o);
    players.push_back(o);
    /* FIXME! */
    // ((Mugen::Character *)o)->setJumpingYVelocity(DEFAULT_JUMP_VELOCITY);
    playerInfo[o].oldx = o->getX();
    playerInfo[o].oldy = o->getY();
    playerInfo[o].leftTension = false;
    playerInfo[o].rightTension = false;
    playerInfo[o].leftSide = false;
    playerInfo[o].rightSide = false;
    playerInfo[o].jumped = false;
}

// Add player2 people
void MugenStage::addp2( Object * o ){
    o->setAlliance(Player2Side);
    o->setX( DEFAULT_OBJECT_OFFSET + abs(boundleft) + p2startx );
    o->setY( p2starty );
    o->setZ( zoffset );
    o->setFacing( Object::FACING_LEFT );
    objects.push_back(o);
    players.push_back(o);
    /* FIXME! */
    // ((Mugen::Character *)o)->setJumpingYVelocity(DEFAULT_JUMP_VELOCITY);
    playerInfo[o].oldx = o->getX();
    playerInfo[o].oldy = o->getY();
    playerInfo[o].leftTension = false;
    playerInfo[o].rightTension = false;
    playerInfo[o].leftSide = false;
    playerInfo[o].rightSide = false;
    playerInfo[o].jumped = false;
}

// Console
void MugenStage::toggleConsole(){ 
    console->toggle(); 
}

void MugenStage::toggleDebug(){
    debugMode = !debugMode;
    for ( vector< Object * >::iterator it = players.begin(); it != players.end(); it++ ){
        Mugen::Character *player = (Mugen::Character *)(*it);
	player->toggleDebug();
    }
}

void MugenStage::act(){
    logic();
}

void MugenStage::draw( Bitmap * work ){
    render(work);
}

void MugenStage::addObject( Object * o ){ /* Does nothing */ }
bool MugenStage::finished() const { return false; }
void MugenStage::reloadLevel() throw( LoadException ){ 
    cleanup();
    loaded = false;
    load(); 
}
Script::Engine * MugenStage::getEngine() const { return NULL; }
/* upper left hand corner of the screen */
int MugenStage::getX(){
    return (int)getCameraX();
}

int MugenStage::getY(){
    return (int)getCameraY();
}
/* this shouldn't be here */
// I guess ignore this one
const deque<Bitmap*> & MugenStage::getScreenshots(){
    return garbage;
}
int MugenStage::levelLength() const { return 0; }
// Since this isn't a paintown level, I guess block wouldn't apply
const Block * MugenStage::currentBlock() const { return NULL; }
/* bleh.. */
void MugenStage::addEnemy(Enemy * obj){ /* does nothing */ }
Object * MugenStage::findObject(int id){ 
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
        Object * object = *it;
        if (object->getObjectId() == id){
            return object;
        }
    }
    return NULL;
}
// These should be the same, but we'll see, mugen has some funny parameters
int MugenStage::getMaximumZ(){ return zoffset; }
int MugenStage::getMinimumZ(){ return zoffset; }
void MugenStage::drawMiniMaps( bool b ){ /* Not likely */ }
bool MugenStage::shouldDrawMiniMaps(){ return false; }
void MugenStage::killAllHumans( Object * player ){ 
    for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		Object * o = *it;
		o->takeDamage(*this, NULL, 999999 );
    }
}

void MugenStage::addMessage(Network::Message m, Network::Socket from, Network::Socket to){
}

Network::Message MugenStage::createBangMessage( int x, int y, int z ){
    Network::Message m;
    return m;
}
        
bool MugenStage::isPaused(){
    return false;
}
        
void MugenStage::pause(){
}

void MugenStage::unpause(){
}

void MugenStage::changePause(){
}

const std::string MugenStage::getStageName(const std::string &filename) throw (MugenException){
    // Lets look for our def since some people think that all file systems are case insensitive
    std::string dir = Filesystem::find("mugen/stages/");
    Global::debug(1) << dir << endl;
    string fullname = filename;
    if ( fullname.find(".def") == std::string::npos){
	fullname += ".def";
    }
    const std::string defFile = Mugen::Util::fixFileName( dir, std::string(fullname) );
    
    if (defFile.empty()){
        throw MugenException( "Cannot locate stage definition file for: " + fullname );
    }
    
    std::string filesdir = "";
    
    size_t strloc = fullname.find_last_of("/");
    if (strloc != std::string::npos){
	filesdir = fullname.substr(0, strloc);
	filesdir += "/";
    }
    
    Global::debug(1) << "Got subdir: " << filesdir << endl;
    
    Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Def::main(defFile));
    return parsed.findSection("info")->findAttribute("name")->valueAsString();
        
    throw MugenException( "Cannot locate stage definition file for: " + fullname );
    return "";
}

void MugenStage::cleanup(){
    if (loaded){
	if (background){
	    delete background;
	    background = 0;
	}
	
	if (board){
	    delete board;
	    board = 0;
	}
	
	if (console){
	    delete console;
	    console = 0;
	}

        for (Mugen::SpriteMap::iterator it1 = effects.begin(); it1 != effects.end(); it1++){
            for (Mugen::GroupMap::iterator it2 = (*it1).second.begin(); it2 != (*it1).second.end(); it2++){
                MugenSprite * sprite = (*it2).second;
                delete sprite;
            }
        }
        for (map<int, MugenAnimation*>::iterator it = sparks.begin(); it != sparks.end(); it++){
            MugenAnimation * animation = (*it).second;
            delete animation;
        }
    }
}

bool MugenStage::isaPlayer( Object * o ){
    for ( vector< Object * >::iterator it = players.begin(); it != players.end(); it++ ){
        if ( (*it) == o ){
            return true;
        }
    }
    return false;
}


void MugenStage::updatePlayer( Object *o ){
    // Z/Y offset
    if (zoffsetlink == DEFAULT_BACKGROUND_ID){
	o->setZ( zoffset + abs(boundhigh) );
    } else {
	o->setZ( zoffset );
    }
    
    // Move X and Camera
    const double px = o->getX();
    const double py = o->getY();
    const double pdiffx = px - playerInfo[o].oldx;
    const double screenLeft = abs(boundleft) + camerax;
    const double screenRight = abs(boundleft) + camerax + DEFAULT_WIDTH;
    const double leftTension = screenLeft + tension;
    const double rightTension = screenRight - tension;
    // Check leftbound rightbound
    if (px <= (abs(boundleft) + boundleft)){ 
	o->setX(abs(boundleft) + boundleft );
	playerInfo[o].oldx = px;
    } else if (px >= (DEFAULT_WIDTH + abs(boundleft) + boundright)){
	o->setX(DEFAULT_WIDTH + abs(boundleft) + boundright );
	playerInfo[o].oldx = px;
    } 
    
    // Check if in tension
    if (px <= leftTension){
	if (!playerInfo[o].leftTension){
	    playerInfo[o].leftTension = true;
	    inleft++;
	}
    } else if (px >= rightTension){
	if (!playerInfo[o].rightTension){
	    playerInfo[o].rightTension = true;
	    inright++;
	}
    } 
    
    // Release tension
    if (px > leftTension){
	if (playerInfo[o].leftTension){
	    playerInfo[o].leftTension = false;
	    inleft--;
	}
    }
    // Release tension
    if (px < rightTension){
	if (playerInfo[o].rightTension){
	    playerInfo[o].rightTension = false;
	    inright--;
	}
    }
    
    // Check Screen sides
    if (px <= screenLeft){
	o->setX(screenLeft);
	playerInfo[o].oldx = px;
	if (!playerInfo[o].leftSide){
	    playerInfo[o].leftSide = true;
	    onLeftSide++;
	}
    } else if (px >= screenRight){
	o->setX(screenRight);
	playerInfo[o].oldx = px;
	if (!playerInfo[o].rightSide){
	    playerInfo[o].rightSide = true;
	    onRightSide++;
	}
    } 
    // Release side
    if (px > screenLeft){
	if (playerInfo[o].leftSide){
	    playerInfo[o].leftSide = false;
	    onLeftSide--;
	}
    }
    // Release side
    if (px < screenRight){
	if (playerInfo[o].rightSide){
	    playerInfo[o].rightSide = false;
	    onRightSide--;
	}
    }
    
    //Global::debug(0) << "Left Tension: " << inleft << " | Right Tension: "<< inright << endl;
    //Global::debug(0) << "Left Screen Edge: " << onLeftSide << " | Right Screen Edge: "<< onRightSide << endl;
    
    if (playerInfo[o].leftTension){
	if (pdiffx < 0){
	    if (!onRightSide){
		moveCamera(pdiffx,0);
	    }
	} else if (pdiffx > 0){
	    if (inright){
		moveCamera(pdiffx,0);
	    }
	}
    } else if (playerInfo[o].rightTension){
	if (pdiffx > 0){
	    if(!onLeftSide){
		moveCamera(pdiffx,0);
	    }
	} else if (pdiffx < 0){
	    if(inleft){
		moveCamera(pdiffx,0);
	    }
	}
    }
    
    // Vertical movement of camera
    if (playerInfo[o].oldy != py){
	if (verticalfollow > 0){
	    const double pdiffy = playerInfo[o].oldy - py;
	    if (py > floortension){
		if (!playerInfo[o].above){
		    playerInfo[o].above = true;
		    inabove++;
		}
	    } else if ( playerInfo[o].above){
		playerInfo[o].above = false;
		inabove--;
	    }
	    if (playerInfo[o].above && pdiffy < 0){
		moveCamera( 0, verticalfollow * -3.2 );
	    } else if (playerInfo[o].above && pdiffy > 0){
		moveCamera( 0, verticalfollow * 3.2 );
	    }
	}
    }
    //Global::debug(1) << "Our players Y: " << py << " | Above: "<< playerInfo[o].above << " | total inabove: " << inabove << endl;
}

void MugenStage::initializeName(){
    try{
	std::string str = this->location;
	// Lets look for our def since some people think that all file systems are case insensitive
	baseDir = Filesystem::find("mugen/stages/");
	Global::debug(1) << baseDir << endl;
	if (str.find(".def")==std::string::npos){
	    str+=".def";
	}
	// Get correct directory
	baseDir = Mugen::Util::getFileDir(baseDir + str);
	str = Mugen::Util::stripDir(str);
	const std::string ourDefFile = Mugen::Util::getCorrectFileLocation(baseDir, str);
	// Set name of map
	name = Mugen::Util::probeDef(ourDefFile, "info", "name");
    } catch (const MugenException &ex){
	Global::debug(1) << "Couldn't find the name of the map!" << endl;
	Global::debug(1) << "Error was: " << ex.getReason() << endl;
    }
}
