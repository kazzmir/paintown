#include "mugen_storyboard.h"

#include "util/bitmap.h"
#include "init.h"
#include "resource.h"
#include "util/funcs.h"
#include "game/console.h"
#include "object/animation.h"
#include "object/object.h"
#include "object/character.h"
#include "object/object_attack.h"
#include "object/player.h"
#include "globals.h"
#include "factory/font_render.h"

#include "mugen_animation.h"
#include "mugen/background.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"
#include "mugen_font.h"

#include "util/timedifference.h"
#include "ast/all.h"
#include "parser/all.h"

#include "gui/keyinput_manager.h"

namespace PaintownUtil = ::Util;

using namespace std;

static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;
static const int DEFAULT_SCREEN_X_AXIS = 160;
static const int DEFAULT_SCREEN_Y_AXIS = 0;

MugenLayer::MugenLayer():
actionno(-1),
startTime(0),
animation(0){
    offset.x = 0;
    offset.y = 0;
}

MugenLayer::~MugenLayer(){
}

void MugenLayer::act(){
    if (actionno != -1){
        animation->logic();
    }
}

void MugenLayer::draw(const int xaxis, const int yaxis, Bitmap *bmp){
    if (actionno != -1){
        animation->render(xaxis + offset.x, yaxis + offset.y, *bmp);
    }
}

void MugenLayer::reset(){
    if (actionno != -1){
	animation->reset();
    }
}
	
MugenScene::MugenScene():
clearColor(-2),
ticker(0),
endTime(0),
backgroundName(""),
background(0){
    for (int i = 0; i < 10; ++i){
	// initiate the layers
	MugenLayer *layer = new MugenLayer();
	layers.push_back(layer);
    }
    defaultAxis.x = -1;
    defaultAxis.y = -1;
    fader.setState(FADEIN);
}

MugenScene::~MugenScene(){

    if (background){
        delete background;
    }

    // layers
    for ( std::vector< MugenLayer *>::iterator i = layers.begin(); i != layers.end(); ++i ){
        if ((*i))delete (*i);
    }
}

void MugenScene::act(){
    // backgrounds
    if (background){
        background->act();
    }
    // layers
    for ( std::vector< MugenLayer *>::iterator i = layers.begin(); i != layers.end(); ++i ){
        MugenLayer *layer = *i;
        if(ticker >= layer->startTime)layer->act();
    }
    // Fader
    fader.act();
    if (ticker == endTime - fader.getFadeOutTime()){
        fader.setState(FADEOUT);
    }
    // tick tick
    ticker++;
}
void MugenScene::draw(Bitmap *bmp){
    if (clearColor != -1 && clearColor != -2){
        bmp->fill(clearColor);
    }
    // backgrounds
    if (background){
        background->renderBackground(0,0, *bmp);
    }
    // layers
    for (std::vector< MugenLayer *>::iterator i = layers.begin(); i != layers.end(); ++i ){
        MugenLayer *layer = *i;
        layer->draw(defaultAxis.x,defaultAxis.y,bmp);
    }
    // foregrounds
    if (background){
        background->renderForeground(0,0, *bmp);
    }
    // fader
    fader.draw(bmp);
}

bool MugenScene::isDone(){
    return (ticker >= endTime);
}

void MugenScene::reset(){
    ticker = 0;
    fader.setState(FADEIN);
    // layers
    for (std::vector< MugenLayer *>::iterator i = layers.begin(); i != layers.end(); ++i ){
	MugenLayer *layer = *i;
	layer->reset();
    }
}

MugenStoryboard::MugenStoryboard( const string & s ):
location(s),
spriteFile(""),
startscene(0){
}

MugenStoryboard::~MugenStoryboard(){
    cleanup();
}

void MugenStoryboard::load() throw (MugenException){
    // Lets look for our def since some people think that all file systems are case insensitive
    std::string baseDir = Mugen::Util::getFileDir(location);
    const std::string ourDefFile = Mugen::Util::fixFileName( baseDir, Mugen::Util::stripDir(location) );
    // get real basedir
    //baseDir = Mugen::Util::getFileDir( ourDefFile );
    Global::debug(1) << baseDir << endl;

    if (ourDefFile.empty()){
        throw MugenException("Cannot locate storyboard definition file for: " + location);
    }

    std::string filesdir = "";

    Global::debug(1) << "Got subdir: " << filesdir << endl;

    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Def::main(ourDefFile));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + ourDefFile + " in" + diff.printTime("") << endl;

    /* set by bg.name = "foo" */
    string bgname;

    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
        /* this should really be head = Mugen::Util::fixCase(head) */
	head = Mugen::Util::fixCase(head);

        // Global::debug(1) << "Name: " << head << endl;
        if (head == "info"){
            class InfoWalk: public Ast::Walker{
            public:
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "name"){
                        string name;
                        simple >> name;
                        Global::debug(1) << "Read name '" << name << "'" << endl;
                    } else if (simple == "author"){
                        string name;
                        simple >> name;
                        Global::debug(1) << "Made by: '" << name << "'" << endl;
                    } else {
                        Global::debug(0) << "Warning: ignored attribute: " << simple.toString() << endl;
                    }
                }
            };

            InfoWalk walk;
            section->walk(walk);

        } else if (head == "scenedef"){
            class SceneWalk: public Ast::Walker{
            public:
                SceneWalk(const string & baseDir, MugenStoryboard & board):
                    baseDir(baseDir),
                    board(board){
                }

                const string & baseDir;
                MugenStoryboard & board;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "spr"){
                        simple >> board.spriteFile;
                        Mugen::Util::readSprites(Mugen::Util::getCorrectFileLocation(this->baseDir, board.spriteFile), "", board.sprites);
                    } else if (simple == "startscene"){
                        simple >> board.startscene;
                        Global::debug(1) << "Starting storyboard at: '" << board.startscene << "'" << endl;
                    } else {
                        Global::debug(0) << "Warning: ignored attribute: " << simple.toString() << endl;
                    }
                }
            };

            SceneWalk walk(baseDir, *this);
            section->walk(walk);
        } else if (PaintownUtil::matchRegex(head, "^scene")){
            class SceneWalker: public Ast::Walker{
            public:
                SceneWalker():
                scene(new MugenScene){
                }

                MugenScene * scene;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "fadein.time"){
                        int time;
                        simple >> time;
                        scene->fader.setFadeInTime(time);
                    } else if (simple == "fadein.col"){
                        int r,g,b;
                        simple >> r >> g >> b;
                        scene->fader.setFadeInColor(Bitmap::makeColor(r,g,b));
                    } else if (simple == "fadeout.time"){
                        int time;
                        simple >> time;
                        scene->fader.setFadeOutTime(time);
                    } else if (simple == "fadeout.col"){
                        int r,g,b;
                        simple >> r >> g >> b;
                        scene->fader.setFadeOutColor(Bitmap::makeColor(r,g,b));
                    } else if (simple == "bg.name"){
                        simple >> scene->backgroundName;
                    } else if (simple == "clearcolor"){
                        int r,g,b;
                        simple >> r >> g >> b;
                        scene->clearColor = (r == -1 ? r : Bitmap::makeColor(r,g,b));
                    } else if (simple == "end.time"){
                        simple >> scene->endTime;
                    } else if (simple == "layerall.pos"){
                        simple >> scene->defaultAxis.x;
                        simple >> scene->defaultAxis.y;
                    } else if (PaintownUtil::matchRegex(simple.idString(), "layer[0-9]\\.anim")){
                        int num = atoi(PaintownUtil::captureRegex(simple.idString(), "layer([0-9])\\.anim", 0).c_str());
                        if (num > 0 && num < 10){
                            simple >> scene->layers[num]->actionno;
                        }
                    } else if (PaintownUtil::matchRegex(simple.idString(), "layer[0-9]\\.offset")){
                        int num = atoi(PaintownUtil::captureRegex(simple.idString(), "layer([0-9])\\.offset", 0).c_str());
                        if (num > 0 && num < 10){
                            simple >> scene->layers[num]->offset.x;
                            simple >> scene->layers[num]->offset.y;
                        }
                    } else if (PaintownUtil::matchRegex(simple.idString(), "layer[0-9]\\.starttime")){
                        int num = atoi(PaintownUtil::captureRegex(simple.idString(), "layer([0-9])\\.starttime", 0).c_str());
                        if (num > 0 && num < 10){
                            simple >> scene->layers[num]->startTime;
                        }
                    } else if (simple == "bgm"){
                        // do nothing
                    } else if (simple == "bgm.loop"){
                        // do nothing
                    } else {
                        throw MugenException( "Unhandled option in Scene Section: " + simple.toString());
                    }
                }
            };

            SceneWalker walker;
            section->walk(walker);
            scenes.push_back(walker.scene);
            bgname = walker.scene->backgroundName;
            bgname.erase(std::remove(bgname.begin(), bgname.end(), ' '), bgname.end());
            bgname = Mugen::Util::fixCase(bgname);
            Global::debug(1) << "Got Scene number: '" << scenes.size() - 1 << "' bgname is '" << bgname << "'" << endl;
        } else if (PaintownUtil::matchRegex(head, "begin action")){
            int h = atoi(PaintownUtil::captureRegex(head, "begin action *([0-9]+)", 0).c_str());
            animations[h] = Mugen::Util::getAnimation(section, sprites);
        } else if (PaintownUtil::matchRegex(head, bgname + "def$")){
            Global::debug(1) << "Checking def!" << endl;
            if (scenes.back()){
                MugenScene *scene = scenes.back();
                /*
                std::string name = collection[i]->getHeader();
                name = Mugen::Util::fixCase(name);
                Global::debug(1) << "Checking for background: " << scene->backgroundName << " in Head: " << name << endl;
                */
                Mugen::Background *manager = new Mugen::Background(ourDefFile, bgname);
                scenes.back()->background = manager;
            }
        } //else throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
    }

    // position
    Mugen::Point positionFix;
    positionFix.x = 0;
    positionFix.y = 0;
    // ClearColor
    int clearColorFix = -1;
    int counter = 0;
    // Set up the animations for those that have action numbers assigned (not -1 )
    // Also do their preload
    for( std::vector< MugenScene * >::iterator s = scenes.begin(); s != scenes.end(); ++s ){
        MugenScene *scene = *s;
        // layers
        for( std::vector< MugenLayer *>::iterator i = scene->layers.begin(); i != scene->layers.end(); ++i ){
            MugenLayer *layer = *i;
            if (layer->actionno != -1){
                layer->animation = animations[ layer->actionno ];
            }
        }
        // Positioning
        if( scene->defaultAxis.x == -1 && scene->defaultAxis.y == -1 ){
            scene->defaultAxis.x = positionFix.x;
            scene->defaultAxis.y = positionFix.y;
        }
        // Clear colors
        // Not assigned set to previous scene
        if (counter > 0 && scene->clearColor == -2){
            scene->clearColor = clearColorFix;
        }
        counter++;
        // set position
        positionFix.x = scene->defaultAxis.x;
        positionFix.y = scene->defaultAxis.y;
        // Set to previous scene
        clearColorFix = scene->clearColor;
    }
}

void MugenStoryboard::run(Bitmap *bmp, bool repeat){
    double gameSpeed = 1.0;
    double runCounter = 0;
    bool quit = false;

    Bitmap work( DEFAULT_WIDTH, DEFAULT_HEIGHT );

    for( std::vector< MugenScene * >::iterator i = scenes.begin() ; i != scenes.end() ; ++i ){
        if( (*i) ){
            (*i)->reset();
        }
    }

    std::vector< MugenScene * >::iterator sceneIterator = scenes.begin() + startscene;

    while( !quit ){
        bool draw = false;

        MugenScene *scene = *sceneIterator;
        if ( Global::speed_counter > 0 ){

            runCounter += Global::speed_counter * gameSpeed * Global::LOGIC_MULTIPLIER;//(double) 90 / (double) 60;
            while (runCounter > 1){
                scene->act();
                runCounter -= 1;
                draw = true;
                if( keyInputManager::keyState(keys::ESC, true)){
                    quit = true;
                    return;
                }
                if (keyInputManager::keyState(keys::ENTER, true)){
                    quit = true;
                    return;
                }
                if (scene->isDone()){
                    sceneIterator++;
                    if (sceneIterator == scenes.end()){
                        if (repeat){
                            sceneIterator = scenes.begin();
                        } else {
                            return;
                        }
                    }
                    scene = *sceneIterator;
                    scene->reset();
                }
            }
            Global::speed_counter = 0;
        }

        if (draw){
            scene->draw(&work);
            work.Stretch(*bmp);
            if (Global::getDebug() > 0){
                Font::getDefaultFont().printf( 15, 310, Bitmap::makeColor(0,255,128), *bmp, "Scene: Time(%i) : EndTime(%i) : Fade in(%i) : Fade out(%i)",0, scene->ticker,scene->endTime,scene->fader.getFadeInTime(),scene->fader.getFadeOutTime() );
            }
            bmp->BlitToScreen();
        }

        while (Global::speed_counter == 0){
            Util::rest(1);
            keyInputManager::update();
        }
    }
}

void MugenStoryboard::cleanup(){

    // Get rid of animation lists;
    for( std::map< int, MugenAnimation * >::iterator i = animations.begin() ; i != animations.end() ; ++i ){
        if( i->second )delete i->second;
    }

    // Get rid of scene lists;
    for( std::vector< MugenScene * >::iterator i = scenes.begin() ; i != scenes.end() ; ++i ){
        if( (*i) )delete (*i);
    }
    // sprites
    for( std::map< unsigned int, std::map< unsigned int, MugenSprite * > >::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
        for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
            if( j->second )delete j->second;
        }
    }

}
