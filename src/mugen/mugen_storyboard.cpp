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
#include "mugen_background.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"
#include "mugen_font.h"

#include "gui/keyinput_manager.h"

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
        background->logic(0,0,0,0);
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
        background->renderBack(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT, bmp);
    }
    // layers
    for (std::vector< MugenLayer *>::iterator i = layers.begin(); i != layers.end(); ++i ){
        MugenLayer *layer = *i;
        layer->draw(defaultAxis.x,defaultAxis.y,bmp);
    }
    // foregrounds
    if (background){
        background->renderFront(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT, bmp);
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
    // Lets look for our def since some assholes think that all file systems are case insensitive
    std::string baseDir = Mugen::Util::getFileDir(location);
    const std::string ourDefFile = Mugen::Util::fixFileName( baseDir, Mugen::Util::stripDir(location) );
    // get real basedir
    //baseDir = Mugen::Util::getFileDir( ourDefFile );
    Global::debug(1) << baseDir << endl;

    if( ourDefFile.empty() )throw MugenException( "Cannot locate storyboard definition file for: " + location );

    std::string filesdir = "";

    Global::debug(1) << "Got subdir: " << filesdir << endl;

    MugenReader reader( ourDefFile );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();

    /* Extract info for our first section of our stage */
    for( unsigned int i = 0; i < collection.size(); ++i ){
        std::string head = collection[i]->getHeader();
        Mugen::Util::fixCase(head);
        Global::debug(1) << "Name: " << head << endl;
        if( head == "info" ){
            while( collection[i]->hasItems() ){
                MugenItemContent *content = collection[i]->getNext();
                const MugenItem *item = content->getNext();
                std::string itemhead = item->query();
                Mugen::Util::removeSpaces(itemhead);
                if ( itemhead.find("name")!=std::string::npos ){
                    std::string temp;
                    *content->getNext() >> temp;
                    Global::debug(1) << "Read name '" << temp << "'" << endl;
                } else if ( itemhead.find("author")!=std::string::npos ){
                    std::string temp;
                    *content->getNext() >> temp;
                    Global::debug(1) << "Made by: '" << temp << "'" << endl;
                } else throw MugenException( "Unhandled option in Info Section: " + itemhead );
            }
        }
        else if( head == "scenedef" ){
            while( collection[i]->hasItems() ){
                MugenItemContent *content = collection[i]->getNext();
                const MugenItem *item = content->getNext();
                std::string itemhead = item->query();
                Mugen::Util::removeSpaces(itemhead);
                Mugen::Util::fixCase(itemhead);
                if ( itemhead.find("spr")!=std::string::npos ){
                    *content->getNext() >> spriteFile;
                    Global::debug(1) << "Got Sprite File: '" << spriteFile << "'" << endl;
                    Mugen::Util::readSprites( Mugen::Util::getCorrectFileLocation(baseDir, spriteFile), "", sprites );
                } else if ( itemhead.find("startscene")!=std::string::npos ){
                    *content->getNext() >> startscene;
                    Global::debug(1) << "Starting storyboard at: '" << startscene << "'" << endl;
                } else {
                    //throw MugenException( "Unhandled option in SceneDef Section: " + itemhead );
                    Global::debug(0) << "Unhandled option in SceneDef Section: '" << itemhead << "'... Continuing" << endl;
                }
            }
        }
        else if( head.find("scene ") != std::string::npos ){
            Global::debug(1) << "Found: '" << head << "'" << endl;
            MugenScene *scene = new MugenScene();
            if (scene){
                Global::debug(1) << "Created: '" << head << "' ok!" << endl;
            } else {
                Global::debug(1) << "Failed to create: '" << head << "'!" << endl;
            }
            while( collection[i]->hasItems() ){
                MugenItemContent *content = collection[i]->getNext();
                const MugenItem *item = content->getNext();
                std::string itemhead = item->query();
                Mugen::Util::removeSpaces(itemhead);
                Mugen::Util::fixCase(itemhead);
                if ( itemhead.find("fadein.time")!=std::string::npos ){
                    int time;
                    *content->getNext() >> time;
                    scene->fader.setFadeInTime(time);
                } else if ( itemhead.find("fadein.col")!=std::string::npos ){
                    int r,g,b;
                    *content->getNext() >> r;
                    *content->getNext() >> g;
                    *content->getNext() >> b;
                    scene->fader.setFadeInColor(Bitmap::makeColor(r,g,b));
                } else if ( itemhead.find("fadeout.time")!=std::string::npos ){
                    int time;
                    *content->getNext() >> time;
                    scene->fader.setFadeOutTime(time);
                } else if ( itemhead.find("fadeout.col")!=std::string::npos ){
                    int r,g,b;
                    *content->getNext() >> r;
                    *content->getNext() >> g;
                    *content->getNext() >> b;
                    scene->fader.setFadeOutColor(Bitmap::makeColor(r,g,b));
                } else if ( itemhead.find("bg.name")!=std::string::npos ){
                    *content->getNext() >> scene->backgroundName;
                } else if ( itemhead.find("clearcolor")!=std::string::npos ){
                    int r,g,b;
                    *content->getNext() >> r;
                    *content->getNext() >> g;
                    *content->getNext() >> b;
                    scene->clearColor = (r == -1 ? r : Bitmap::makeColor(r,g,b));
                } else if ( itemhead.find("end.time")!=std::string::npos ){
                    *content->getNext() >> scene->endTime;
                } else if ( itemhead.find("layerall.pos")!=std::string::npos ){
                    *content->getNext() >> scene->defaultAxis.x;
                    *content->getNext() >> scene->defaultAxis.y;
                } else if ( itemhead.find("layer0.anim")!=std::string::npos ){
                    *content->getNext() >> scene->layers[0]->actionno;
                } else if ( itemhead.find("layer0.offset")!=std::string::npos ){
                    *content->getNext() >> scene->layers[0]->offset.x;
                    *content->getNext() >> scene->layers[0]->offset.y;
                } else if ( itemhead.find("layer0.starttime")!=std::string::npos ){
                    *content->getNext() >> scene->layers[0]->startTime;
                } else if ( itemhead.find("layer1.anim")!=std::string::npos ){
                    *content->getNext() >> scene->layers[1]->actionno;
                } else if ( itemhead.find("layer1.offset")!=std::string::npos ){
                    *content->getNext() >> scene->layers[1]->offset.x;
                    *content->getNext() >> scene->layers[1]->offset.y;
                } else if ( itemhead.find("layer1.starttime")!=std::string::npos ){
                    *content->getNext() >> scene->layers[1]->startTime;
                } else if ( itemhead.find("layer2.anim")!=std::string::npos ){
                    *content->getNext() >> scene->layers[2]->actionno;
                } else if ( itemhead.find("layer2.offset")!=std::string::npos ){
                    *content->getNext() >> scene->layers[2]->offset.x;
                    *content->getNext() >> scene->layers[2]->offset.y;
                } else if ( itemhead.find("layer2.starttime")!=std::string::npos ){
                    *content->getNext() >> scene->layers[2]->startTime;
                } else if ( itemhead.find("layer3.anim")!=std::string::npos ){
                    *content->getNext() >> scene->layers[3]->actionno;
                } else if ( itemhead.find("layer3.offset")!=std::string::npos ){
                    *content->getNext() >> scene->layers[3]->offset.x;
                    *content->getNext() >> scene->layers[3]->offset.y;
                } else if ( itemhead.find("layer3.starttime")!=std::string::npos ){
                    *content->getNext() >> scene->layers[3]->startTime;
                } else if ( itemhead.find("layer4.anim")!=std::string::npos ){
                    *content->getNext() >> scene->layers[4]->actionno;
                } else if ( itemhead.find("layer4.offset")!=std::string::npos ){
                    *content->getNext() >> scene->layers[4]->offset.x;
                    *content->getNext() >> scene->layers[4]->offset.y;
                } else if ( itemhead.find("layer4.starttime")!=std::string::npos ){
                    *content->getNext() >> scene->layers[4]->startTime;
                } else if ( itemhead.find("layer5.anim")!=std::string::npos ){
                    *content->getNext() >> scene->layers[5]->actionno;
                } else if ( itemhead.find("layer5.offset")!=std::string::npos ){
                    *content->getNext() >> scene->layers[5]->offset.x;
                    *content->getNext() >> scene->layers[5]->offset.y;
                } else if ( itemhead.find("layer5.starttime")!=std::string::npos ){
                    *content->getNext() >> scene->layers[5]->startTime;
                } else if ( itemhead.find("layer6.anim")!=std::string::npos ){
                    *content->getNext() >> scene->layers[6]->actionno;
                } else if ( itemhead.find("layer6.offset")!=std::string::npos ){
                    *content->getNext() >> scene->layers[6]->offset.x;
                    *content->getNext() >> scene->layers[6]->offset.y;
                } else if ( itemhead.find("layer6.starttime")!=std::string::npos ){
                    *content->getNext() >> scene->layers[6]->startTime;
                } else if ( itemhead.find("layer7.anim")!=std::string::npos ){
                    *content->getNext() >> scene->layers[7]->actionno;
                } else if ( itemhead.find("layer7.offset")!=std::string::npos ){
                    *content->getNext() >> scene->layers[7]->offset.x;
                    *content->getNext() >> scene->layers[7]->offset.y;
                } else if ( itemhead.find("layer7.starttime")!=std::string::npos ){
                    *content->getNext() >> scene->layers[7]->startTime;
                } else if ( itemhead.find("layer8.anim")!=std::string::npos ){
                    *content->getNext() >> scene->layers[8]->actionno;
                } else if ( itemhead.find("layer8.offset")!=std::string::npos ){
                    *content->getNext() >> scene->layers[8]->offset.x;
                    *content->getNext() >> scene->layers[8]->offset.y;
                } else if ( itemhead.find("layer8.starttime")!=std::string::npos ){
                    *content->getNext() >> scene->layers[8]->startTime;
                } else if ( itemhead.find("layer9.anim")!=std::string::npos ){
                    *content->getNext() >> scene->layers[9]->actionno;
                } else if ( itemhead.find("layer9.offset")!=std::string::npos ){
                    *content->getNext() >> scene->layers[9]->offset.x;
                    *content->getNext() >> scene->layers[9]->offset.y;
                } else if ( itemhead.find("layer9.starttime")!=std::string::npos ){
                    *content->getNext() >> scene->layers[9]->startTime;
                } else if ( itemhead.find("bgm")!=std::string::npos ){
                    // do nothing
                } else if ( itemhead.find("bgm.loop")!=std::string::npos ){
                    // do nothing
                } else throw MugenException( "Unhandled option in Scene Section: " + itemhead + " -> " + head);
            }
            scenes.push_back(scene);
            Global::debug(1) << "Got Scene number: '" << scenes.size() - 1 << "'" << endl;
        }
        else if( head.find("begin action") != std::string::npos ){
            head.replace(0,13,"");
            int h;
            MugenItem(head) >> h;
            animations[h] = Mugen::Util::getAnimation(collection[i], sprites);
        }
        else if ( head.find("def")  != std::string::npos && head.find("scenedef") == std::string::npos ){
            Global::debug(1) << "Checking def!" << endl;
            if (scenes.back()){
                MugenScene *scene = scenes.back();
                std::string name = collection[i]->getHeader();
                Mugen::Util::fixCase(name);
                Global::debug(1) << "Checking for background: " << scene->backgroundName << " in Head: " << name << endl;
                if (name.find(scene->backgroundName)){
                    // this is a background lets set it up
                    MugenBackgroundManager *manager = new MugenBackgroundManager(baseDir,collection, i,scenes.back()->ticker,&sprites);
                    scenes.back()->background = manager;
                    Global::debug(1) << "Got background: '" << manager->getName() << "'" << endl;
                }
            }
        }
        else throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
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
        // backgrounds
        if (scene->background){
            scene->background->preload( DEFAULT_SCREEN_X_AXIS, DEFAULT_SCREEN_Y_AXIS );
        }
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
