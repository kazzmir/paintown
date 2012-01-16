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

#include "util/events.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/bitmap.h"
#include "util/stretch-bitmap.h"
#include "util/trans-bitmap.h"
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
#include "item.h"
#include "item-content.h"
#include "section.h"
#include "projectile.h"
#include "sound.h"
#include "reader.h"
#include "sprite.h"
#include "util.h"
#include "characterhud.h"

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

Effect::Effect(const Character * owner, PaintownUtil::ReferenceCount<MugenAnimation> animation, int id, int x, int y, double scaleX, double scaleY, int spritePriority):
owner(owner),
/* Copy the animation here so that it can start from frame 0 and not accidentally
 * be shared with another Effect
 */
animation(PaintownUtil::ReferenceCount<MugenAnimation>(new MugenAnimation(*animation))),
id(id),
x(x),
y(y),
scaleX(scaleX),
scaleY(scaleY),
spritePriority(spritePriority){
}
    
void Effect::draw(const Graphics::Bitmap & work, int cameraX, int cameraY){
    animation->render((int)(x - cameraX), (int)(y - cameraY), work, scaleX, scaleY);
}

void Effect::logic(){
    animation->logic();
}

void Effect::superPauseStart(){
}

void Effect::superPauseEnd(){
}
    
int Effect::getSpritePriority() const {
    return spritePriority;
}

bool Effect::isDead(){
    return animation->hasLooped();
}

Effect::~Effect(){
}

class Spark: public Effect {
public:
    Spark(int x, int y, int spritePriority, PaintownUtil::ReferenceCount<MugenAnimation> animation);
    virtual ~Spark();
};

/* FIXME: can sparks be scaled? */
Spark::Spark(int x, int y, int spritePriority, PaintownUtil::ReferenceCount<MugenAnimation> animation):
Effect(NULL, animation, -1, x, y, 1, 1, spritePriority){
}

Spark::~Spark(){
}

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
// console(new Console::Console(CONSOLE_SIZE)),
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
cycles(0),
quake_time(0){
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
                throw MugenException( "Unhandled option in Info Section: " + simple->toString(), __FILE__, __LINE__);
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

static bool matchRegex(const string & str, const string & regex){
    return Util::matchRegex(str, regex);
}

static string regexResult(const string & str, const string & regex){
    return "";
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
    AstRef parsed(Mugen::Util::parseDef(ourDefFile.path()));
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
    camerax = startx;
    cameray = starty;
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
    destroyRaw(effects);
    // sparks = Mugen::Util::loadAnimations(Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath("fightfx.air")), effects);
    sparks = Mugen::Util::loadAnimations(getMotifFile("fightfx.air"), effects, true);

    // Mugen::Util::readSounds(Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath("common.snd")), sounds);
    Mugen::Util::readSounds(getMotifFile("common.snd"), sounds);

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

void Mugen::Stage::destroyRaw(const map< unsigned int, std::map< unsigned int, MugenSprite * > > & sprites){
    for (map< unsigned int, std::map< unsigned int, MugenSprite * > >::const_iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
        for(map< unsigned int, MugenSprite * >::const_iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
            MugenSprite * sprite = j->second;
            sprite->unloadRaw();
        }
    }
}

void Mugen::Stage::setCamera( const double x, const double y ){ 
    camerax = x;
    cameray = y; 
    // Camera boundaries
    if( camerax < boundleft ) camerax = boundleft;
    else if( camerax > boundright )camerax = boundright;
    if( cameray < boundhigh ) cameray = boundhigh;
    else if( cameray > boundlow )cameray = boundlow;
}
void Mugen::Stage::moveCamera( const double x, const double y ){ 
    camerax += x; cameray += y; 
    // Camera boundaries
    if( camerax < boundleft ) camerax = boundleft;
    else if( camerax > boundright )camerax = boundright;
    if( cameray < boundhigh ) cameray = boundhigh;
    else if( cameray > boundlow )cameray = boundlow;
}

static bool anyCollisions(const vector<MugenArea> & boxes1, int x1, int y1, const vector<MugenArea> & boxes2, int x2, int y2){

    for (vector<MugenArea>::const_iterator attack_i = boxes1.begin(); attack_i != boxes1.end(); attack_i++){
        for (vector<MugenArea>::const_iterator defense_i = boxes2.begin(); defense_i != boxes2.end(); defense_i++){
            const MugenArea & attack = *attack_i;
            const MugenArea & defense = *defense_i;
            if (attack.collision(x1, y1, defense, x2, y2)){
                return true;
            }
        }
    }

    return false;

}

static bool anyBlocking(const vector<MugenArea> & boxes1, int x1, int y1, int attackDist, const vector<MugenArea> & boxes2, int x2, int y2){
    for (vector<MugenArea>::const_iterator attack_i = boxes1.begin(); attack_i != boxes1.end(); attack_i++){
        for (vector<MugenArea>::const_iterator defense_i = boxes2.begin(); defense_i != boxes2.end(); defense_i++){
            const MugenArea & attack = *attack_i;
            MugenArea defense = *defense_i;
	    defense.x1 -= attackDist;
	    defense.x2 += attackDist;
            if (attack.collision(x1, y1, defense, x2, y2)){
                return true;
            }
        }
    }

    return false;
}

bool Mugen::Stage::doBlockingDetection(Mugen::Object * obj1, Mugen::Object * obj2){
    return anyBlocking(obj1->getAttackBoxes(), (int) obj1->getX(), (int) obj1->getY(), obj1->getAttackDistance(), obj2->getDefenseBoxes(), (int) obj2->getX(), (int) obj2->getY());
}

bool Mugen::Stage::doCollisionDetection(Mugen::Object * obj1, Mugen::Object * obj2){
    return anyCollisions(obj1->getAttackBoxes(), (int) obj1->getX(), (int) obj1->getY(), obj2->getDefenseBoxes(), (int) obj2->getX(), (int) obj2->getY());
}

PaintownUtil::ReferenceCount<MugenAnimation> Mugen::Stage::getFightAnimation(int id){
    if (sparks[id] == 0){
        ostringstream out;
        out << "No fightfx animation for " << id;
        throw MugenException(out.str(), __FILE__, __LINE__);
    }

    return sparks[id];
}

void Mugen::Stage::addSpark(int x, int y, int sparkNumber){
    if (sparks[sparkNumber] == 0){
        /*
        ostringstream out;
        out << "No spark animation for " << sparkNumber;
        throw MugenException(out.str());
        */
        Global::debug(0) << "No spark animation for " << sparkNumber << endl;
        return;
    }
    /* FIXME: sprite priority */
    Mugen::Spark * spark = new Mugen::Spark(x, y, 0, PaintownUtil::ReferenceCount<MugenAnimation>(sparks[sparkNumber]));
    showSparks.push_back(spark);
}

void Mugen::Stage::addSpark(int x, int y, const PaintownUtil::ReferenceCount<MugenAnimation> & animation){
    if (animation != NULL){
        /* FIXME: sprite priority */
        Mugen::Spark * spark = new Mugen::Spark(x, y, 0, animation);
        showSparks.push_back(spark);
    }
}

void Mugen::Stage::playSound(int group, int item, bool own){
    /* FIXME: handle own */
    MugenSound * sound = sounds[group][item];
    if (sound != 0){
        sound->play();
    }
}

/* for helpers and players */
void Mugen::Stage::physics(Object * mugen){
    /* ignore physics while the player is paused */
    if (mugen->isPaused()){
        return;
    }

    mugen->doMovement(objects, *this);

    if (mugen->getCurrentPhysics() == Mugen::Physics::Stand ||
        mugen->getCurrentPhysics() == Mugen::Physics::Crouch){
        /* friction */
        if (mugen->getY() == 0){
            mugen->setXVelocity(mugen->getXVelocity() * mugen->getGroundFriction());
            if (mugen->getMoveType() == Mugen::Move::Hit && 
                mugen->getXVelocity() < 0 &&
                ticker % 5 == 0){
                /* 120 is small dust */
                addSpark((int)mugen->getX(), (int)(mugen->getRY()), 120);
            }
        }
    }

    if (mugen->getMoveType() == Mugen::Move::Attack && mugen->getHit().isEnabled()){

        for (vector<Mugen::Object*>::iterator enem = objects.begin(); enem != objects.end(); ++enem){
            Mugen::Character * enemy = (Mugen::Character*) *enem;
            if (enemy->getAlliance() != mugen->getAlliance() && enemy->canBeHit(mugen)){
		// Check attack distance to make sure we begin block at the correct distance
                /*
		if (doBlockingDetection(mugen, enemy) && enemy->isBlocking(*mugen->getHit())){
		    enemy->guarded(mugen);
		}
                */

                bool collision = doCollisionDetection(mugen, enemy);
                /* blocking collision extends a little further than a a normal collision */
                bool blockingCollision = doBlockingDetection(mugen, enemy);

                /* guarding */
                if ((collision || blockingCollision) && enemy->isBlocking(mugen->getHit())){
                    if (collision){
                        /* add guard spark and play guard sound */
                        int spark = mugen->getHit().guardSpark;
                        if (spark == -1){
                            spark = mugen->getDefaultGuardSpark();
                        }
                        addSpark((int)(mugen->getHit().sparkPosition.x + enemy->getX()), (int)(mugen->getHit().sparkPosition.y + mugen->getRY()), spark);
                        playSound(mugen->getHit().guardHitSound.group, mugen->getHit().guardHitSound.item, mugen->getHit().guardHitSound.own);
                    }
                    mugen->didHitGuarded(enemy, *this);
                    enemy->guarded(mugen, mugen->getHit());
                    /*
                       vector<string> empty;
                       enemy->changeState(*this, Mugen::StartGuardStand, empty);
                       */
                } else if (collision){
                    /* do hitdef stuff */
                    // Global::debug(0) << "Collision!" << endl;
                    /* the hit state */
                    int spark = mugen->getHit().spark;
                    if (spark == -1){
                        spark = mugen->getDefaultSpark();
                    }
                    addSpark((int)(mugen->getHit().sparkPosition.x + enemy->getX()), (int)(mugen->getHit().sparkPosition.y + mugen->getRY()), spark);
                    playSound(mugen->getHit().hitSound.group, mugen->getHit().hitSound.item, mugen->getHit().hitSound.own);

                    /* order matters here, the guy attacking needs to know that
                     * he hit enemy so the guy can update his combo stuff.
                     */
                    mugen->didHit(enemy, *this);
                    enemy->wasHit(*this, mugen, mugen->getHit());
                    // enemy->changeState(5000);
                }
            }
        }
    }

    for (vector<Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); it++){
        Projectile * projectile = *it;
        if (projectile->getOwner() != mugen && projectile->canCollide()){
            if (anyCollisions(mugen->getDefenseBoxes(), (int) mugen->getX(), (int) mugen->getRY(),
                              projectile->getAttackBoxes(), (int) projectile->getX(), (int) projectile->getY())){
                projectile->doCollision(mugen);
                Mugen::Character * enemy = (Mugen::Character*) mugen;
                mugen->wasHit(*this, projectile->getOwner(), projectile->getHitDefinition());
            }
        }
    }

    // Check collisions
    for (vector<Mugen::Object*>::iterator enem = objects.begin(); enem != objects.end(); ++enem){
        Mugen::Object *enemy = *enem;
        if (mugen->getAlliance() != enemy->getAlliance()){
            // Do stuff for players
            if (isaPlayer(enemy)){
                // He collides with another push him away
                Mugen::Object * mplayer = (Mugen::Object *) mugen;
                Mugen::Object * menemy = (Mugen::Object *) enemy;
                /* TODO: make this cleaner */
                while (anyCollisions(mplayer->getDefenseBoxes(), (int) mplayer->getX(), (int) mplayer->getY(), menemy->getDefenseBoxes(), (int) menemy->getX(), (int) menemy->getY()) && centerCollision(mplayer, menemy) && enemy->getY() == 0 && mplayer->getY() < enemy->getHeight() && menemy->getMoveType() == Mugen::Move::Idle){

                    /* use move*Force to override pos freeze */

                    if (enemy->getX() < mugen->getX()){
                        if (enemy->getX() <= maximumLeft()){
                            /* FIXME */
                            mugen->moveRightForce(0.5);
                        } else {
                            /* FIXME! */
                            enemy->moveLeftForce(0.5);
                        }
                        // enemy->moveLeft( ((Mugen::Character *)player)->getSpeed() );
                    } else if (enemy->getX() > mugen->getX()){
                        if (enemy->getX() >= maximumRight()){
                            /* FIXME */
                            mugen->moveLeftForce(0.5);
                        } else {
                            /* FIXME! */
                            enemy->moveRightForce(0.5);
                        }
                        // enemy->moveRight( ((Mugen::Character *)player)->getSpeed() );
                    } else if (enemy->getX() == mugen->getX()){
                        if (enemy->getX() >= maximumRight()){
                            mugen->moveLeftForce(0.5);
                        } else {
                            mugen->moveRightForce(0.5);
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

vector<Mugen::Object*> Mugen::Stage::getOpponents(Mugen::Object * who){
    vector<Mugen::Object*> out;
    for (vector<Mugen::Object*>::iterator it = objects.begin(); it != objects.end(); ++it){
        Mugen::Object * player = *it;
        if (isaPlayer(player) && player->getAlliance() != who->getAlliance()){
            out.push_back(player);
        }
    }

    return out;
}

void Mugen::Stage::unbind(Mugen::Object * what){
    for (vector<Mugen::Object*>::iterator it = objects.begin(); it != objects.end(); ++it){
        Mugen::Object * guy = *it;
        guy->unbind(what);
    }
}

/* A main cycle of the game */
void Mugen::Stage::runCycle(){
    if (paletteEffects.time > 0){
        paletteEffects.time = 0;
        paletteEffects.counter += 1;
    }

    // camera crap
    if (quake_time > 0){
        quake_time--;
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

    for (vector<Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); /**/){
        Projectile * projectile = *it;
        projectile->logic();

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
    ticker++;

    const double diffx = startx - camerax;
    const double diffy = starty - cameray;

    if (environmentColor.time > 0){
        environmentColor.time -= 1;
    }

    if (superPause.time > 0){
        superPause.time -= 1;
        if (superPause.time == 0){
            for (vector<Mugen::Effect*>::iterator it = showSparks.begin(); it != showSparks.end(); it++){ 
                Mugen::Effect * effect = *it;
                effect->superPauseEnd();
            }
        }
    } else {
        background->act();

        // Players go in here
        std::vector<Mugen::Object *> add;
        addedObjects.clear();
        for (vector<Mugen::Object*>::iterator it = objects.begin(); it != objects.end(); /**/ ){
            bool next = true;
            /* use local variables more often, iterators can be easily confused */
            Mugen::Object * player = *it;
            player->act(&objects, this, &add);
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

        objects.insert(objects.end(), add.begin(), add.end());
        objects.insert(objects.end(), addedObjects.begin(), addedObjects.end());
    }

}

void Mugen::Stage::logic(){
    // Console::ConsoleEnd & cend = Console::Console::endl;

    /* cycles slow the stage down, like after ko */
    cycles += 1;
    if (cycles >= 1 / gameRate){
        cycles = 0;
        runCycle();
    }
    
    // Correct camera
    if ((verticalfollow > 0) && !inabove && (getCameraY() < 0)){
        /* FIXME: where did 3.2 come from? */
	moveCamera(0, verticalfollow * 3.2);
    }
    
    // Player HUD Need to make this more ellegant than casting and passing from array
    gameHUD->act(*this, *((Mugen::Character *)players[0]),*((Mugen::Character *)players[1]));
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

/* Returns a sorted listed of sprite priorties */
vector<int> Mugen::Stage::allSpritePriorities(){
    vector<int> priorities;

    for (vector<Mugen::Object*>::iterator it = objects.begin(); it != objects.end(); it++){
        Mugen::Object * object = *it;
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
    std::unique(priorities.begin(), priorities.end());

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

    if (environmentColor.time == 0){
        if (paletteEffects.time > 0){
            drawBackgroundWithEffects((int) camerax, (int) cameray, *work);
        } else {
            background->renderBackground((int) camerax, (int) cameray, *work);
        }
    } else if (environmentColor.under){
        /* FIXME: I'm not exactly sure where the environment color is supposed to go.
         * After the super pause? After the background hud elements?
         */
        work->fill(environmentColor.color);
    }

    /* darken the background */
    if (superPause.time > 0){
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
        for (vector<Mugen::Object*>::iterator it = objects.begin(); it != objects.end(); it++){
            Mugen::Object *obj = *it;

            if (obj->getSpritePriority() == *spritePriority){
                /* Reflection */
                /* FIXME: reflection and shade need camerax/y */
                if (reflectionIntensity > 0){
                    obj->drawReflection(work, (int)(camerax - DEFAULT_WIDTH / 2), (int) cameray, reflectionIntensity);
                }

                /* Shadow */
                obj->drawMugenShade(work, (int)(camerax - DEFAULT_WIDTH / 2), shadowIntensity, shadowColor, shadowYscale, shadowFadeRangeMid, shadowFadeRangeHigh);

                /* draw the player */
                obj->draw(work, (int)(camerax - DEFAULT_WIDTH / 2), (int) cameray);
            }
        }

        for (vector<Mugen::Effect*>::iterator it = showSparks.begin(); it != showSparks.end(); it++){
            Mugen::Effect * spark = *it;
            if (spark->getSpritePriority() == *spritePriority){
                spark->draw(*work, (int) (camerax - DEFAULT_WIDTH / 2), (int) cameray);
            }
        }

        for (vector<Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); it++){
            Projectile * projectile = *it;
            if (projectile->getSpritePriority() == *spritePriority){
                projectile->draw(*work, camerax - DEFAULT_WIDTH / 2, cameray);
            }
        }

    }

    if (environmentColor.time > 0 && !environmentColor.under){
        work->fill(environmentColor.color);
    }

    //! Render layer 1 HUD
    gameHUD->render(Mugen::Element::Foreground, *work);

    if (environmentColor.time == 0){
        if (paletteEffects.time > 0){
            drawForegroundWithEffects((int) camerax, (int) cameray, *work);
        } else {
            background->renderForeground((int) camerax, (int) cameray, *work);
        }
    }
    
    //! Render layer 2 HUD
    gameHUD->render(Mugen::Element::Top, *work);

    // Player debug
    for (vector<Mugen::Object*>::iterator it = objects.begin(); it != objects.end(); it++){
	if (isaPlayer(*it)){
            Mugen::Character *character = (Mugen::Character*)*it;
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

void Mugen::Stage::reset(){
    camerax = startx;
    cameray = starty;
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
    for (vector<Mugen::Object*>::iterator it = objects.begin(); it != objects.end();){
        Mugen::Object *player = *it;

        /* remove any non-player objects, like projectiles or helpers */
        if (!isaPlayer(player)){
            player->destroyed(*this);
            // unbind(player);
            delete player;
            it = objects.erase(it);
        } else {
            Mugen::Character * character = (Mugen::Character*) player;
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
    
    inleft = inright = onLeftSide = onRightSide = 0;
    if (players.size() < 2){
        throw MugenException("Need at least 2 players", __FILE__, __LINE__);
    }

    if (gameHUD == NULL){
        throw MugenException("Internal error: Stage not loaded, call load()", __FILE__, __LINE__);
    }

    gameHUD->reset(*this, *((Mugen::Character *)players[0]),*((Mugen::Character *)players[1]));
}

std::vector<Mugen::Object *> Mugen::Stage::getPlayers() const {
    return players;
}

// Add player1 people
void Mugen::Stage::addPlayer1( Mugen::Object * o ){
    o->setAlliance(Player1Side);
    o->setX(p1startx);
    o->setY(p1starty);
    o->setZ(currentZOffset());
    o->setFacing(FacingRight);
    objects.push_back(o);
    players.push_back(o);

    playerInfo[o].oldx = o->getX();
    playerInfo[o].oldy = o->getY();
    playerInfo[o].leftTension = false;
    playerInfo[o].rightTension = false;
    playerInfo[o].leftSide = false;
    playerInfo[o].rightSide = false;
    playerInfo[o].jumped = false;

    ((Mugen::Character *) o)->setCommonSounds(&sounds);
}

// Add player2 people
void Mugen::Stage::addPlayer2(Mugen::Object * o ){
    o->setAlliance(Player2Side);
    o->setX(p2startx);
    o->setY(p2starty);
    o->setZ(currentZOffset());
    o->setFacing(FacingLeft);
    objects.push_back(o);
    players.push_back(o);
    
    playerInfo[o].oldx = o->getX();
    playerInfo[o].oldy = o->getY();
    playerInfo[o].leftTension = false;
    playerInfo[o].rightTension = false;
    playerInfo[o].leftSide = false;
    playerInfo[o].rightSide = false;
    playerInfo[o].jumped = false;

    ((Mugen::Character *) o)->setCommonSounds(&sounds);
}

void Mugen::Stage::setPlayerHealth(int health){
    for ( vector<Mugen::Object * >::iterator it = players.begin(); it != players.end(); it++ ){
        Mugen::Character *player = (Mugen::Character *)(*it);
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
    for (vector<Mugen::Object *>::iterator it = players.begin(); it != players.end(); it++, count++){
        Mugen::Character *player = (Mugen::Character *)(*it);
        if (choose == count){
            player->enableDebug();
        } else {
            player->disableDebug();
        }
    }
}

void Mugen::Stage::draw( Graphics::Bitmap * work ){
    render(work);
}

void Mugen::Stage::addObject(Mugen::Object * o){
    addedObjects.push_back(o);
}

bool Mugen::Stage::finished() const { return false; }
void Mugen::Stage::reloadLevel() throw( LoadException ){ 
    cleanup();
    loaded = false;
    load(); 
}
/* upper left hand corner of the screen */
int Mugen::Stage::getX(){
    return (int)getCameraX();
}

int Mugen::Stage::getY(){
    return (int)getCameraY();
}

/* bleh.. */
Mugen::Object * Mugen::Stage::findObject(int id){ 
    for (vector<Mugen::Object*>::iterator it = objects.begin(); it != objects.end(); it++){
        Mugen::Object * object = *it;
        if (object->getObjectId() == id){
            return object;
        }
    }
    return NULL;
}

// These should be the same, but we'll see, mugen has some funny parameters
int Mugen::Stage::getMaximumZ(){ return zoffset; }
int Mugen::Stage::getMinimumZ(){ return zoffset; }

void Mugen::Stage::begin(){
}

const std::string Mugen::Stage::getStageName(const std::string &filename) throw (MugenException){
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
    
    AstRef parsed(Mugen::Util::parseDef(defFile.path()));
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

        for (Mugen::SpriteMap::iterator it1 = effects.begin(); it1 != effects.end(); it1++){
            for (Mugen::GroupMap::iterator it2 = (*it1).second.begin(); it2 != (*it1).second.end(); it2++){
                MugenSprite * sprite = (*it2).second;
                delete sprite;
            }
        }
        effects.clear();

        sparks.clear();

        for (vector<Mugen::Effect*>::iterator it = showSparks.begin(); it != showSparks.end(); it++){
            delete *it;
        }
        showSparks.clear();

        for (map<unsigned int, map<unsigned int, MugenSound*> >::iterator it1 = sounds.begin(); it1 != sounds.end(); it1++){
            map<unsigned int, MugenSound*> & group = (*it1).second;
            for (map<unsigned int, MugenSound*>::iterator it2 = group.begin(); it2 != group.end(); it2++){
                delete (*it2).second;
            }
        }
        sounds.clear();

        for (vector<Mugen::Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); it++){
            delete *it;
        }
        projectiles.clear();

        for (vector<Mugen::Object*>::iterator it = objects.begin(); it != objects.end(); /**/){
            Mugen::Object * object = *it;

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

bool Mugen::Stage::isaPlayer(Mugen::Object * o) const {
    for (vector<Mugen::Object *>::const_iterator it = players.begin(); it != players.end(); it++ ){
        if ( (*it) == o ){
            return true;
        }
    }
    return false;
}

int Mugen::Stage::maximumRight() const {
    return (int)(camerax + DEFAULT_WIDTH / 2);
}

int Mugen::Stage::maximumLeft() const {
    return (int)(camerax - DEFAULT_WIDTH / 2);
}
    
int Mugen::Stage::maximumUp() const {
    return cameray;
}

int Mugen::Stage::maximumDown() const {
    return cameray + DEFAULT_HEIGHT;
}
    
void Mugen::Stage::addProjectile(Projectile * projectile){
    projectiles.push_back(projectile);
}

void Mugen::Stage::updatePlayer(Mugen::Object * player){
    // Z/Y offset
    player->setZ(currentZOffset());

    // Move X and Camera
    const double px = player->getX();
    const double py = player->getY();
    const double pdiffx = px - playerInfo[player].oldx;
    /*
       const double screenLeft = camerax - DEFAULT_WIDTH / 2;
       const double screenRight = camerax + DEFAULT_WIDTH / 2;
       */
    const double screenLeft = maximumLeft();
    const double screenRight = maximumRight();

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
            inleft++;
        }
    } else if (px >= rightTension){
        if (!playerInfo[player].rightTension){
            playerInfo[player].rightTension = true;
            inright++;
        }
    } 

    // Release tension
    if (px > leftTension){
        if (playerInfo[player].leftTension){
            playerInfo[player].leftTension = false;
            inleft--;
        }
    }
    // Release tension
    if (px < rightTension){
        if (playerInfo[player].rightTension){
            playerInfo[player].rightTension = false;
            inright--;
        }
    }

    // Check Screen sides
    if (px <= screenLeft){
        player->setX(screenLeft);
        playerInfo[player].oldx = px;
        if (!playerInfo[player].leftSide){
            playerInfo[player].leftSide = true;
            onLeftSide++;
        }
    } else if (px >= screenRight){
        player->setX(screenRight);
        playerInfo[player].oldx = px;
        if (!playerInfo[player].rightSide){
            playerInfo[player].rightSide = true;
            onRightSide++;
        }
    } 
    // Release side
    if (px > screenLeft){
        if (playerInfo[player].leftSide){
            playerInfo[player].leftSide = false;
            onLeftSide--;
        }
    }
    // Release side
    if (px < screenRight){
        if (playerInfo[player].rightSide){
            playerInfo[player].rightSide = false;
            onRightSide--;
        }
    }

    //Global::debug(0) << "Left Tension: " << inleft << " | Right Tension: "<< inright << endl;
    //Global::debug(0) << "Left Screen Edge: " << onLeftSide << " | Right Screen Edge: "<< onRightSide << endl;

    if (playerInfo[player].leftTension){
        if (pdiffx < 0){
            if (!onRightSide){
                moveCamera(pdiffx,0);
            }
        } else if (pdiffx > 0){
            if (inright){
                moveCamera(pdiffx,0);
            }
        }
    } else if (playerInfo[player].rightTension){
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
    if (playerInfo[player].oldy != py){
        if (verticalfollow > 0){
            const double pdiffy = playerInfo[player].oldy - py;
            if (py > floortension){
                if (!playerInfo[player].above){
                    playerInfo[player].above = true;
                    inabove++;
                }
            } else if ( playerInfo[player].above){
                playerInfo[player].above = false;
                inabove--;
            }
            if (playerInfo[player].above && pdiffy < 0){
                moveCamera( 0, verticalfollow * -3.2 );
            } else if (playerInfo[player].above && pdiffy > 0){
                moveCamera( 0, verticalfollow * 3.2 );
            }
        }
    }
    //Global::debug(1) << "Our players Y: " << py << " | Above: "<< playerInfo[o].above << " | total inabove: " << inabove << endl;
}

void Mugen::Stage::initializeName(){
    try{
#if 0
        Filesystem::AbsolutePath str = this->location;
	// Lets look for our def since some people think that all file systems are case insensitive
	baseDir = Filesystem::find(Filesystem::RelativePath("mugen/stages/"));
	Global::debug(1) << baseDir.path() << endl;

        /* FIXME: this is ugly */
	if (str.path().find(".def") == std::string::npos){
	    str = Filesystem::AbsolutePath(str.path() + ".def");
	}

	// Get correct directory
	baseDir = baseDir.join(str).getDirectory();
	// str = str.getFilename();
	const Filesystem::AbsolutePath ourDefFile = Mugen::Util::getCorrectFileLocation(baseDir, Filesystem::RelativePath(str.getFilename()).path());
#endif
	// Set name of map
	name = Mugen::Util::probeDef(location, "info", "name");
    } catch (const MugenException &ex){
	Global::debug(1) << "Couldn't find the name of the map!" << endl;
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
    Filesystem::AbsolutePath systemFile = Mugen::Data::getInstance().getFileFromMotif(Mugen::Data::getInstance().getMotif());
    
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

    MugenFont font(Mugen::Util::findFile(Filesystem::RelativePath(fontFile)));
    
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
        default:
            break;
    }
    

    double gameSpeed = 1.0;
    double runCounter = 0;

    // Put character in continue state
    std::vector<std::string> empty;
    character->changeState(*this, Mugen::Continue, empty);

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
            vector<InputMap<Mugen::Keys>::InputEvent> out = InputManager::getEvents(input, InputSource());
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
            std::vector<Mugen::Object *> add;
            character->act(&add, stage, &add);
        }

        double ticks(double system){
            return Util::gameTicks(system);
        }
    };

    class Draw: public PaintownUtil::Draw {
    public:
        Draw(Mugen::Background * background, int reflectionIntensity, Mugen::Character * character, double cameray, int shadowIntensity, Graphics::Color shadowColor, double shadowYscale, int shadowFadeRangeHigh, int shadowFadeRangeMid, MugenFont & font, const Logic & logic):
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
    
        MugenFont & font;
        const Logic & logic;

        void draw(const Graphics::Bitmap & screen){
            Graphics::StretchedBitmap board(320, 240, screen);
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
            screen.BlitToScreen();
        }
    };

    Logic logic(input, character, this);
    Draw draw(background, reflectionIntensity, character, cameray, shadowIntensity, shadowColor, shadowYscale, shadowFadeRangeHigh, shadowFadeRangeMid, font, logic);

    PaintownUtil::standardLoop(logic, draw);

    return logic.getAnswer();
}
    
Mugen::Character * Mugen::Stage::getEnemy(const Mugen::Character * who) const {
    for (vector<Mugen::Object*>::const_iterator enem = objects.begin(); enem != objects.end(); ++enem){
        Mugen::Object * enemy = *enem;
        if (who->getAlliance() != enemy->getAlliance() && isaPlayer(enemy)){
            return (Mugen::Character*) enemy;
        }
    }

    return NULL;
}

int Mugen::Stage::getGameTime() const {
    if (gameHUD){
        return gameHUD->getGameTime();
    }
    return 0;
}
    
void Mugen::Stage::doSuperPause(int time, Character & guy, int animation, bool ownAnimation, int positionX, int positionY){
    superPause.time = time;

    for (vector<Mugen::Effect*>::iterator it = showSparks.begin(); it != showSparks.end(); it++){ 
        Mugen::Effect * effect = *it;
        effect->superPauseStart();
    }

    if (animation != -1){
        if (ownAnimation){
            PaintownUtil::ReferenceCount<MugenAnimation> use = guy.getAnimation(animation);
            if (use != NULL){
                addSpark(positionX, positionY, use);
            }
        } else {
            addSpark(positionX, positionY, animation);
        }
    }
}
   
void Mugen::Stage::doPause(int time, int buffer, int moveAllowed, bool pauseBackground){
    /* TODO */
}
    
void Mugen::Stage::createDust(int x, int y){
    addSpark(x, y, 120);
}
        
void Mugen::Stage::addEffect(Mugen::Effect * effect){
    showSparks.push_back(effect);
}
                    
int Mugen::Stage::countMyEffects(const Mugen::Character * owner) const {
    int total = 0;
    for (vector<Mugen::Effect*>::const_iterator it = showSparks.begin(); it != showSparks.end(); it++){ 
        Mugen::Effect * effect = *it;
        if (effect->getOwner() == owner){
            total += 1;
        }
    }
    return total;
}
    
int Mugen::Stage::countMyHelpers(const Mugen::Character * owner) const {
    int count = 0;
    for (vector<Mugen::Object*>::const_iterator it = objects.begin(); it != objects.end(); it++){
        /* FIXME! dont assume its a character */
        Mugen::Character * who = (Mugen::Character*) *it;
        if (who->isHelper()){
            Mugen::Helper * helper = (Mugen::Helper*) who;
            if (helper->getParent() == owner){
                count += 1;
            }
        }
    }
    return count;
}

vector<Mugen::Character *> Mugen::Stage::getTargets(int id, const Mugen::Character * from) const {
    vector<Mugen::Character *> targets;
    if (id == -1){
        /* TODO: match all enemies */
    } else {
        Mugen::Object * target = from->getTargetId(id);
        if (target != NULL){
            /* FIXME: dont assume its a character */
            targets.push_back((Mugen::Character*) target);
        }
    }
    return targets;
}
    
/* Set the background to a solid color for some length of time */
void Mugen::Stage::setEnvironmentColor(Graphics::Color color, int time, bool under){
    environmentColor.time = time;
    environmentColor.under = under;
    environmentColor.color = color;
}

void Mugen::Stage::removeHelper(Mugen::Character * who){
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
        helper->reParent(NULL);
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

vector<Mugen::Helper*> Mugen::Stage::findHelpers(const Mugen::Character * owner, int id) const {
    vector<Mugen::Helper*> out;
    for (vector<Mugen::Object*>::const_iterator it = objects.begin(); it != objects.end(); it++){
        /* FIXME! dont assume its a character */
        Mugen::Character * who = (Mugen::Character*) *it;
        if (who->isHelper()){
            Mugen::Helper * helper = (Mugen::Helper*) who;
            if (helper->getHelperId() == id && helper->getParent() == owner){
                out.push_back(helper);
            }
        }
    }
    return out;
}

vector<Mugen::Helper*> Mugen::Stage::findHelpers(const Mugen::Character * owner) const {
    vector<Mugen::Helper*> out;
    for (vector<Mugen::Object*>::const_iterator it = objects.begin(); it != objects.end(); it++){
        /* FIXME! dont assume its a character */
        Mugen::Character * who = (Mugen::Character*) *it;
        if (who->isHelper()){
            Mugen::Helper * helper = (Mugen::Helper*) who;
            if (helper->getParent() == owner){
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
    quake_time += q;
}
