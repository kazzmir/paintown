#include "mugen/storyboard.h"

#include "util/bitmap.h"
#include "init.h"
#include "resource.h"
#include "util/funcs.h"
#include "globals.h"
#include "factory/font_render.h"

#include "mugen_animation.h"
#include "mugen/background.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"
#include "mugen_font.h"

#include "util/timedifference.h"
#include "ast/all.h"
#include "parser/all.h"
#include "input/input-map.h"
#include "input/input-manager.h"

namespace PaintownUtil = ::Util;

using namespace std;
using namespace Mugen;

static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;
static const int DEFAULT_SCREEN_X_AXIS = 160;
static const int DEFAULT_SCREEN_Y_AXIS = 0;

Layer::Layer():
startTime(0),
enabled(false),
animation(0){
}

Layer::~Layer(){
    if (animation){
	delete animation;
    }
}

void Layer::act(int currentTime){
    if (currentTime >= startTime && !enabled){
        enabled = true;
    }

    if (enabled && animation){
        animation->logic();
    }
}

void Layer::render(int x, int y, const Bitmap &bmp){
    if (enabled && animation){
        animation->render(x + offset.x, y + offset.y, bmp);
    }
}

void Layer::reset(){
    enabled = false;
    if (animation){
	animation->reset();
    }
}
	
Scene::Scene(Ast::Section * data, const std::string & file, Ast::AstParse & parsed, SpriteMap & sprites):
clearColor(-2),
clearColorSet(false),
ticker(0),
endTime(0),
defaultPositionSet(false),
background(0),
maxLayers(10){
    for (int i = 0; i < maxLayers; ++i){
	Layer *layer = new Layer();
	layers.push_back(layer);
    }
    class SceneWalker: public Ast::Walker {
	public:
	    SceneWalker(Scene & scene, const std::string & file, SpriteMap & sprites, Ast::AstParse & parse):
	    scene(scene),
	    file(file),
	    sprites(sprites),
	    parsed(parse){
	    }

	    ~SceneWalker(){
	    }
	    
	    Scene & scene;
	    const std::string & file;
	    SpriteMap & sprites;
	    Ast::AstParse & parsed;
	    
	    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
		if (simple == "fadein.time"){
		    int time;
		    simple >> time;
		    scene.fader.setFadeInTime(time);
		} else if (simple == "fadein.col"){
		    int r=0,g=0,b=0;
		    try{
			simple >> r >> g >> b;
		    } catch (const Ast::Exception & e){
		    }
		    scene.fader.setFadeInColor(Bitmap::makeColor(r,g,b));
		} else if (simple == "fadeout.time"){
		    int time;
		    simple >> time;
		    scene.fader.setFadeOutTime(time);
		} else if (simple == "fadeout.col"){
		    int r=0,g=0,b=0;
		    try {
			simple >> r >> g >> b;
		    } catch (const Ast::Exception & e){
		    }
		    scene.fader.setFadeOutColor(Bitmap::makeColor(r, g, b));
		} else if (simple == "bg.name"){
		    std::string name;
		    simple >> name;
		    scene.background = new Background(file, PaintownUtil::trim(name));
		} else if (simple == "clearcolor"){
		    int r=0,g=0,b=0;
		    try {
			simple >> r >> g >> b;
		    } catch (const Ast::Exception & e){
		    }
		    scene.clearColor = (r == -1 ? r : Bitmap::makeColor(r, g, b));
		    scene.clearColorSet = true;
		} else if (simple == "end.time"){
		    simple >> scene.endTime;
		} else if (simple == "layerall.pos"){
		    try{
			simple >> scene.defaultPosition.x;
			simple >> scene.defaultPosition.y;
			scene.defaultPositionSet = true;
		    } catch (const Ast::Exception & e){
		    }
		} else if (PaintownUtil::matchRegex(simple.idString(), "layer[0-9]\\.anim")){
		    int num = atoi(PaintownUtil::captureRegex(simple.idString(), "layer([0-9])\\.anim", 0).c_str());
		    if (num >= 0 && num < scene.maxLayers){
			std::string action;
			simple >> action;
			Ast::Section * section = parsed.findSection("begin action " + action);
			scene.layers[num]->setAnimation(Util::getAnimation(section,sprites));
		    }
		} else if (PaintownUtil::matchRegex(simple.idString(), "layer[0-9]\\.offset")){
		    int num = atoi(PaintownUtil::captureRegex(simple.idString(), "layer([0-9])\\.offset", 0).c_str());
		    if (num >= 0 && num < scene.maxLayers){
			int x=0,y=0;
			try{
			    simple >> x >> y;
			} catch (Ast::Exception & e){
			}
			scene.layers[num]->setOffset(x, y);
		    }
		} else if (PaintownUtil::matchRegex(simple.idString(), "layer[0-9]\\.starttime")){
		    int num = atoi(PaintownUtil::captureRegex(simple.idString(), "layer([0-9])\\.starttime", 0).c_str());
		    if (num >= 0 && num < scene.maxLayers){
			int time;
			simple >> time;
			scene.layers[num]->setStartTime(time);
                        // Global::debug(0) << "Setting layer " << scene.layers[num] << " [" << num << "] start time to " << time << endl;
		    }
		} else if (simple == "bgm"){
		    // do nothing
		} else if (simple == "bgm.loop"){
		    // do nothing
		} else {
			Global::debug(0) << "Unhandled option in Scene Section: " << simple.toString();
		}
	    }
    };

    SceneWalker walker(*this, file, sprites, parsed);
    data->walk(walker);

    // set initial fade state
    fader.setState(FADEIN);
}

Scene::~Scene(){

    if (background){
        delete background;
    }

    // layers
    for ( std::vector< Layer *>::iterator i = layers.begin(); i != layers.end(); ++i ){
        if ((*i))delete (*i);
    }
}

void Scene::act(){
    // backgrounds
    if (background){
        background->act();
    }
    // layers
    for ( std::vector< Layer *>::iterator i = layers.begin(); i != layers.end(); ++i ){
        Layer *layer = *i;
        layer->act(ticker);
        // Global::debug(0) << "Layer " << layer << " start time is " << layer->getStartTime() << endl;
    }
    // Fader
    fader.act();
    if (ticker == endTime - fader.getFadeOutTime()){
        fader.setState(FADEOUT);
    }
    // tick tick
    ticker++;
}
void Scene::render(const Bitmap & bmp){
    if (clearColor != -1 && clearColor != -2){
        bmp.fill(clearColor);
    }
    // backgrounds
    if (background){
        background->renderBackground(0, 0, bmp);
    }

    // layers
    for (std::vector< Layer *>::iterator i = layers.begin(); i != layers.end(); ++i ){
        Layer *layer = *i;
        layer->render(defaultPosition.x,defaultPosition.y,bmp);
    }
    // foregrounds
    if (background){
        background->renderForeground(0,0, bmp);
    }
    // fader
    fader.draw(bmp);
}

bool Scene::isDone(){
    return (ticker >= endTime);
}

void Scene::reset(){
    ticker = 0;
    fader.setState(FADEIN);
    // layers
    for (std::vector< Layer *>::iterator i = layers.begin(); i != layers.end(); ++i ){
	Layer *layer = *i;
	layer->reset();
    }
}

Storyboard::Storyboard(const string & file):
storyBoardFile(file),
startscene(0){
    // Lets look for our def since some people think that all file systems are case insensitive
    std::string baseDir = Util::getFileDir(storyBoardFile);
    const std::string ourDefFile = Util::fixFileName( baseDir, Util::stripDir(storyBoardFile) );
    // get real basedir
    //baseDir = Util::getFileDir( ourDefFile );
    Global::debug(1) << baseDir << endl;

    if (ourDefFile.empty()){
        throw MugenException("Cannot locate storyboard definition file for: " + storyBoardFile);
    }

    std::string filesdir = "";

    Global::debug(1) << "Got subdir: " << filesdir << endl;

    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed((list<Ast::Section*>*) Def::main(ourDefFile));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + ourDefFile + " in" + diff.printTime("") << endl;

    // Default position for all layers
    bool defaultPositionSet = false;
    Mugen::Point defaultPosition;
    
    // Default clear color for all scenes
    bool clearColorSet = false;
    int clearColor;
    
    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
	
	head = Util::fixCase(head);
	
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
                SceneWalk(const string & baseDir, Storyboard & board):
                    baseDir(baseDir),
                    board(board){
                }

                const string & baseDir;
                Storyboard & board;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "spr"){
			std::string temp;
                        simple >> temp;
                        Util::readSprites(Util::getCorrectFileLocation(this->baseDir, temp), "", board.sprites);
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
	    Scene *scene = new Scene(section, ourDefFile, parsed, sprites);
	    scenes.push_back(scene);
	    // Check default position
	    if (!scene->getDefaultPositionSet() && defaultPositionSet){
		scene->setDefaultPosition(defaultPosition);
	    }
	    if (scene->getDefaultPositionSet()){
		defaultPosition = scene->getDefaultPosition();
		defaultPositionSet = true;
	    }
	    // Check default clear color
	    if (!scene->getClearColorSet() && clearColorSet){
		scene->setClearColor(clearColor);
	    }
	    if (scene->getClearColorSet()){
		clearColor = scene->getClearColor();
		clearColorSet = true;
	    }
	}
    }
}

Storyboard::~Storyboard(){
    
    // Get rid of scene lists;
    for (std::vector<Scene *>::iterator i = scenes.begin(); i != scenes.end(); ++i){
        if (*i){
            delete *i;
        }
    }

    // sprites
    for (SpriteMap::iterator i = sprites.begin(); i != sprites.end(); ++i){
        for (map<unsigned int, MugenSprite *>::iterator j = i->second.begin(); j != i->second.end(); ++j){
            if (j->second){
                delete j->second;
            }
        }
    }
}

void Storyboard::run(const Bitmap &bmp, bool repeat){
    double gameSpeed = 1.0;
    double runCounter = 0;
    bool quit = false;

    Bitmap work( 320, 240 );

    for( std::vector< Scene * >::iterator i = scenes.begin() ; i != scenes.end() ; ++i ){
        if( (*i) ){
            (*i)->reset();
        }
    }

    std::vector< Scene * >::iterator sceneIterator = scenes.begin() + startscene;
    
    // Start and enter interrupts this as well
    InputMap<int> keyInput;
    keyInput.set(Keyboard::Key_ESC, 10, true, 0);
    keyInput.set(Keyboard::Key_ENTER, 10, true, 1);
    int mugenSpeed = 60;
    
    while( !quit ){
        bool draw = false;

        Scene *scene = *sceneIterator;
        if ( Global::speed_counter > 0 ){

            // runCounter += Global::speed_counter * gameSpeed * Global::LOGIC_MULTIPLIER;//(double) 90 / (double) 60;
            runCounter += Global::speed_counter * gameSpeed * mugenSpeed / Global::TICS_PER_SECOND;
            while (runCounter > 1){
                runCounter -= 1;
                draw = true;
                // Key handler
		InputManager::poll();

                InputMap<CharacterKeys>::Output out = InputManager::getMap(input);

                if (out[Up]){
	        }
	        if (out[Down]){
	        }
	        if (out[Left]){
	        }
	        if (out[Right]){
	        }
	        if (out[A]){
                    quit = true;
                    return;
	        }
	        if (out[B]){
                    quit = true;
                    return;
	        }
	        if (out[C]){
                    quit = true;
                    return;
	        }
	        if (out[X]){
                    quit = true;
                    return;
	        }
	        if (out[Y]){
                    quit = true;
                    return;
	        }
	        if (out[Z]){
                    quit = true;
                    return;
	        }
	        if (out[Start]){
                    quit = true;
                    return;
	        }
		
		InputMap<int>::Output keyBoard = InputManager::getMap(keyInput);
		if (keyBoard[0]){
		    quit = true;
		    return;
		}
		if (keyBoard[1]){
		    quit = true;
		    return;
		}
		
                scene->act();
                
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
            scene->render(work);
            work.Stretch(bmp);
            if (Global::getDebug() > 0){
                Font::getDefaultFont().printf( 15, 310, Bitmap::makeColor(0,255,128), bmp, "Scene: Time(%i) : EndTime(%i) : Fade in(%i) : Fade out(%i)",0, scene->getTicker(),scene->getEndTime(),scene->getFadeTool().getFadeInTime(),scene->getFadeTool().getFadeOutTime() );
            }
            bmp.BlitToScreen();
        }

        while (Global::speed_counter == 0){
            PaintownUtil::rest(1);
        }
    }
}
