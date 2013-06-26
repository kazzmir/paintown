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

#include "stage.h"

#include "util/init.h"
#include "state.h"
#include "world.h"

#include "util/system.h"
#include "util/events.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/graphics/bitmap.h"
// #include "util/console.h"
/*
#include "object/animation.h"
#include "object/object.h"
// #include "object/character.h"
#include "object/object_attack.h"
// #include "object/player.h"
*/
#include "globals.h"
#include "util/debug.h"
#include "factory/font_render.h"
#include "ast/all.h"
#include "util/timedifference.h"
#include "character.h"
#include "helper.h"

#include "parse-cache.h"
#include "parser/all.h"

#include "animation.h"
#include "background.h"
#include "config.h"
#include "effect.h"
#include "item.h"
#include "item-content.h"
#include "section.h"
#include "projectile.h"
#include "sound.h"
#include "reader.h"
#include "sprite.h"
#include "util.h"
#include "characterhud.h"
#include "common.h"

#include "font.h"

using namespace std;

namespace PaintownUtil = ::Util;

// Some static variables
static const int CONTROLLER_VALUE_NOT_SET = -999999;
static const int DEFAULT_BACKGROUND_ID = -9999;
static const int DEFAULT_OBJECT_OFFSET = 160;
static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;
static const double DEFAULT_JUMP_VELOCITY = 7.2;
// static const int CONSOLE_SIZE = 95;
static const double DEFAULT_X_JUMP_VELOCITY = 2.2;

namespace Mugen{

}

static bool centerCollision(Mugen::Object *p1, Mugen::Object *p2){
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
        
/*
Mugen::StageStateData::StageStateData():
quake_time(0),
cycles(0),
inleft(0),
inright(0),
onLeftSide(0),
onRightSide(0),
inabove(0),
camerax(0),
cameray(0),
ticker(0){
}
*/

Mugen::Stage::Stage(const Filesystem::AbsolutePath & location):
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
shadowColor(Graphics::makeColor(0,0,0)),
shadowYscale(0.4),
shadowFadeRangeHigh(0),
shadowFadeRangeMid(0),
reflectionIntensity(0),
musicVolume(0),
//sffFile(""),
//debugbg(false),
// board(0),
xaxis(0),
yaxis(0),
stageStart(false),
totalRounds(3),
round(1),
totalTime(99),
time(99),
p1points(0),
p2points(0),
// console(new Console::Console(CONSOLE_SIZE)),
debugMode(false),
loaded(false),
gameHUD(NULL),
gameOver(false),
gameRate(1),
objectId(0),
replay(false){
}

#if 0
Mugen::Stage::Mugen::Stage( const char * location ):
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
musicVolume(0),
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
loaded(false),
gameHUD(NULL),
gameOver(false),
gameRate(1),
cycles(0){
}
#endif

Mugen::Stage::~Stage(){
    cleanup();
    if (gameHUD){
        delete gameHUD;
    }
}

/* fix */
void Mugen::Stage::loadSectionCamera(Ast::Section * section){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "startx"){
                (*simple).view() >> startx;
            } else if (*simple == "starty"){
                simple->view() >> starty;
            } else if (*simple == "boundleft"){
                simple->view() >> boundleft;
            } else if (*simple == "boundright"){
                simple->view() >> boundright;
            } else if (*simple == "boundhigh"){
                simple->view() >> boundhigh;
            } else if (*simple == "boundlow"){
                // This is always 0 so don't grab it
                // *content->getNext() >> boundlow;
            } else if (*simple == "verticalfollow"){
                simple->view() >> verticalfollow;
                if (verticalfollow > 1){
                    verticalfollow = 1;
                } else if (verticalfollow < 0){
                    verticalfollow = 0;
                }
            } else if (*simple == "floortension"){
                simple->view() >> floortension;
            } else if (*simple == "tension"){
                simple->view() >> tension;
            } else {
                throw MugenException( "Unhandled option in Camera Section: " + simple->toString(), __FILE__, __LINE__);
            }
        }
    }
}

void Mugen::Stage::loadSectionInfo(Ast::Section * section){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "name"){
                /* use setName() here */
                simple->view() >> name;
                Global::debug(1) << "Read name '" << name << "'" << endl;
            } else if (*simple == "author"){
                // Seems to be that some people think that author belongs in background defs
                std::string temp;
                simple->view() >> temp;
                Global::debug(1) << "Made by this guy: '" << temp << "'" << endl;
            } else {
                Global::debug(0) << "Ignoring option in Info Section: " + simple->toString() << std::endl;
            }
        }
    }
}

void Mugen::Stage::loadSectionPlayerInfo(Ast::Section * section){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "p1startx"){
                simple->view() >> p1startx;
            } else if (*simple == "p1starty"){
                simple->view() >> p1starty;
            } else if (*simple == "p1startz"){
                simple->view() >> p1startz;
            } else if (*simple == "p1facing"){
                simple->view() >> p1facing;
            } else if (*simple == "p2startx"){
                simple->view() >> p2startx;
            } else if (*simple == "p2starty"){
                simple->view() >> p2starty;
            } else if (*simple == "p2startz"){
                simple->view() >> p2startz;
            } else if (*simple == "p2facing"){
                simple->view() >> p2facing;
            } else if (*simple == "leftbound"){
                simple->view() >> leftbound;
            } else if (*simple == "rightbound"){
                simple->view() >> rightbound;
            } else if (*simple == "topbound"){
                simple->view() >> topbound;
            } else if (*simple == "botbound"){
                simple->view() >> botbound;
            } else {
                throw MugenException("Unhandled option in PlayerInfo Section: " + simple->toString(), __FILE__, __LINE__);
            }
        }
    }
}

void Mugen::Stage::loadSectionBound(Ast::Section * section){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "screenleft"){
                simple->view() >> screenleft;
            } else if (*simple == "screenright"){
                simple->view() >> screenright;
            } else {
                throw MugenException("Unhandled option in Bound Section: " + simple->toString(), __FILE__, __LINE__);
            }
        }
    }
}

void Mugen::Stage::loadSectionStageInfo(Ast::Section * section){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "zoffset" ){
                simple->view() >> zoffset;
            } else if (*simple == "zoffsetlink" ){
                simple->view() >> zoffsetlink;
            } else if (*simple == "autoturn"){
                simple->view() >> autoturn;
            } else if (*simple == "resetbg"){
                simple->view() >> resetBG;
            } else {
                throw MugenException("Unhandled option in StageInfo Section: " + simple->toString(), __FILE__, __LINE__);
            }
        }
    }
}

void Mugen::Stage::loadSectionShadow(Ast::Section * section, cymk_holder & shadow){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "intensity"){
                simple->view() >> shadow.k;
            // *content->getNext() >> shadowIntensity;
            // shadow.k = shadowIntensity;
            } else if (*simple == "reflect"){
                simple->view() >> reflect;
            } else if (*simple == "color"){
                try{
                    Ast::View view = simple->view();
                    view >> shadow.c;
                    view >> shadow.y;
                    view >> shadow.m;
                } catch (const Ast::Exception & fail){
                }
            } else if (*simple == "yscale"){
                simple->view() >> shadowYscale;
            } else if (*simple == "fade.range"){
                simple->view() >> shadowFadeRangeHigh >> shadowFadeRangeMid;
            } else {
                throw MugenException("Unhandled option in Shadow Section: " + simple->toString(), __FILE__, __LINE__);
            }
        }
    }
}

void Mugen::Stage::loadSectionReflection(Ast::Section * section){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "intensity"){
                simple->view() >> reflectionIntensity;
            } else {
                throw MugenException("Unhandled option in Reflection Section: " + simple->toString(), __FILE__, __LINE__);
            }
        }
    }
}

void Mugen::Stage::loadSectionMusic(Ast::Section * section){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "bgmusic"){
		try {
		    simple->view() >> music;
		} catch (const MugenException &ex){
		}
            } else if (*simple == "bgvolume"){
		try {
		    simple->view() >> musicVolume;
		} catch (const MugenException &ex){
		}
	    } else {
                throw MugenException("Unhandled option in Music Section: " + simple->toString(), __FILE__, __LINE__);
            }
        }
    }
}

int Mugen::Stage::getFloor() const {
    return currentZOffset();
}

int Mugen::Stage::currentZOffset() const {
    if (zoffsetlink != DEFAULT_BACKGROUND_ID){
	// Link zoffset to id
        vector<Mugen::BackgroundElement *> elements = background->getIDList(zoffsetlink);
        if (elements.size() != 0){
            Mugen::BackgroundElement * element = elements[0];
            return (int) element->getCurrentY();
        }
    }

    return zoffset;
}

static Filesystem::AbsolutePath getMotifFile(const string & path){
    return Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath(path));
}

void Mugen::Stage::load(){
    if (loaded){
        return;
    }
#if 0
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
#endif
    baseDir = location.getDirectory();
    const Filesystem::AbsolutePath ourDefFile = location;
    
    std::string filesdir = "";
    
    /* will this screw up on windows??
     * FIXME: redo this code to work with Path objects instead of their strings
     */
    size_t strloc = location.path().find_last_of("/");
    if (strloc != std::string::npos){
	filesdir = location.path().substr(0, strloc);
	filesdir += "/";
    }
    
    Global::debug(1) << "Got subdir: " << filesdir << endl;

    TimeDifference diff;
    diff.startTime();
    AstRef parsed(Mugen::Util::parseDef(ourDefFile));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + ourDefFile.path() + " in" + diff.printTime("") << endl;
    // list<Ast::Section*> * sections = (list<Ast::Section*>*) Mugen::Def::main(ourDefFile);

    struct cymk_holder shadow;
    
    /* Extract info for our first section of our stage */
    for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
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
        /*
	} else if (matchRegex(head, ".*bgdef.*")){
	    // Background management
            vector<Ast::Section*> backgroundStuff = Mugen::Util::collectBackgroundStuff(section_it, parsed.getSections()->end());
	    MugenBackgroundManager *manager = new MugenBackgroundManager(baseDir, backgroundStuff, ticker, 0);
	    background = manager;
	    Global::debug(1) << "Got background: '" << manager->getName() << "'" << endl;
	}*/
        } else if (head == "music" ){
           loadSectionMusic(section);
        } else {
            // throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
        }
    }

    /* backgrounds should always use `BG' as their name */
    /* FIXME: pass in the parsed data instead of making the background reparse it */
    background = new Mugen::Background(ourDefFile, "BG");
    
    // Setup board our worksurface to the proper size of the entire stage 320x240 :P
    // Global::debug(1) << "Creating level size of Width: " << abs(boundleft) + boundright << " and Height: " << abs(boundhigh) + boundlow << endl;
    //board = new Bitmap( DEFAULT_WIDTH, DEFAULT_HEIGHT );
    // Nope we need it to be the size of the entire board... we then pan the blit so our characters will stay put without fiddling with their x coordinates
    // board = new Bitmap( abs(boundleft) + boundright + DEFAULT_WIDTH, abs(boundhigh) + boundlow + DEFAULT_HEIGHT);
    // board = new Graphics::Bitmap(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    getStateData().camerax = startx;
    getStateData().cameray = starty;
    // xaxis = (abs(boundleft) + boundright + DEFAULT_WIDTH)/2;//abs(boundleft);
    // yaxis = abs(boundhigh);
    /* FIXME: do we need xaxis and yaxis anymore? I don't think so */
    xaxis = DEFAULT_WIDTH / 2;
    yaxis = 0;
    
    // Preload background
    // background->preload( startx, starty );
    
    // zoffsetlink
    /*
    if (zoffsetlink != DEFAULT_BACKGROUND_ID){
	// Link zoffset to id
        vector<Mugen::BackgroundElement *> elements = background->getIDList(zoffsetlink);
        if (elements.size() != 0){
            Mugen::BackgroundElement * element = elements[0];
            zoffset = element->getCurrentY();
        }
    }
    */

    int r, g, b;
    Graphics::Bitmap::cymkToRGB(shadow.c, shadow.y, shadow.m, shadow.k, &r, &g, &b);
    Global::debug(1) << "Shadow c/y/m/k " << shadow.c << " " << shadow.y << " " << shadow.m << " " << shadow.k << " r/g/b " << r << " " << g << " " << b << endl;
    shadowColor = Graphics::makeColor(r, g, b);

    /* shadowIntensity is used as the alpha value. its some combination of the
     * cymk components but I'm not sure what it is. This is relatively close
     * but its definately not 100% accurate.
     */
    shadowIntensity = PaintownUtil::min((shadow.c + shadow.y + shadow.m + shadow.k * 2) / 3, 255);
    Global::debug(1) << "Shadow intensity " << shadowIntensity << endl;

    // Mugen::Util::readSprites(Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath("fightfx.sff")), Filesystem::AbsolutePath(), effects);
    Mugen::Util::readSprites(getMotifFile("fightfx.sff"), Filesystem::AbsolutePath(), effects, true);
    // sparks = Mugen::Util::loadAnimations(Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath("fightfx.air")), effects);
    sparks = Mugen::Util::loadAnimations(getMotifFile("fightfx.air"), effects, true);

    // Mugen::Util::readSounds(Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath("common.snd")), sounds);
    Mugen::Util::readSounds(getMotifFile("common.snd"), sounds);

    /*
    for (Mugen::SpriteMap::iterator it = effects.begin(); it != effects.end(); it++){
        Global::debug(-1) << "Effect group " << (*it).first << endl;
    }

    spark = new Animation();
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
    
    /*
    // Console stuff
    console->setTextHeight(10);
    console->setTextWidth(10); 
    */
    
    // *FIXME Use current motif instead of direct file access
    try{
        gameHUD = new Mugen::GameInfo(Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath("fight.def")));
    } catch (const MugenException &e){
        Global::debug(0) << "Problem loading HUD. Reason: " << e.getReason() << endl;
        /* FIXME: throw an exception here?? */
    }

    // Stage is loaded
    loaded = true;
}

void Mugen::Stage::setCamera( const double x, const double y ){ 
    getStateData().camerax = x;
    getStateData().cameray = y; 
    // Camera boundaries
    if (getStateData().camerax < boundleft){
        getStateData().camerax = boundleft;
    } else if (getStateData().camerax > boundright){
        getStateData().camerax = boundright;
    }

    if (getStateData().cameray < boundhigh){
        getStateData().cameray = boundhigh;
    } else if (getStateData().cameray > boundlow){
        getStateData().cameray = boundlow;
    }
}
void Mugen::Stage::moveCamera(const double x, const double y){ 
    getStateData().camerax += x;
    getStateData().cameray += y; 

    // Camera boundaries
    if (getStateData().camerax < boundleft){
        getStateData().camerax = boundleft;
    } else if (getStateData().camerax > boundright){
        getStateData().camerax = boundright;
    }

    if (getStateData().cameray < boundhigh){
        getStateData().cameray = boundhigh;
    } else if (getStateData().cameray > boundlow){
        getStateData().cameray = boundlow;
    }
}

static bool anyCollisions(const vector<Mugen::Area> & boxes1, int x1, int y1, const vector<Mugen::Area> & boxes2, int x2, int y2){

    for (vector<Mugen::Area>::const_iterator attack_i = boxes1.begin(); attack_i != boxes1.end(); attack_i++){
        for (vector<Mugen::Area>::const_iterator defense_i = boxes2.begin(); defense_i != boxes2.end(); defense_i++){
            const Mugen::Area & attack = *attack_i;
            const Mugen::Area & defense = *defense_i;
            if (attack.collision(x1, y1, defense, x2, y2)){
                return true;
            }
        }
    }

    return false;

}

static bool anyBlocking(const vector<Mugen::Area> & boxes1, int x1, int y1, int attackDist, const vector<Mugen::Area> & boxes2, int x2, int y2){
    for (vector<Mugen::Area>::const_iterator attack_i = boxes1.begin(); attack_i != boxes1.end(); attack_i++){
        for (vector<Mugen::Area>::const_iterator defense_i = boxes2.begin(); defense_i != boxes2.end(); defense_i++){
            const Mugen::Area & attack = *attack_i;
            Mugen::Area defense = *defense_i;
	    defense.x1 -= attackDist;
	    defense.x2 += attackDist;
            if (attack.collision(x1, y1, defense, x2, y2)){
                return true;
            }
        }
    }

    return false;
}

bool Mugen::Stage::doBlockingDetection(Mugen::Character * obj1, Mugen::Character * obj2){
    // return anyBlocking(obj1->getAttackBoxes(), (int) obj1->getX(), (int) obj1->getY(), obj1->getAttackDistance(), obj2->getDefenseBoxes(), (int) obj2->getX(), (int) obj2->getY());
    return anyBlocking(obj1->getAttackBoxes(), (int) obj1->getX(), (int) obj1->getY(), 0, obj2->getDefenseBoxes(), (int) obj2->getX(), (int) obj2->getY());
}

bool Mugen::Stage::doCollisionDetection(Mugen::Character * obj1, Mugen::Character * obj2){
    return anyCollisions(obj1->getAttackBoxes(), (int) obj1->getX(), (int) obj1->getY(), obj2->getDefenseBoxes(), (int) obj2->getX(), (int) obj2->getY());
}

bool Mugen::Stage::doReversalDetection(Mugen::Character * obj1, Mugen::Character * obj2){
    return anyCollisions(obj1->getAttackBoxes(), (int) obj1->getX(), (int) obj1->getY(), obj2->getAttackBoxes(), (int) obj2->getX(), (int) obj2->getY());
}
    
bool Mugen::Stage::replayEnabled() const {
    return replay;
}

void Mugen::Stage::setReplay(bool what){
    this->replay = what;
}

PaintownUtil::ReferenceCount<Mugen::Animation> Mugen::Stage::getFightAnimation(int id){
    if (sparks[id] == 0){
        ostringstream out;
        out << "No fightfx animation for " << id;
        throw MugenException(out.str(), __FILE__, __LINE__);
    }

    return sparks[id];
}

void Mugen::Stage::addSpark(int x, int y, int sparkNumber, bool own, Character * owner){
    PaintownUtil::ReferenceCount<Mugen::Animation> sprite;
    if (own && owner != NULL){
        sprite = owner->getAnimation(sparkNumber);
    } else {
        sprite = sparks[sparkNumber];
    }
    if (sprite == NULL){
        /*
        ostringstream out;
        out << "No spark animation for " << sparkNumber;
        throw MugenException(out.str());
        */
        Global::debug(0) << "No spark animation for " << sparkNumber << endl;
        return;
    }
    /* FIXME: sprite priority */
    Mugen::Spark * spark = new Mugen::Spark(x, y, 0, PaintownUtil::ReferenceCount<Mugen::Animation>(sprite->copy()));
    showSparks.push_back(spark);
}

void Mugen::Stage::addSpark(int x, int y, const ResourceEffect & resource, const ResourceEffect & default_, Character * owner){
    int spark = resource.group;
    bool own = resource.own;
    if (spark == -1){
        spark = default_.group;
        own = default_.own;
    }

    addSpark(x, y, spark, own, owner);
}

/*
void Mugen::Stage::addSpark(int x, int y, const PaintownUtil::ReferenceCount<Animation> & animation){
    if (animation != NULL){
        / * FIXME: sprite priority * /
        Mugen::Spark * spark = new Mugen::Spark(x, y, 0, PaintownUtil::ReferenceCount<Animation>(animation->copy()));
        showSparks.push_back(spark);
    }
}
*/

void Mugen::Stage::playSound(Character * owner, int group, int item, bool own){
    PaintownUtil::ReferenceCount<Mugen::Sound> sound = PaintownUtil::ReferenceCount<Mugen::Sound>(NULL);
    if (own){
        sound = owner->getSound(group, item);
    } else {
        sound = owner->getCommonSound(group, item);
    }

    if (sound != NULL){
        sound->play();
    }
}

void Mugen::Stage::doProjectileCollision(Projectile * projectile, Character * mugen){
    if (anyCollisions(mugen->getDefenseBoxes(), (int) mugen->getX(), (int) mugen->getRY(),
                      projectile->getAttackBoxes(), (int) projectile->getX(), (int) projectile->getY())){
        projectile->doCollision(mugen, *this);

        Character * owner = getCharacter(projectile->getOwner());

        bool block = mugen->isBlocking(projectile->getHitDefinition());

        if (block){
            /* add guard spark and play guard sound */
            addSpark((int)(projectile->getHitDefinition().sparkPosition.x + projectile->getX()),
                     (int)(projectile->getHitDefinition().sparkPosition.y + projectile->getY()),
                     projectile->getHitDefinition().guardSpark,
                     owner->getDefaultGuardSpark(),
                     getCharacter(projectile->getOwner()));

            playSound(owner, projectile->getHitDefinition().guardHitSound.group, projectile->getHitDefinition().guardHitSound.item, projectile->getHitDefinition().guardHitSound.own);
            mugen->guarded(*this, owner, projectile->getHitDefinition());
            projectile->wasGuarded(mugen, *this);
            /* TODO: Should we call didHitGuarded on owner? */
        } else {
            addSpark((int)(projectile->getHitDefinition().sparkPosition.x + projectile->getX()),
                     (int)(projectile->getHitDefinition().sparkPosition.y + projectile->getY()),
                     projectile->getHitDefinition().spark,
                     owner->getDefaultSpark(),
                     getCharacter(projectile->getOwner()));

            playSound(owner, projectile->getHitDefinition().hitSound.group, projectile->getHitDefinition().hitSound.item, projectile->getHitDefinition().hitSound.own);
            mugen->wasHit(*this, owner, projectile->getHitDefinition());
            /* TODO: Should we call didHit on owner? */
        }
    }
}

void Mugen::Stage::doProjectileToProjectileCollision(Projectile * mine, Projectile * his){
    if (anyCollisions(mine->getDefenseBoxes(), (int) mine->getX(), (int) mine->getY(),
                      his->getAttackBoxes(), (int) his->getX(), (int) his->getY())){
        if (mine->getPriority() > his->getPriority()){
            his->canceled(*this, mine);
        } else if (his->getPriority() > mine->getPriority()){
            mine->canceled(*this, his);
        } else {
            /* Both cancel if priorities are the same */
            mine->canceled(*this, his);
            his->canceled(*this, mine);
        }
    }
}

/* for helpers and players */
void Mugen::Stage::physics(Character * mugen){
    // Z/Y offset
    mugen->setZ(currentZOffset());

    /* ignore physics while the player is paused */
    if (mugen->isPaused()){
        return;
    }

    if (getStateData().pause.time > 0){
        if (getStateData().pause.moveTime == 0){
            return;
        }

        if (getCharacter(getStateData().pause.who) != mugen){
            return;
        }
    }

    mugen->doMovement(*this);

    if (mugen->getCurrentPhysics() == Mugen::Physics::Stand ||
        mugen->getCurrentPhysics() == Mugen::Physics::Crouch){
        // mugen->setY(0);
        /* friction */
        if (mugen->getY() == 0){
            mugen->setXVelocity(mugen->getXVelocity() * mugen->getGroundFriction());
            if (mugen->getMoveType() == Mugen::Move::Hit && 
                mugen->getXVelocity() < 0 &&
                getTicks() % 5 == 0){
                createDust((int) mugen->getX(), (int) mugen->getRY());
            }
        }
    } else if (mugen->getCurrentPhysics() == Mugen::Physics::Air){
        /* gravity */
        if (mugen->getY() < 0){
            mugen->setYVelocity(mugen->getYVelocity() + mugen->getGravity());
        }
    }

    if (mugen->isAttacking() && mugen->getHit().alive){

        for (vector<Mugen::Character*>::iterator enem = objects.begin(); enem != objects.end(); ++enem){
            Mugen::Character * enemy = *enem;
            if (enemy->getAlliance() != mugen->getAlliance() && enemy->canBeHit((Character*) mugen)){
		// Check attack distance to make sure we begin block at the correct distance
                /*
		if (doBlockingDetection(mugen, enemy) && enemy->isBlocking(*mugen->getHit())){
		    enemy->guarded(mugen);
		}
                */

                /* Checks attack boxes vs defense boxes */
                bool collision = doCollisionDetection(mugen, enemy);

                /* blocking collision extends a little further than a a normal collision */
                bool blockingCollision = doBlockingDetection(mugen, enemy);

                /* Checks attack boxes vs attack boxes */
                bool reversalCollision = doReversalDetection(mugen, enemy);

                /* If enemy is doing a reversal then that takes precedence
                 * over everything else here, I think.
                 */
                if (enemy->canReverse(mugen) && reversalCollision){

                    /* Add the spark at mugen->hitdef's sparkxy +
                     *  enemy->reversal­>sparkxy as an offset.
                     */

                    addSpark((int)(mugen->getHit().sparkPosition.x + enemy->getReversal().sparkX),
                             (int)(mugen->getHit().sparkPosition.y + enemy->getReversal().sparkY),
                                 enemy->getReversal().spark,
                                 enemy->getReversal().spark,
                                 enemy);

                    playSound(enemy, enemy->getReversal().hitSound.group, enemy->getReversal().hitSound.item, enemy->getReversal().hitSound.own);

                    enemy->didReverse(*this, mugen, enemy->getReversal());
                    mugen->wasReversed(*this, enemy, enemy->getReversal());
                } else if ((collision || blockingCollision) &&
                           enemy->isBlocking(mugen->getHit()) &&
                           enemy->compatibleHitFlag(mugen->getHit().guardFlag)){
                    /* guarding */

                    /* FIXME: why do we differentiate between blocking collision and a
                     * regular collision?
                     */
                    if (collision){
                        addSpark((int)(mugen->getHit().sparkPosition.x + enemy->getX()),
                                 (int)(mugen->getHit().sparkPosition.y + enemy->getRY()),
                                 mugen->getHit().guardSpark, mugen->getDefaultGuardSpark(),
                                 mugen);

                        playSound(mugen, mugen->getHit().guardHitSound.group, mugen->getHit().guardHitSound.item, mugen->getHit().guardHitSound.own);
                    }
                    mugen->didHitGuarded(enemy, *this);
                    enemy->guarded(*this, mugen, mugen->getHit());
                } else if (collision && enemy->compatibleHitFlag(mugen->getHit().hitFlag)){
                    addSpark((int)(mugen->getHit().sparkPosition.x + enemy->getX()),
                             (int)(mugen->getHit().sparkPosition.y + enemy->getRY()),
                             mugen->getHit().spark, mugen->getDefaultSpark(),
                             mugen);

                    playSound(mugen, mugen->getHit().hitSound.group, mugen->getHit().hitSound.item, mugen->getHit().hitSound.own);

                    /* order matters here, the guy attacking needs to know that
                     * he hit enemy so the guy can update his combo stuff.
                     */
                    mugen->didHit(enemy, *this);
                    enemy->wasHit(*this, mugen, mugen->getHit());
                }
            }
        }
    }

    for (vector<Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); it++){
        Projectile * projectile = *it;
        if (projectile->getOwner() != mugen->getId() && projectile->canCollide()){
            doProjectileCollision(projectile, mugen);
        }

        for (vector<Projectile*>::iterator it2 = projectiles.begin(); it2 != projectiles.end(); it2++){
            Projectile * other = *it2;
            /* I'm assuming that projectiles fired from the same character cant cancel each other */
            /* FIXME: should we test to see if both projectiles can collide or will they
             * cancel each other even if one has its miss time active?
             */
            if (other != projectile && other->getOwner() != projectile->getOwner()){
                doProjectileToProjectileCollision(projectile, other);
            }
        }
    }

    /* TODO: projectile collisions with other projectiles */

    // Check collisions
    for (vector<Mugen::Character*>::iterator enem = objects.begin(); enem != objects.end(); ++enem){
        Mugen::Character *enemy = *enem;
        if (mugen->getAlliance() != enemy->getAlliance()){
            // Do stuff for players
            if (isaPlayer(enemy)){
                // He collides with another push him away
                Mugen::Character * mplayer = mugen;
                Mugen::Character * menemy = enemy;
                /* TODO: make this cleaner
                   FIXME: we need to check collisions in the air and push as well */
                // NOTE: if Push Check is disabled do not do this
                if (mplayer->isPushable() && menemy->isPushable()){
                    while (anyCollisions(mplayer->getDefenseBoxes(), (int) mplayer->getX(), (int) mplayer->getY(),
                                         menemy->getDefenseBoxes(), (int) menemy->getX(), (int) menemy->getY()) &&
                           centerCollision(mplayer, menemy) &&
                           (fabs(enemy->getX() - mugen->getX()) < enemy->getWidth() + mugen->getWidth()) &&
                           mplayer->getY() < enemy->getHeight()){
                        
                        bool enemyMoved = false;
                        /* use move*Force to override pos freeze */
                        
                        /* NOTE centers used to check for the end of the screen.
                        *      A character shouldn't be able to jump over another if he's bound to either end of the stage 
                        */
                        const int playerCenter = mplayer->getX()+(mplayer->getWidth()/2);
                        const int enemyCenter = enemy->getX()+(enemy->getWidth()/2);
                        if (enemy->getFacing() == FacingLeft &&
                            enemy->getX() == maximumRight(enemy) &&
                            playerCenter >= enemyCenter){
                            // Right hand side of stage
                            mplayer->moveLeftForce(1.5);
                            enemyMoved = true;
                        } else if (enemy->getFacing() == FacingRight &&
                                   enemy->getX() == maximumLeft(enemy) &&
                                   playerCenter <= enemyCenter){
                            // Left hand side of stage
                            mplayer->moveRightForce(1.5);
                            enemyMoved = true;
                        } else if (enemy->getX() >= mplayer->getX()){
                            // Move character
                            if (enemy->getX() >= maximumRight(enemy)){
                                mplayer->moveLeftForce(0.5);
                                enemyMoved = true;
                            } else {
                                enemy->moveRightForce(0.5);
                                enemyMoved = true;
                            }
                        } else if (enemy->getX() < mplayer->getX()){
                            // Move character
                            if (enemy->getX() <= maximumLeft(enemy)){
                                mplayer->moveRightForce(0.5);
                                enemyMoved = true;
                            } else {
                                enemy->moveLeftForce(0.5);
                                enemyMoved = true;
                            }
                        }
                        
                        /* NOTE: player should not be able to fall through the other player */
                        if (mplayer->getY() < 0 && !enemyMoved){
                            const int bottom = mplayer->getY() + mplayer->getHeight();
                            if (bottom >= enemy->getY()){
                                mplayer->setY(enemy->getY()-mplayer->getHeight());
                            }
                        }
                    }
                }
                // autoturn need to do turning actions
                if (autoturn){
                    if (isaPlayer(mugen)){
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
        }
    }
}

vector<Mugen::Character*> Mugen::Stage::getOpponents(Mugen::Object * who){
    vector<Mugen::Character*> out;
    for (vector<Mugen::Character*>::iterator it = objects.begin(); it != objects.end(); ++it){
        Mugen::Character * player = *it;
        if (isaPlayer(player) && player->getAlliance() != who->getAlliance()){
            out.push_back(player);
        }
    }

    return out;
}

void Mugen::Stage::unbind(Mugen::Character * what){
    for (vector<Mugen::Character*>::iterator it = objects.begin(); it != objects.end(); ++it){
        Mugen::Character * guy = *it;
        guy->unbind(what);
    }
}

void Mugen::Stage::updateZoom(){
    /* Zoom in, then wait for the zoom time to expire, then zoom back out */
    if (getStateData().zoom.enabled){

        /* If the original owner got hit and we should remove then disable zoom.
         * We can't just set zoom.enabled to false immediately because the effect
         * would look goofy, instead we have to zoom back out. So let the zoom in
         * finish and then immediately zoom back out. We can do this by setting
         * the time that the zoom is focused to 0.
         */
        if (getStateData().zoom.removeOnGetHit && exists(getStateData().zoom.owner)){
            if (getCharacter(getStateData().zoom.owner)->getWasHitCount() > getStateData().zoom.hitCount &&
                getStateData().zoom.time > 0){
                getStateData().zoom.time = 0;
            }
        }

        if (getStateData().superPause.time > 0){
            if (getStateData().zoom.superMoveTime > 0){
                getStateData().zoom.superMoveTime -= 1;
            } else {
                return;
            }
        }

        if (getStateData().pause.time > 0){
            if (getStateData().zoom.pauseMoveTime > 0){
                getStateData().zoom.pauseMoveTime -= 1;
            } else {
                return;
            }
        }

        /* Also check for pause time */

        if (getStateData().zoom.zoom == 0){
            if (getStateData().zoom.in == true){
                getStateData().zoom.time -= 1;
                if (getStateData().zoom.time <= 0){
                    getStateData().zoom.in = false;
                    getStateData().zoom.zoom = getStateData().zoom.zoomOutTime;
                }
            } else {
                getStateData().zoom.enabled = false;
            }
        } else {
            getStateData().zoom.zoom -= 1;
        }

        if (getStateData().zoom.bindTime > 0){
            getStateData().zoom.bindTime -= 1;
            if (exists(getStateData().zoom.bound)){
                getStateData().zoom.x = getStateData().zoom.deltaX + getCharacter(getStateData().zoom.bound)->getX();
                getStateData().zoom.y = getStateData().zoom.deltaY + getCharacter(getStateData().zoom.bound)->getRY();
            }
        } else {
            /* move by velocity and acceleration */
            getStateData().zoom.x += getStateData().zoom.velocityX;
            getStateData().zoom.y += getStateData().zoom.velocityY;
            getStateData().zoom.velocityX += getStateData().zoom.accelX;
            getStateData().zoom.velocityY += getStateData().zoom.accelY;
        }
    }
}
        
bool Mugen::Stage::exists(Character * who){
    /* Returns true if the pointer is in the current object list. There is a very small
     * chance that an old object could be deleted and recreated with the same address
     * so that exists is true for the old pointer when really it should have been false.
     * I doubt this will matter in practice.
     */
    for (vector<Mugen::Character*>::iterator it = objects.begin(); it != objects.end(); it++){
        if (*it == who){
            return true;
        }
    }
    return false;
}

bool Mugen::Stage::exists(CharacterId id){
    return getCharacter(id) != NULL;
}

/* A main cycle of the game */
void Mugen::Stage::runCycle(){
    updateZoom();

    getStateData().screenBound.clear();

    if (paletteEffects.time > 0){
        paletteEffects.time = 0;
        paletteEffects.counter += 1;
    }
                    
    // camera crap
    if (getStateData().quake_time > 0){
        getStateData().quake_time--;
    }

    for (vector<Mugen::Effect*>::iterator it = showSparks.begin(); it != showSparks.end(); /**/){ 
        Mugen::Effect * spark = *it;
        spark->logic();

        /* if the spark looped then kill it */
        if (spark->isDead()){
            delete spark;
            it = showSparks.erase(it);
        } else {
            it++;
        }
    }

    /* FIXME: Projectiles should not act during a pause or superpause */
    for (vector<Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); /**/){
        Projectile * projectile = *it;
        projectile->logic(*this);

        if (projectile->isDead()){
            delete projectile;
            it = projectiles.erase(it);
        } else {
            it++;
        }
    }

    // implement some stuff before we actually begin the round then start the round
    if (!stageStart){
        stageStart = true;
    }

    // Run our ticker on and on like energizer bunnies (tm)
    getStateData().ticker++;

    /*
    const double diffx = startx - camerax;
    const double diffy = starty - cameray;
    */

    if (getStateData().environmentColor.time > 0){
        getStateData().environmentColor.time -= 1;
    }

    if (getStateData().superPause.time > 0){
        getStateData().superPause.time -= 1;
        if (getStateData().superPause.time == 0){
            for (vector<Mugen::Effect*>::iterator it = showSparks.begin(); it != showSparks.end(); it++){ 
                Mugen::Effect * effect = *it;
                effect->superPauseEnd();
            }
        }
    } else {
        if (getStateData().pause.time > 0){
            getStateData().pause.time -= 1;
            if (getStateData().pause.moveTime > 0){
                getStateData().pause.moveTime -= 1;
            }
        }

        if (getStateData().pause.time <= 0 || (getStateData().pause.time > 0 && !getStateData().pause.pauseBackground)){
            background->act();
        }
       
        // Players go in here
        std::vector<Mugen::Character *> add;
        /* Do all states first */
        for (vector<Mugen::Character*>::iterator it = objects.begin(); it != objects.end(); it++){
            /* use local variables more often, iterators can be easily confused */
            Mugen::Character * player = *it;

            /* If a pause is occuring then only allow the player who started the pause
             * to move if moveTime is > 0.
             */
            if (getStateData().pause.time <= 0 || (getStateData().pause.time > 0 && getStateData().pause.moveTime > 0 && getCharacter(getStateData().pause.who) == player)){
                player->act(this);
            }
        }

        /* Then do physics/collision detection */
        for (vector<Mugen::Character*>::iterator it = objects.begin(); it != objects.end(); /**/ ){
            bool next = true;
            Mugen::Character * player = *it;
            physics(player);

            /* Debug crap put it on console */
            // *console << "Object: " << player << " x: " << player->getX() << " y: " << player->getY() << cend;

            if (isaPlayer(player)){
                // Lets check their boundaries and camera whateva
                updatePlayer(player);

                // Update old position
                playerInfo[player].oldx = player->getX();
                playerInfo[player].oldy = player->getY();

                // Non players, objects, projectiles misc
            } else if (!isaPlayer(player) && player->getHealth() <= 0){
                player->destroyed(*this);
                // unbind(player);
                delete player;
                it = objects.erase(it);
                next = false;
            }

            if (next){
                it++;
            }
        }

        /* Have to insert objects at the front of the vector because new helpers should
         * have their states executed before players. At least this is the only way
         * I can get MVC2_IronMan's intro to work properly.
         */
        objects.insert(objects.begin(), add.begin(), add.end());
        objects.insert(objects.begin(), addedObjects.begin(), addedObjects.end());
        addedObjects.clear();
    }
}

void Mugen::Stage::logic(){

    /* This must be the first thing done in this function! */
    /*
    if (observer != NULL){
        observer->beforeLogic(*this);
    }
    */

    // Console::ConsoleEnd & cend = Console::Console::endl;

    /* cycles slow the stage down, like after ko */
    getStateData().cycles += 1;
    if (getStateData().cycles >= 1 / gameRate){
        getStateData().cycles = 0;
        runCycle();
    }
    
    // Correct camera
    if ((verticalfollow > 0) && !getStateData().inabove && (getCameraY() < 0)){
        /* FIXME: where did 3.2 come from? */
	moveCamera(0, verticalfollow * 3.2);
    }
    
    // Player HUD Need to make this more ellegant than casting and passing from array
    gameHUD->act(*this, *((Mugen::Character *)players[0]),*((Mugen::Character *)players[1]));

    /* This must be the last thing done in this function! */
    /*
    if (observer != NULL){
        observer->afterLogic(*this);
    }
    */

#if 0
    if (!gameOver){
        for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); ++it){
            /* use local variables more often, iterators can be easily confused */
            Object * player = *it;

            if (isaPlayer(player)){
                if (player->getHealth() <= 0){
                    gameOver = true;
                    vector<Object*> others = getOpponents(player);
                    for (vector<Object*>::iterator it2 = others.begin(); it2 != others.end(); it2++){
                        Mugen::Character * character = (Mugen::Character*) *it2;
                        vector<string> inputs;
                        character->changeState(*this, 180, inputs);
                    }
                }
            }
        }
    }
#endif
}

class PaletteShader: public Graphics::Bitmap::Filter {
public:
    PaletteShader(int time, int addRed, int addGreen, int addBlue, int multiplyRed, int multiplyGreen, int multiplyBlue, int sinRed, int sinGreen, int sinBlue, int period, int invert, int color):
        time(time),
        addRed(addRed),
        addGreen(addGreen),
        addBlue(addBlue),
        multiplyRed(multiplyRed),
        multiplyGreen(multiplyGreen),
        multiplyBlue(multiplyBlue),
        sinRed(sinRed),
        sinGreen(sinGreen),
        sinBlue(sinBlue),
        period(period),
        invert(invert),
        color(color){
        }

    int time;
    int addRed;
    int addGreen;
    int addBlue;
    int multiplyRed;
    int multiplyGreen;
    int multiplyBlue;
    int sinRed;
    int sinGreen;
    int sinBlue;
    int period;
    int invert;
    int color;

    mutable map<Graphics::Color, Graphics::Color> cache;

    Graphics::Color doFilter(int red, int green, int blue) const {
        int newRed = red;
        int newGreen = green;
        int newBlue = blue;

        if (color < 255){
            double greyRed = (1 - 0.299) * color / 255 + 0.299;
            double greyGreen = (1 - 0.587) * color / 255 + 0.587;
            double greyBlue = (1 - 0.114) * color / 255 + 0.114;
            red = (int)(red * greyRed + 0.5 + 16);
            green = (int)(green * greyGreen + 0.5 + 16);
            blue = (int)(blue * greyBlue + 0.5 + 16);
        }

        if (invert){
            red = 255 - red;
            green = 255 - green;
            blue = 255 - blue;
        }

        if (period > 0){
            newRed = (red + addRed + sinRed * sin(2 * PaintownUtil::pi * time / period)) * multiplyRed / 256;
            newGreen = (green + addGreen + sinGreen * sin(2 * PaintownUtil::pi * time / period)) * multiplyGreen / 256;
            newBlue = (blue + addBlue + sinBlue * sin(2 * PaintownUtil::pi * time / period)) * multiplyBlue / 256;
        } else {
            newRed = (red + addRed) * multiplyRed / 256;
            newGreen = (green + addGreen) * multiplyGreen / 256;
            newBlue = (blue + addBlue) * multiplyBlue / 256;
        }

        if (newRed > 255){
            newRed = 255;
        }

        if (newRed < 0){
            newRed = 0;
        }

        if (newGreen > 255){
            newGreen = 255;
        }

        if (newGreen < 0){
            newGreen = 0;
        }

        if (newBlue > 255){
            newBlue = 255;
        }

        if (newBlue < 0){
            newBlue = 0;
        }

        return Graphics::makeColor(newRed, newGreen, newBlue);
    }

    /* FIXME: this was copied verbatim from character.cpp */
    static PaintownUtil::ReferenceCount<Graphics::Shader> create(){
        PaintownUtil::ReferenceCount<Graphics::Shader> out;

#ifdef USE_ALLEGRO5
        std::ostringstream vertex;
        vertex << "#version 110\n";
        vertex << Graphics::defaultVertexShader();
        ALLEGRO_SHADER * a5shader = Graphics::create_shader(vertex.str(),
                                                            Storage::readFile(Storage::instance().find(Filesystem::RelativePath("shaders/mugen-palette-effect.fragment.glsl"))));
        out = PaintownUtil::ReferenceCount<Graphics::Shader>(new Graphics::Shader(a5shader));
#endif

        return out;
    }

    PaintownUtil::ReferenceCount<Graphics::Shader> shader;

    PaintownUtil::ReferenceCount<Graphics::Shader> getShader(){
        if (shader == NULL){
            PaintownUtil::ReferenceCount<Graphics::ShaderManager> manager = Graphics::shaderManager.current();
            /* This is the same shader name as the character palfx shader */
            shader = manager->getShader("mugen-palfx", create);
        }

        return shader;
    }

    void setupShader(const PaintownUtil::ReferenceCount<Graphics::Shader> & what){
#ifdef USE_ALLEGRO5
        Graphics::setShaderBool(shader->getShader(), "invert", invert > 0);
        Graphics::setShaderInt(shader->getShader(), "time", time);
        Graphics::setShaderInt(shader->getShader(), "period", period);
        Graphics::setShaderFloat(shader->getShader(), "color", (float) color / 255.0);
        Graphics::setShaderVec4(shader->getShader(), "add", (float) addRed / 255.0, (float) addGreen / 255.0, (float) addBlue / 255.0, 0);
        Graphics::setShaderVec4(shader->getShader(), "multiply", (float) multiplyRed / 256.0, (float) multiplyGreen / 256.0, (float) multiplyBlue / 256.0, 0);
        Graphics::setShaderVec4(shader->getShader(), "sin_", (float) sinRed / 255.0, (float) sinGreen / 255.0, (float) sinBlue / 255.0, 0);
#endif

    }

    Graphics::Color filter(Graphics::Color pixel) const {

        if (cache.find(pixel) != cache.end()){
            return cache[pixel];
        }

        int red = Graphics::getRed(pixel);
        int green = Graphics::getGreen(pixel);
        int blue = Graphics::getBlue(pixel);
        Graphics::Color out = doFilter(red, green, blue);
        cache[pixel] = out;
        return out;
    }
};

void Mugen::Stage::drawBackgroundWithEffectsSide(int x, int y, const Graphics::Bitmap & board, void (Mugen::Background::*render) (int, int, const Graphics::Bitmap &, Graphics::Bitmap::Filter *)){
    PaletteShader effects(paletteEffects.counter, paletteEffects.addRed,
                    paletteEffects.addGreen, paletteEffects.addBlue,
                    paletteEffects.multiplyRed, paletteEffects.multiplyGreen,
                    paletteEffects.multiplyBlue, paletteEffects.sinRed,
                    paletteEffects.sinGreen, paletteEffects.sinBlue,
                    paletteEffects.period, paletteEffects.invert,
                    paletteEffects.color);

    (background->*render)(x, y, board, &effects);
}
        
void Mugen::Stage::drawBackgroundWithEffects(int x, int y, const Graphics::Bitmap & board){
    drawBackgroundWithEffectsSide(x, y, board, &Background::renderBackground);
}

void Mugen::Stage::drawForegroundWithEffects(int x, int y, const Graphics::Bitmap & board){
    drawBackgroundWithEffectsSide(x, y, board, &Background::renderForeground);
}
    
PaintownUtil::ReferenceCount<Mugen::World> Mugen::Stage::snapshotState(){
    PaintownUtil::ReferenceCount<Mugen::World> world(new Mugen::World());

    for (vector<Mugen::Character*>::iterator it = objects.begin(); it != objects.end(); it++){
        Mugen::Character * character = *it;
        world->addCharacter(*character);
    }

    world->setStageData(getStateData());
    world->setRandom(*Random::getState());
    world->setGameTime(gameHUD->getGameTime());

    return world;
}
    
void Mugen::Stage::updateState(const Mugen::World & world){
    setStateData(world.getStageData());
    Random::setState(world.getRandom());
    gameHUD->setGameTime(world.getGameTime());

    const map<CharacterId, AllCharacterData> & data = world.getCharacterData();
    for (map<CharacterId, AllCharacterData>::const_iterator it = data.begin(); it != data.end(); it++){
        Mugen::Character * who = getCharacter(it->first);
        if (who != NULL){
            who->setStateData(it->second.character);
            who->setCurrentAnimationState(it->second.animation);
            who->setStatePersistent(it->second.statePersistent);
        }
    }
}

/* Returns a sorted listed of sprite priorties */
vector<int> Mugen::Stage::allSpritePriorities(){
    vector<int> priorities;

    for (vector<Mugen::Character*>::iterator it = objects.begin(); it != objects.end(); it++){
        Mugen::Character * object = *it;
        priorities.push_back(object->getSpritePriority());
    }

    for (vector<Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); it++){
        Projectile * projectile = *it;
        priorities.push_back(projectile->getSpritePriority());
    }
        
    for (vector<Mugen::Effect*>::iterator it = showSparks.begin(); it != showSparks.end(); it++){
        Mugen::Effect * spark = *it;
        priorities.push_back(spark->getSpritePriority());
    }

    std::sort(priorities.begin(), priorities.end());
    vector<int>::iterator newEnd = std::unique(priorities.begin(), priorities.end());
    priorities.resize(newEnd - priorities.begin());

    return priorities;
}

int Mugen::Stage::findMinimumSpritePriority(){
    vector<int> priorities = allSpritePriorities();
    std::sort(priorities.begin(), priorities.end());

    if (priorities.size() > 0){
        return priorities[0];
    }

    return 0;
}

int Mugen::Stage::findMaximumSpritePriority(){
    vector<int> priorities = allSpritePriorities();
    std::sort(priorities.begin(), priorities.end());

    if (priorities.size() > 0){
        return priorities[priorities.size() - 1];
    }

    return 0;
}

void Mugen::Stage::render(Graphics::Bitmap *work){

    if (getStateData().environmentColor.time == 0){
        if (paletteEffects.time > 0){
            drawBackgroundWithEffects((int) getStateData().camerax, (int) getStateData().cameray, *work);
        } else {
            background->renderBackground((int) getStateData().camerax, (int) getStateData().cameray, *work);
        }
    } else if (getStateData().environmentColor.under){
        /* FIXME: I'm not exactly sure where the environment color is supposed to go.
         * After the super pause? After the background hud elements?
         */
        work->fill(getStateData().environmentColor.color);
    }

    /* darken the background */
    if (getStateData().superPause.time > 0){
        /* FIXME: this should be faded I think */
        Graphics::Bitmap::transBlender(0, 0, 0, 128);
        work->translucent().rectangleFill(0, 0, work->getWidth(), work->getHeight(), Graphics::makeColor(0, 0, 0));
    }

    //! Render layer 0 HUD
    gameHUD->render(Mugen::Element::Background, *work);

    /* FIXME: this is a hack to deal with sprite priorities. Really we should get all
     * the drawable objects and sort them.
     */
    /*
    int minumumSpritePriority = findMinimumSpritePriority();
    int maximumSpritePriority = findMaximumSpritePriority();
    */

    vector<int> priorities = allSpritePriorities();
    for (vector<int>::iterator spritePriority = priorities.begin(); spritePriority != priorities.end(); spritePriority++){
        // Players go in here
        for (vector<Mugen::Character*>::iterator it = objects.begin(); it != objects.end(); it++){
            Mugen::Character *obj = *it;

            if (obj->getSpritePriority() == *spritePriority){
                /* Reflection */
                /* FIXME: reflection and shade need camerax/y */
                if (reflectionIntensity > 0){
                    obj->drawReflection(work, (int)(getStateData().camerax - DEFAULT_WIDTH / 2), (int) getStateData().cameray, reflectionIntensity);
                }

                /* Shadow */
                obj->drawMugenShade(work, (int)(getStateData().camerax - DEFAULT_WIDTH / 2), shadowIntensity, shadowColor, shadowYscale, shadowFadeRangeMid, shadowFadeRangeHigh);

                /* draw the player */
                obj->draw(work, (int)(getStateData().camerax - DEFAULT_WIDTH / 2), (int) getStateData().cameray);
            }
        }

        for (vector<Mugen::Effect*>::iterator it = showSparks.begin(); it != showSparks.end(); it++){
            Mugen::Effect * spark = *it;
            if (spark->getSpritePriority() == *spritePriority){
                spark->draw(*work, (int) (getStateData().camerax - DEFAULT_WIDTH / 2), (int) getStateData().cameray);
            }
        }

        for (vector<Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); it++){
            Projectile * projectile = *it;
            if (projectile->getSpritePriority() == *spritePriority){
                projectile->draw(*work, getStateData().camerax - DEFAULT_WIDTH / 2, getStateData().cameray);
            }
        }

    }

    if (getStateData().environmentColor.time > 0 && !getStateData().environmentColor.under){
        work->fill(getStateData().environmentColor.color);
    }

    //! Render layer 1 HUD
    gameHUD->render(Mugen::Element::Foreground, *work);

    if (getStateData().environmentColor.time == 0){
        if (paletteEffects.time > 0){
            drawForegroundWithEffects((int) getStateData().camerax, (int) getStateData().cameray, *work);
        } else {
            background->renderForeground((int) getStateData().camerax, (int) getStateData().cameray, *work);
        }
    }
    
    //! Render layer 2 HUD
    gameHUD->render(Mugen::Element::Top, *work);

    // Player debug
    for (vector<Mugen::Character*>::iterator it = objects.begin(); it != objects.end(); it++){
	if (isaPlayer(*it)){
            Mugen::Character *character = *it;
	    // Player debug crap
	    if (debugMode){
		// Players x positioning
		work->vLine( 150, (int)character->getX(), (int)character->getZ(), Graphics::makeColor( 255, 0, 0));
	    }
	}
    }
    
    // Debug crap for board coordinates
    if (debugMode){
	work->hLine( 0, abs(boundhigh) + currentZOffset(), work->getWidth(), Graphics::makeColor( 0,255,0 ));
	work->vLine( 0, xaxis, work->getHeight(), Graphics::makeColor(255,0,0));
    }
    
    // board->Blit( (int)(abs(boundleft) + camerax) + ( quake_time > 0 ? Util::rnd( 9 ) - 4 : 0 ), (int)(yaxis + cameray) + ( quake_time > 0 ? Util::rnd( 9 ) - 4 : 0 ), DEFAULT_WIDTH, DEFAULT_HEIGHT, 0,0, *work);

    /* FIXME: add quake back in */
    // board->Blit((int)(quake_time > 0 ? PaintownUtil::rnd( 9 ) - 4 : 0), (int)(quake_time > 0 ? PaintownUtil::rnd( 9 ) - 4 : 0), *work);
    
    // Debug crap for screen coordinates
    if (debugMode){
	work->vLine( 0, tension, 240, Graphics::makeColor( 0,255,0 ));
	work->vLine( 0, 320 - tension, 240, Graphics::makeColor( 0,255,0 ));
    }
    
    /*
    // Life bars, will eventually be changed out with mugens interface
    for (vector<Mugen::Object*>::iterator it = objects.begin(); it != objects.end(); it++){
	int p1Side = 5;
	int p2Side = 5;
	if (isaPlayer(*it)){
            Mugen::Character *character = (Mugen::Character*)*it;
	    if (character->getAlliance() == Player1Side){
                / * FIXME! * /
		// character->drawLifeBar( 5, p1Side, work );
		p1Side += 10;
	    } else if (character->getAlliance() == Player2Side){
                / * FIXME! * /
		// character->drawLifeBar( 215, p2Side, work );
		p2Side += 10;
	    }
	}
    }
    */
    
    // Render console
    // console->draw(*work);
}
    
void Mugen::Stage::setMatchWins(int wins){
    this->gameHUD->setMatchWins(wins);
}

void Mugen::Stage::reset(){
    getStateData().camerax = startx;
    getStateData().cameray = starty;
    originalMaxLeft = maximumLeft(NULL);
    originalMaxRight = maximumRight(NULL);
    gameOver = false;
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

    /* FIXME: implement this again */
    // background->reset(startx, starty, resetBG);
    
    // Reset player positions
    for (vector<Mugen::Character*>::iterator it = objects.begin(); it != objects.end();){
        Mugen::Character *player = *it;

        /* remove any non-player objects, like projectiles or helpers */
        if (!isaPlayer(player)){
            player->destroyed(*this);
            // unbind(player);
            delete player;
            it = objects.erase(it);
        } else {
            Mugen::Character * character = player;
            vector<string> inputs;
            //character->changeState(*this, Mugen::Intro, inputs);
            character->setHealth(character->getMaxHealth());
            if (player->getAlliance() == Player1Side){
                //((Player *)player)->deathReset();
                player->setX(p1startx);
                player->setY(p1starty);
                player->setZ(currentZOffset());
                player->setFacing(FacingRight);
                playerInfo[player].oldx = player->getX();
                playerInfo[player].oldy = player->getY();
                playerInfo[player].leftTension = false;
                playerInfo[player].rightTension = false;
                playerInfo[player].leftSide = false;
                playerInfo[player].rightSide = false;
                playerInfo[player].jumped = false;
            } else if (player->getAlliance() == Player2Side){
                //((Player *)player)->deathReset();
                player->setX(p2startx);
                player->setY(p2starty);
                player->setZ(currentZOffset());
                player->setFacing(FacingLeft);
                playerInfo[player].oldx = player->getX();
                playerInfo[player].oldy = player->getY();
                playerInfo[player].leftTension = false;
                playerInfo[player].rightTension = false;
                playerInfo[player].leftSide = false;
                playerInfo[player].rightSide = false;
                playerInfo[player].jumped = false;
            }
            it++;
        }
    }
    
    getStateData().inleft = getStateData().inright = getStateData().onLeftSide = getStateData().onRightSide = 0;
    if (players.size() < 2){
        throw MugenException("Need at least 2 players", __FILE__, __LINE__);
    }

    if (gameHUD == NULL){
        throw MugenException("Internal error: Stage not loaded, call load()", __FILE__, __LINE__);
    }

    gameHUD->reset(*this, *((Mugen::Character *)players[0]),*((Mugen::Character *)players[1]));
}

std::vector<Mugen::Character *> Mugen::Stage::getPlayers() const {
    return players;
}

// Add player1 people
void Mugen::Stage::addPlayer1(Mugen::Character * o){
    o->setAlliance(Player1Side);
    o->setX(p1startx);
    o->setY(p1starty);
    o->setZ(currentZOffset());
    o->setFacing(FacingRight);
    o->setId(nextId());
    objects.push_back(o);
    players.push_back(o);

    playerInfo[o].oldx = o->getX();
    playerInfo[o].oldy = o->getY();
    playerInfo[o].leftTension = false;
    playerInfo[o].rightTension = false;
    playerInfo[o].leftSide = false;
    playerInfo[o].rightSide = false;
    playerInfo[o].jumped = false;

    o->setCommonSounds(&sounds);
}

// Add player2 people
void Mugen::Stage::addPlayer2(Mugen::Character * o){
    o->setAlliance(Player2Side);
    o->setX(p2startx);
    o->setY(p2starty);
    o->setZ(currentZOffset());
    o->setFacing(FacingLeft);
    o->setId(nextId());
    objects.push_back(o);
    players.push_back(o);
    
    playerInfo[o].oldx = o->getX();
    playerInfo[o].oldy = o->getY();
    playerInfo[o].leftTension = false;
    playerInfo[o].rightTension = false;
    playerInfo[o].leftSide = false;
    playerInfo[o].rightSide = false;
    playerInfo[o].jumped = false;

    o->setCommonSounds(&sounds);
}

void Mugen::Stage::setPlayerHealth(int health){
    for (vector<Mugen::Character *>::iterator it = players.begin(); it != players.end(); it++ ){
        Mugen::Character *player = *it;
	player->setHealth(health);
    }
}

/*
// Console
void Mugen::Stage::toggleConsole(){ 
    console->toggle(); 
}
*/

void Mugen::Stage::toggleDebug(int choose){
    debugMode = !debugMode;
    int count = 0;
    for (vector<Mugen::Character *>::iterator it = players.begin(); it != players.end(); it++, count++){
        Mugen::Character *player = *it;
        if (player != NULL){
            if (debugMode){
                player->enableDebug();
            } else {
                player->disableDebug();
            }
        }
    }
}

void Mugen::Stage::draw( Graphics::Bitmap * work ){
    render(work);
}

void Mugen::Stage::addObject(Mugen::Character * o){
    o->setId(nextId());
    addedObjects.push_back(o);
}

bool Mugen::Stage::finished() const { return false; }
void Mugen::Stage::reloadLevel(){
    cleanup();
    loaded = false;
    load(); 
}
    
Mugen::CharacterId Mugen::Stage::nextId(){
    int now = objectId;
    objectId += 1;
    return CharacterId(now);
}

/* bleh.. */
/*
Mugen::Character * Mugen::Stage::findObject(int id){ 
    for (vector<Mugen::Character*>::iterator it = objects.begin(); it != objects.end(); it++){
        Mugen::Character * object = *it;
        if (object->getd() == id){
            return object;
        }
    }
    return NULL;
}
*/

// These should be the same, but we'll see, mugen has some funny parameters
int Mugen::Stage::getMaximumZ(){ return zoffset; }
int Mugen::Stage::getMinimumZ(){ return zoffset; }

void Mugen::Stage::begin(){
}

const std::string Mugen::Stage::getStageName(const std::string &filename){
    // Lets look for our def since some people think that all file systems are case insensitive
    Filesystem::AbsolutePath dir = Storage::instance().find(Filesystem::RelativePath("mugen/stages/"));
    Global::debug(1) << dir.path() << endl;
    string fullname = filename;
    if ( fullname.find(".def") == std::string::npos){
	fullname += ".def";
    }
    const Filesystem::AbsolutePath defFile = Mugen::Util::fixFileName( dir, std::string(fullname) );
    
    if (defFile.isEmpty()){
        throw MugenException( "Cannot locate stage definition file for: " + fullname, __FILE__, __LINE__);
    }
    
    std::string filesdir = "";
    
    size_t strloc = fullname.find_last_of("/");
    if (strloc != std::string::npos){
	filesdir = fullname.substr(0, strloc);
	filesdir += "/";
    }
    
    Global::debug(1) << "Got subdir: " << filesdir << endl;
    
    AstRef parsed(Mugen::Util::parseDef(defFile));
    return parsed->findSection("info")->findAttribute("name")->valueAsString();
        
    throw MugenException( "Cannot locate stage definition file for: " + fullname, __FILE__, __LINE__);
    return "";
}

void Mugen::Stage::cleanup(){
    if (loaded){
	if (background){
	    delete background;
	    background = 0;
	}
	
        /*
	if (board){
	    delete board;
	    board = 0;
	}
        */
	
        /*
	if (console){
	    delete console;
	    console = 0;
	}
        */
/*
        for (Mugen::SpriteMap::iterator it1 = effects.begin(); it1 != effects.end(); it1++){
            for (Mugen::GroupMap::iterator it2 = (*it1).second.begin(); it2 != (*it1).second.end(); it2++){
                PaintownUtil::ReferenceCount<Mugen::Sprite> sprite = (*it2).second;
                delete sprite;
            }
        }
        */
        effects.clear();

        sparks.clear();

        for (vector<Mugen::Effect*>::iterator it = showSparks.begin(); it != showSparks.end(); it++){
            delete *it;
        }
        showSparks.clear();
/*
        for (map<unsigned int, map<unsigned int, Mugen::Sound*> >::iterator it1 = sounds.begin(); it1 != sounds.end(); it1++){
            map<unsigned int, Mugen::Sound*> & group = (*it1).second;
            for (map<unsigned int, Mugen::Sound*>::iterator it2 = group.begin(); it2 != group.end(); it2++){
                delete (*it2).second;
            }
        }
        */
        sounds.clear();

        for (vector<Mugen::Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); it++){
            delete *it;
        }
        projectiles.clear();

        for (vector<Mugen::Character*>::iterator it = objects.begin(); it != objects.end(); /**/){
            Mugen::Character * object = *it;

            /* remove any non-player objects, like projectiles or helpers */
            if (!isaPlayer(object)){
                object->destroyed(*this);
                // unbind(object);
                delete object;
                it = objects.erase(it);
            } else {
                it++;
            }
        }
    }
}

bool Mugen::Stage::isaPlayer(Mugen::Character * o) const {
    for (vector<Mugen::Character *>::const_iterator it = players.begin(); it != players.end(); it++ ){
        if ( (*it) == o ){
            return true;
        }
    }
    return false;
}

int Mugen::Stage::getStartingLeft() const {
    return originalMaxLeft;
}

int Mugen::Stage::getStartingRight() const {
    return originalMaxRight;
}

int Mugen::Stage::maximumRight(const Character * who) const {
    if (who != NULL){
        map<CharacterId, ScreenBound>::const_iterator find = getStateData().screenBound.find(who->getId());
        if (find != getStateData().screenBound.end() &&
            find->second.enabled && find->second.offScreen){
            return boundright + DEFAULT_WIDTH / 2;
        }
    }
    return (int)(getStateData().camerax + DEFAULT_WIDTH / 2);
}

int Mugen::Stage::maximumLeft(const Character * who) const {
    if (who != NULL){
        map<CharacterId, ScreenBound>::const_iterator find = getStateData().screenBound.find(who->getId());
        if (find != getStateData().screenBound.end() &&
            find->second.enabled && find->second.offScreen){
            return boundleft - DEFAULT_WIDTH / 2;
        }
    }
    return (int)(getStateData().camerax - DEFAULT_WIDTH / 2);
}

/* FIXME: I think screenbound should deal with maximumUp/Down */
int Mugen::Stage::maximumUp() const {
    return getStateData().cameray;
}

int Mugen::Stage::maximumDown() const {
    return getStateData().cameray + DEFAULT_HEIGHT;
}
    
void Mugen::Stage::addProjectile(Projectile * projectile){
    projectiles.push_back(projectile);
}

void Mugen::Stage::updatePlayer(Mugen::Character * player){

    // Move X and Camera
    const double px = player->getX();
    const double py = player->getY();
    const double pdiffx = px - playerInfo[player].oldx;
    /*
       const double screenLeft = camerax - DEFAULT_WIDTH / 2;
       const double screenRight = camerax + DEFAULT_WIDTH / 2;
       */
    const double screenLeft = maximumLeft(player);
    const double screenRight = maximumRight(player);

    /*
       const double screenLeft = abs(boundleft) + camerax;
       const double screenRight = abs(boundleft) + camerax + DEFAULT_WIDTH;
       */
    const double leftTension = screenLeft + tension;
    const double rightTension = screenRight - tension;
    // Check leftbound rightbound
    if (px < leftbound){
        player->setX(leftbound);
    } else if (px > rightbound){
        player->setX(rightbound);
    }
    /*
       if (px <= (abs(boundleft) + boundleft)){ 
       o->setX(abs(boundleft) + boundleft );
       playerInfo[o].oldx = px;
       } else if (px >= (DEFAULT_WIDTH + abs(boundleft) + boundright)){
       o->setX(DEFAULT_WIDTH + abs(boundleft) + boundright );
       playerInfo[o].oldx = px;
       } 
       */

    // Check if in tension
    if (px <= leftTension){
        if (!playerInfo[player].leftTension){
            playerInfo[player].leftTension = true;
            getStateData().inleft++;
        }
    } else if (px >= rightTension){
        if (!playerInfo[player].rightTension){
            playerInfo[player].rightTension = true;
            getStateData().inright++;
        }
    } 

    // Release tension
    if (px > leftTension){
        if (playerInfo[player].leftTension){
            playerInfo[player].leftTension = false;
            getStateData().inleft--;
        }
    }
    // Release tension
    if (px < rightTension){
        if (playerInfo[player].rightTension){
            playerInfo[player].rightTension = false;
            getStateData().inright--;
        }
    }

    // Check Screen sides
    if (px <= screenLeft){
        player->setX(screenLeft);
        playerInfo[player].oldx = px;
        if (!playerInfo[player].leftSide){
            playerInfo[player].leftSide = true;
            getStateData().onLeftSide++;
        }
    } else if (px >= screenRight){
        player->setX(screenRight);
        playerInfo[player].oldx = px;
        if (!playerInfo[player].rightSide){
            playerInfo[player].rightSide = true;
            getStateData().onRightSide++;
        }
    } 
    // Release side
    if (px > screenLeft){
        if (playerInfo[player].leftSide){
            playerInfo[player].leftSide = false;
            getStateData().onLeftSide--;
        }
    }
    // Release side
    if (px < screenRight){
        if (playerInfo[player].rightSide){
            playerInfo[player].rightSide = false;
            getStateData().onRightSide--;
        }
    }

    //Global::debug(0) << "Left Tension: " << inleft << " | Right Tension: "<< inright << endl;
    //Global::debug(0) << "Left Screen Edge: " << onLeftSide << " | Right Screen Edge: "<< onRightSide << endl;

    if (!getStateData().screenBound[player->getId()].enabled ||
        getStateData().screenBound[player->getId()].panX){
        if (playerInfo[player].leftTension){
            if (pdiffx < 0){
                if (!getStateData().onRightSide){
                    moveCamera(pdiffx, 0);
                }
            } else if (pdiffx > 0){
                if (getStateData().inright){
                    moveCamera(pdiffx, 0);
                }
            }
        } else if (playerInfo[player].rightTension){
            if (pdiffx > 0){
                if(!getStateData().onLeftSide){
                    moveCamera(pdiffx, 0);
                }
            } else if (pdiffx < 0){
                if(getStateData().inleft){
                    moveCamera(pdiffx, 0);
                }
            }
        }
    }

    if (!getStateData().screenBound[player->getId()].enabled ||
        getStateData().screenBound[player->getId()].panY){
        // Vertical movement of camera
        if (playerInfo[player].oldy != py){
            if (verticalfollow > 0){
                const double pdiffy = playerInfo[player].oldy - py;
                if (py > floortension){
                    if (!playerInfo[player].above){
                        playerInfo[player].above = true;
                        getStateData().inabove++;
                    }
                } else if ( playerInfo[player].above){
                    playerInfo[player].above = false;
                    getStateData().inabove--;
                }
                if (playerInfo[player].above && pdiffy < 0){
                    moveCamera( 0, verticalfollow * -3.2 );
                } else if (playerInfo[player].above && pdiffy > 0){
                    moveCamera( 0, verticalfollow * 3.2 );
                }
            }
        }

    }
    //Global::debug(1) << "Our players Y: " << py << " | Above: "<< playerInfo[o].above << " | total inabove: " << inabove << endl;
}

void Mugen::Stage::initializeName(){
    try{
	name = Mugen::Util::probeDef(location, "info", "name");
    } catch (const MugenException &ex){
	Global::debug(1) << "Couldn't find the name of the stage!" << endl;
	Global::debug(1) << "Error was: " << ex.getReason() << endl;
    }
}
        
void Mugen::Stage::setGameRate(double rate){
    gameRate = rate;
    if (rate <= 0){
        gameRate = 0.1;
    }
}

//! Do continue screen return true to continue playing, false to end
bool Mugen::Stage::doContinue(const Mugen::PlayerType & type, InputMap<Mugen::Keys> & input){
    Filesystem::AbsolutePath systemFile = Mugen::Data::getInstance().getMotif();
    
    // Check if we have the continue screen enabled
    std::string enabled = Mugen::Util::probeDef(systemFile, "Continue Screen", "enabled");
    // If so we can render it otherwise it's not enabled, so no continue
    if (enabled != "1"){
        return false;
    }

    // Lets look for our def since some people think that all file systems are case insensitive
    Filesystem::AbsolutePath baseDir = systemFile.getDirectory();

    // Uses system font3 by default
    std::string fontFile = Mugen::Util::probeDef(systemFile, "Files", "font3");

    Font font(Mugen::Util::findFile(Filesystem::RelativePath(fontFile)));
    
    Mugen::Character * character = NULL;

    switch (type){
        case Mugen::Player1:
            character = ((Mugen::Character *)players[0]);
            character->setFacing(FacingRight);
            break;
        case Mugen::Player2:
            character = ((Mugen::Character *)players[1]);
            character->setFacing(FacingLeft);
            break;
        default: {
            ostringstream fail;
            fail << "Internal error: unknown player type " << type;
            throw MugenException(fail.str(), __FILE__, __LINE__);
            break;
        }
    }
    

    double gameSpeed = 1.0;
    double runCounter = 0;

    // Put character in continue state
    character->changeState(*this, Mugen::Continue);

    class Logic: public PaintownUtil::Logic {
    public:
        Logic(InputMap<Mugen::Keys> & input, Mugen::Character * character, Mugen::Stage * stage):
        is_done(false),
        answer(false),
        selector(true),
        input(input),
        character(character),
        stage(stage){
        }

        bool is_done;
        bool answer;
        bool selector;
        InputMap<Mugen::Keys> & input;
        Mugen::Character * character;
        Mugen::Stage * stage;

        bool getAnswer() const {
            return answer;
        }

        bool done(){
            return is_done;
        }

        bool selectedYes() const {
            return selector;
        }

        void run(){
            vector<InputMap<Mugen::Keys>::InputEvent> out = InputManager::getEvents(input, InputSource(true));
            for (vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
                const InputMap<Mugen::Keys>::InputEvent & event = *it;
                if (!event.enabled){
                    continue;
                }

                if (event[Mugen::Left] || event[Mugen::Right]){
                    selector = !selector;
                }

                if (event[Mugen::A] || event[Mugen::B] || event[Mugen::C] ||
                    event[Mugen::X] || event[Mugen::Y] || event[Mugen::Z]){
                    if (selector){
                        is_done = true;
                        answer = true;
                    } else {
                        is_done = true;
                        answer = false;
                    }
                }
                // If enter return true
                if (event[Mugen::Esc]){
                    is_done = true;
                    answer = false;
                }
            }
            character->act(stage);
        }

        double ticks(double system){
            return Util::gameTicks(system);
        }
    };

    class Draw: public PaintownUtil::Draw {
    public:
        Draw(Mugen::Background * background, int reflectionIntensity, Mugen::Character * character, double cameray, int shadowIntensity, Graphics::Color shadowColor, double shadowYscale, int shadowFadeRangeHigh, int shadowFadeRangeMid, Font & font, const Logic & logic):
        background(background),
        reflectionIntensity(reflectionIntensity),
        character(character),
        cameray(cameray),
        shadowIntensity(shadowIntensity),
        shadowColor(shadowColor),
        shadowYscale(shadowYscale),
        shadowFadeRangeHigh(shadowFadeRangeHigh),
        shadowFadeRangeMid(shadowFadeRangeMid),
        font(font),
        logic(logic){
        }

        Mugen::Background * background;
        int reflectionIntensity;
        Mugen::Character * character;
        double cameray;

        int shadowIntensity;
        Graphics::Color shadowColor;
        double shadowYscale;
        int shadowFadeRangeHigh;
        int shadowFadeRangeMid;
    
        Font & font;
        const Logic & logic;

        void draw(const Graphics::Bitmap & screen){
            Graphics::StretchedBitmap board(320, 240, screen, Graphics::qualityFilterName(::Configuration::getQualityFilter()));
            board.start();
            // Render background
            background->renderBackground(0, 0, board);
        
            // do darkened background
            // Bitmap::drawingMode(Bitmap::MODE_TRANS);
            Graphics::Bitmap::transBlender(0,0,0,150);
	    board.translucent().rectangleFill(0, 0, board.getWidth(), board.getHeight(), Graphics::makeColor(0,0,0));
	    // Bitmap::drawingMode(Bitmap::MODE_SOLID);
            
            // Render character
            if (reflectionIntensity > 0){
                character->drawReflection(&board, -(DEFAULT_WIDTH / 2), (int) cameray, reflectionIntensity);
            }

	    /* Shadow */
	    character->drawMugenShade(&board, -(DEFAULT_WIDTH / 2), shadowIntensity, shadowColor, shadowYscale, shadowFadeRangeMid, shadowFadeRangeHigh);
        
            character->draw(&board, -(DEFAULT_WIDTH / 2), (int) cameray); 
            
            // Render continue text
            font.render(DEFAULT_WIDTH/2, 40, 0, 0, board, "Continue?" );

            // Render yes and no
            if (logic.selectedYes()){
                font.render(DEFAULT_WIDTH/2 - 20, 50, 0, 4, board, "Yes");
                font.render(DEFAULT_WIDTH/2 + 20, 50, 0, 0, board, "No");
            } else {
                font.render(DEFAULT_WIDTH/2 - 20, 50, 0, 0, board, "Yes");
                font.render(DEFAULT_WIDTH/2 + 20, 50, 0, 4, board, "No");
            }
        
            // Foreground
            background->renderForeground(0, 0, board);

            board.finish();
            // board->Stretch(buffer);
            // screen.BlitToScreen();
        }
    };

    Logic logic(input, character, this);
    Draw draw(background, reflectionIntensity, character, getStateData().cameray, shadowIntensity, shadowColor, shadowYscale, shadowFadeRangeHigh, shadowFadeRangeMid, font, logic);

    PaintownUtil::standardLoop(logic, draw);

    return logic.getAnswer();
}
    
Mugen::Character * Mugen::Stage::getEnemy(const Mugen::Character * who) const {
    for (vector<Mugen::Character*>::const_iterator enem = objects.begin(); enem != objects.end(); ++enem){
        Mugen::Character * enemy = *enem;
        if (who->getAlliance() != enemy->getAlliance() && isaPlayer(enemy)){
            return enemy;
        }
    }

    return NULL;
}

int Mugen::Stage::getGameTime() const {
    return getTicks();
    /*
    if (gameHUD){
        return gameHUD->getGameTime();
    }
    return 0;
    */
}
    
void Mugen::Stage::doSuperPause(int time, Character & guy, int animation, bool ownAnimation, int positionX, int positionY){
    getStateData().superPause.time = time;

    for (vector<Mugen::Effect*>::iterator it = showSparks.begin(); it != showSparks.end(); it++){ 
        Mugen::Effect * effect = *it;
        effect->superPauseStart();
    }

    if (animation != -1){
        addSpark(positionX, positionY, animation, ownAnimation, &guy);
    }
}
   
void Mugen::Stage::doPause(int time, int buffer, int moveAllowed, bool pauseBackground, Character * who){
    getStateData().pause.time = time;
    getStateData().pause.buffer = buffer;
    getStateData().pause.moveTime = moveAllowed;
    getStateData().pause.pauseBackground = pauseBackground;
    getStateData().pause.who = who->getId();
}
    
void Mugen::Stage::createDust(int x, int y){
    addSpark(x, y, 120, false, NULL);
}
        
void Mugen::Stage::addEffect(Mugen::Effect * effect){
    showSparks.push_back(effect);
}

int Mugen::Stage::countMyHelpers(const Mugen::Character * owner) const {
    int count = 0;
    for (vector<Mugen::Character*>::const_iterator it = objects.begin(); it != objects.end(); it++){
        Mugen::Character * who = *it;
        if (who->isHelper()){
            Mugen::Helper * helper = (Mugen::Helper*) who;
            if (helper->getParent() == owner->getId()){
                count += 1;
            }
        }
    }
    return count;
}

vector<Mugen::Character *> Mugen::Stage::getTargets(int id, const Mugen::Character * from) const {
    vector<Mugen::Character *> targets;
    if (id == -1){
        for (vector<Mugen::Character*>::const_iterator enem = objects.begin(); enem != objects.end(); ++enem){
            Mugen::Character * enemy = *enem;
            if (from->getAlliance() != enemy->getAlliance() && isaPlayer(enemy)){
                targets.push_back(enemy);
            }
        }
    } else {
        Mugen::Character * target = getCharacter(from->getTargetId(id));
        if (target != NULL){
            targets.push_back(target);
        }
    }
    return targets;
}
    
/* Set the background to a solid color for some length of time */
void Mugen::Stage::setEnvironmentColor(Graphics::Color color, int time, bool under){
    getStateData().environmentColor.time = time;
    getStateData().environmentColor.under = under;
    getStateData().environmentColor.color = color;
}

void Mugen::Stage::removeHelper(Mugen::Character * who){
    /* Make sure its actually a helper */
    if (who->isHelper()){
        /* The character will ultimately be removed in the logic loop */
        who->setHealth(-1);

        /* Remove any effects owned by this character/helper.
         * Technically this isn't needed because when the helper is destroyed in the
         * main logic loop it will call destroyed() which will eventually
         * call stage.removeEffects(this), but theres no real harm in doing
         * it here as well.
         */
        removeEffects(who, -1);

        vector<Mugen::Helper*> children = findHelpers(who);
        for (vector<Mugen::Helper*>::iterator it = children.begin(); it != children.end(); it++){
            Mugen::Helper * helper = *it;
            /* lose parent association, still has root though */
            helper->reParent(CharacterId(-1));
        }

    }
}

void Mugen::Stage::removeEffects(const Mugen::Character * owner, int id){
    for (vector<Mugen::Effect*>::iterator it = showSparks.begin(); it != showSparks.end(); /**/ ){ 
        Mugen::Effect * effect = *it;
        if (effect->getOwner() == owner && (id == -1 || id == effect->getId())){
            delete effect;
            it = showSparks.erase(it);
        } else {
            it++;
        }
    }
}

Mugen::Character * Mugen::Stage::getCharacter(const CharacterId & id) const {
    for (vector<Mugen::Character*>::const_iterator it = objects.begin(); it != objects.end(); it++){
        Mugen::Character * object = *it;
        if (object->getId() == id){
            return object;
        }
    }
    return NULL;
}

std::vector<Mugen::Effect*> Mugen::Stage::findExplode(int id, const Character * owner) const {
    vector<Effect*> found;

    for (vector<Effect*>::const_iterator it = showSparks.begin(); it != showSparks.end(); it++){
        Effect * effect = *it;
        if ((id == 0 || effect->getId() == id) && effect->getOwner() == owner){
            found.push_back(effect);
        }
    }

    return found;
}

vector<Mugen::Projectile*> Mugen::Stage::findProjectile(int id, const Character * owner) const {
    vector<Projectile*> found;

    for (vector<Projectile*>::const_iterator it = projectiles.begin(); it != projectiles.end(); it++){
        Projectile * projectile = *it;
        if ((id == 0 || projectile->getId() == id) && projectile->getOwner() == owner->getId()){
            found.push_back(projectile);
        }
    }

    return found;
}

vector<Mugen::Helper*> Mugen::Stage::findHelpers(const Mugen::Character * owner, int id) const {
    vector<Mugen::Helper*> out;
    for (vector<Mugen::Character*>::const_iterator it = objects.begin(); it != objects.end(); it++){
        Mugen::Character * who = *it;
        if (who->isHelper()){
            Mugen::Helper * helper = (Mugen::Helper*) who;
            if (helper->getHelperId() == id && helper->getParent() == owner->getId()){
                out.push_back(helper);
            }
        }
    }

    /* Have to check the addedObjects vector in case the player adds a helper and then
     * immediately checks for its existence in the same tick. `Guy' does this.
     */
    for (vector<Mugen::Character*>::const_iterator it = addedObjects.begin(); it != addedObjects.end(); it++){
        Mugen::Character * who = *it;
        if (who->isHelper()){
            Mugen::Helper * helper = (Mugen::Helper*) who;
            if (helper->getHelperId() == id && helper->getParent() == owner->getId()){
                out.push_back(helper);
            }
        }
    }
    return out;
}

vector<Mugen::Helper*> Mugen::Stage::findHelpers(const Mugen::Character * owner) const {
    vector<Mugen::Helper*> out;
    for (vector<Mugen::Character*>::const_iterator it = objects.begin(); it != objects.end(); it++){
        Mugen::Character * who = *it;
        if (who->isHelper()){
            Mugen::Helper * helper = (Mugen::Helper*) who;
            if (helper->getParent() == owner->getId()){
                out.push_back(helper);
            }
        }
    }

    for (vector<Mugen::Character*>::const_iterator it = addedObjects.begin(); it != addedObjects.end(); it++){
        Mugen::Character * who = *it;
        if (who->isHelper()){
            Mugen::Helper * helper = (Mugen::Helper*) who;
            if (helper->getParent() == owner->getId()){
                out.push_back(helper);
            }
        }
    }
    return out;
}
    
Mugen::Effect * Mugen::Stage::findEffect(const Mugen::Character * owner, int id){
    for (vector<Mugen::Effect*>::iterator it = showSparks.begin(); it != showSparks.end(); it++){ 
        Mugen::Effect * effect = *it;
        if (effect->getOwner() == owner && id == effect->getId()){
            return effect;
        }
    }

    return NULL;
}

vector<Mugen::Effect *> Mugen::Stage::findEffects(const Mugen::Character * owner, int id){
    vector<Mugen::Effect*> effects;
    for (vector<Mugen::Effect*>::iterator it = showSparks.begin(); it != showSparks.end(); it++){ 
        Mugen::Effect * effect = *it;
        if (effect->getOwner() == owner && (id == effect->getId() || id == -1)){
            effects.push_back(effect);
        }
    }

    return effects;
}

void Mugen::Stage::setPaletteEffects(int time, int addRed, int addGreen, int addBlue, int multiplyRed, int multiplyGreen, int multiplyBlue, int sinRed, int sinGreen, int sinBlue, int period, int invert, int color){
    paletteEffects.time = time;
    paletteEffects.addRed = addRed;
    paletteEffects.addGreen = addGreen;
    paletteEffects.addBlue = addBlue;
    paletteEffects.multiplyRed = multiplyRed;
    paletteEffects.multiplyGreen = multiplyGreen;
    paletteEffects.multiplyBlue = multiplyBlue;
    paletteEffects.sinRed = sinRed;
    paletteEffects.sinGreen = sinGreen;
    paletteEffects.sinBlue = sinBlue;
    paletteEffects.period = period;
    paletteEffects.invert = invert;
    paletteEffects.color = color;
    paletteEffects.counter = 0;
}

void Mugen::Stage::Quake(int q){
    getStateData().quake_time += q;
}

void Mugen::Stage::enableScreenBound(Character * who, bool offScreen, bool panX, bool panY){
    CharacterId id = who->getId();
    getStateData().screenBound[id].enabled = true;
    getStateData().screenBound[id].offScreen = offScreen;
    getStateData().screenBound[id].panX = panX;
    getStateData().screenBound[id].panY = panY;
}

void Mugen::Stage::doZoom(double x, double y, int zoomTime, int zoomOutTime, int time,
                          int bindTime, double scaleX, double scaleY,
                          double velocityX, double velocityY, double accelX, double accelY,
                          int superMoveTime, int pauseMoveTime, bool removeOnGetHit,
                          Character * bound, Character * owner){

    /* Don't allow a new zoom to override an existing zoom */
    if (getStateData().zoom.enabled){
        return;
    }

    getStateData().zoom.enabled = true;
    getStateData().zoom.x = x;
    getStateData().zoom.y = y;
    getStateData().zoom.zoom = zoomTime;
    getStateData().zoom.in = true;
    getStateData().zoom.zoomTime = zoomTime;
    getStateData().zoom.zoomOutTime = zoomOutTime;
    if (getStateData().zoom.zoomTime < 1){
        getStateData().zoom.zoomTime = 1;
    }
    if (getStateData().zoom.zoomOutTime < 1){
        getStateData().zoom.zoomOutTime = 1;
    }
    getStateData().zoom.time = time;
    getStateData().zoom.bindTime = bindTime;
    if (scaleX < 1){
        scaleX = 1;
    }
    if (scaleY < 1){
        scaleY = 1;
    }
    getStateData().zoom.scaleX = 1.0 - 1.0/scaleX;
    getStateData().zoom.scaleY = 1.0 - 1.0/scaleY;

    getStateData().zoom.velocityX = velocityX;
    getStateData().zoom.velocityY = velocityY;
    getStateData().zoom.accelX = accelX;
    getStateData().zoom.accelY = accelY;
    getStateData().zoom.superMoveTime = superMoveTime;
    getStateData().zoom.pauseMoveTime = pauseMoveTime;
    getStateData().zoom.removeOnGetHit = removeOnGetHit;
    getStateData().zoom.bound = bound->getId();
    getStateData().zoom.hitCount = 0;
    getStateData().zoom.owner = owner->getId();
    if (owner != NULL){
        getStateData().zoom.hitCount = owner->getWasHitCount();
    }
    if (bound != NULL){
        getStateData().zoom.deltaX = getStateData().zoom.x - bound->getX();
        getStateData().zoom.deltaY = getStateData().zoom.y - bound->getRY();
    } else {
        getStateData().zoom.deltaX = 0;
        getStateData().zoom.deltaY = 0;
    }
}

bool Mugen::Stage::isZoomed() const {
    return getStateData().zoom.enabled;
}

double Mugen::Stage::zoomScale() const {
    if (getStateData().zoom.in){
        return (double) (getStateData().zoom.zoomTime - getStateData().zoom.zoom) / (double) getStateData().zoom.zoomTime;
    } else {
        return (double) getStateData().zoom.zoom / (double) getStateData().zoom.zoomOutTime;
    }
}

int Mugen::Stage::zoomX1() const {
    return (getStateData().zoom.x - maximumLeft(NULL)) * getStateData().zoom.scaleX * zoomScale();
}

int Mugen::Stage::zoomY1() const {
    return getStateData().zoom.y * getStateData().zoom.scaleY * zoomScale();
}

int Mugen::Stage::zoomX2() const {
    return DEFAULT_WIDTH - (DEFAULT_WIDTH - (getStateData().zoom.x - maximumLeft(NULL))) * getStateData().zoom.scaleX * zoomScale();
}

int Mugen::Stage::zoomY2() const {
    return DEFAULT_HEIGHT - (DEFAULT_HEIGHT - getStateData().zoom.y) * getStateData().zoom.scaleY * zoomScale();
}

Mugen::StageStateData & Mugen::Stage::getStateData(){
    return stateData;
}
    
const Mugen::StageStateData & Mugen::Stage::getStateData() const {
    return stateData;
}
    
void Mugen::Stage::setStateData(const Mugen::StageStateData & data){
    this->stateData = data;
}

Mugen::StageObserver::StageObserver(){
}

Mugen::StageObserver::~StageObserver(){
}

void Mugen::Stage::setObserver(const PaintownUtil::ReferenceCount<StageObserver> & observer){
    this->observer = observer;
}
    
PaintownUtil::ReferenceCount<Mugen::StageObserver> Mugen::Stage::getObserver(){
    return observer;
}
