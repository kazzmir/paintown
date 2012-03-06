#include "storyboard.h"

#include "util/bitmap.h"
#include "util/stretch-bitmap.h"
#include "util/resource.h"
#include "util/funcs.h"
#include "util/events.h"
#include "globals.h"
#include "util/init.h"
#include "factory/font_render.h"
#include "parse-cache.h"

#include "animation.h"
#include "background.h"
#include "sound.h"
#include "reader.h"
#include "sprite.h"
#include "util.h"
#include "font.h"

#include "util/debug.h"
#include "util/timedifference.h"
#include "util/music.h"
#include "ast/all.h"
#include "parser/all.h"
#include "util/input/input-map.h"
#include "util/input/input-source.h"
#include "util/input/input-manager.h"

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
}

void Layer::act(int currentTime){
    if (currentTime >= startTime && !enabled){
        enabled = true;
    }

    if (enabled && animation != NULL){
        animation->logic();
    }
}

void Layer::render(int x, int y, const Graphics::Bitmap &bmp){
    if (enabled && animation != NULL){
        animation->render(x + offset.x, y + offset.y, bmp);
    }
}

void Layer::reset(){
    enabled = false;
    if (animation != NULL){
	animation->reset();
    }
}
	
Scene::Scene(Ast::Section * data, const Filesystem::AbsolutePath & file, const AstRef & parsed, SpriteMap & sprites):
clearColor(Graphics::MaskColor()),
clearColorSet(false),
ticker(0),
endTime(0),
defaultPositionSet(false),
background(0),
maxLayers(10),
musicStop(false),
musicLoop(true){
    for (int i = 0; i < maxLayers; ++i){
        Layer *layer = new Layer();
        layers.push_back(layer);
    }
    class SceneWalker: public Ast::Walker {
    public:
        SceneWalker(Scene & scene, const Filesystem::AbsolutePath & file, SpriteMap & sprites, const AstRef & parse):
            scene(scene),
            file(file),
            sprites(sprites),
            parsed(parse){
            }

        virtual ~SceneWalker(){
        }

        Scene & scene;
        const Filesystem::AbsolutePath & file;
        SpriteMap & sprites;
        const AstRef & parsed;

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            if (simple == "fadein.time"){
                int time;
                simple.view() >> time;
                scene.fader.setFadeInTime(time);
            } else if (simple == "fadein.col"){
                int r=0,g=0,b=0;
                try{
                    simple.view() >> r >> g >> b;
                } catch (const Ast::Exception & e){
                }
                scene.fader.setFadeInColor(Graphics::makeColor(r,g,b));
            } else if (simple == "fadeout.time"){
                int time;
                simple.view() >> time;
                scene.fader.setFadeOutTime(time);
            } else if (simple == "fadeout.col"){
                int r=0,g=0,b=0;
                try {
                    simple.view() >> r >> g >> b;
                } catch (const Ast::Exception & e){
                }
                scene.fader.setFadeOutColor(Graphics::makeColor(r, g, b));
            } else if (simple == "bg.name"){
                std::string name;
                simple.view() >> name;
                // scene.background = new Background(file, name);
                scene.background = new Background(parsed, name, sprites);
            } else if (simple == "clearcolor"){
                int r=0,g=0,b=0;
                try {
                    simple.view() >> r >> g >> b;
                } catch (const Ast::Exception & e){
                }
                scene.clearColor = (r == -1 ? Graphics::Color() : Graphics::makeColor(r, g, b));
                scene.clearColorSet = true;
            } else if (simple == "end.time"){
                simple.view() >> scene.endTime;
            } else if (simple == "layerall.pos"){
                try{
                    simple.view() >> scene.defaultPosition.x >> scene.defaultPosition.y;
                    scene.defaultPositionSet = true;
                } catch (const Ast::Exception & e){
                }
            } else if (PaintownUtil::matchRegex(simple.idString(), "layer[0-9]\\.anim")){
                int num = atoi(PaintownUtil::captureRegex(simple.idString(), "layer([0-9])\\.anim", 0).c_str());
                if (num >= 0 && num < scene.maxLayers){
                    std::string action;
                    simple.view() >> action;
                    Ast::Section * section = parsed->findSection("begin action " + action);
                    scene.layers[num]->setAnimation(Util::getAnimation(section, sprites, false));
                }
            } else if (PaintownUtil::matchRegex(simple.idString(), "layer[0-9]\\.offset")){
                int num = atoi(PaintownUtil::captureRegex(simple.idString(), "layer([0-9])\\.offset", 0).c_str());
                if (num >= 0 && num < scene.maxLayers){
                    int x=0,y=0;
                    try{
                        simple.view() >> x >> y;
                    } catch (Ast::Exception & e){
                    }
                    scene.layers[num]->setOffset(x, y);
                }
            } else if (PaintownUtil::matchRegex(simple.idString(), "layer[0-9]\\.starttime")){
                int num = atoi(PaintownUtil::captureRegex(simple.idString(), "layer([0-9])\\.starttime", 0).c_str());
                if (num >= 0 && num < scene.maxLayers){
                    int time;
                    simple.view() >> time;
                    scene.layers[num]->setStartTime(time);
                    // Global::debug(0) << "Setting layer " << scene.layers[num] << " [" << num << "] start time to " << time << endl;
                }
            } else if (simple == "bgm"){
                try{
                    std::string bgm;
                    simple.view() >> bgm;
                    if (!bgm.empty()){
                        try{
                            scene.music = Util::findFile(file.getDirectory(), Filesystem::RelativePath(bgm));
                        } catch (const Filesystem::NotFound & fail){
                            Global::debug(0) << "Could not find bgm file " << bgm << " because " << fail.getTrace() << endl;
                        }
                    }
                } catch (const Ast::Exception & e){
                    scene.musicStop = true;
                }
            } else if (simple == "bgm.loop"){
                try{
                    simple.view() >> scene.musicLoop;
                } catch (const Ast::Exception & e){
                }
            } else {
                Global::debug(0) << "Unhandled option in Scene Section: " << simple.toString();
            }
        }
    };

    SceneWalker walker(*this, file, sprites, parsed);
    data->walk(walker);

    // set initial fade state
    fader.setState(Gui::FadeTool::FadeIn);
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
        fader.setState(Gui::FadeTool::FadeOut);
    }
    // tick tick
    ticker++;
}
void Scene::render(const Graphics::Bitmap & bmp){
    if (clearColor != Graphics::MaskColor()){
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
    fader.setState(Gui::FadeTool::FadeIn);
    // layers
    for (std::vector< Layer *>::iterator i = layers.begin(); i != layers.end(); ++i ){
	Layer *layer = *i;
	layer->reset();
    }
}

void Scene::startMusic(){
    // Run bgm
    if (musicStop){
        Music::pause();
    } else if (!music.path().empty()){
        try {
            Music::loadSong(music.path());
            Music::pause();
            Music::play();
        } catch (const MugenException & ex){
        } catch (const Filesystem::NotFound & fail){
            Global::debug(0) << "Could not load music: " << fail.getTrace() << endl;
        }
    }
}

Storyboard::Storyboard(const Filesystem::AbsolutePath & file, bool mask):
storyBoardFile(file),
startscene(0){
    // Lets look for our def since some people think that all file systems are case insensitive
    Filesystem::AbsolutePath baseDir = storyBoardFile.getDirectory();
    const Filesystem::AbsolutePath ourDefFile = Util::fixFileName(baseDir, storyBoardFile.getFilename().path());
    // get real basedir
    //baseDir = Util::getFileDir( ourDefFile );
    Global::debug(1) << baseDir.path() << endl;

    if (ourDefFile.isEmpty()){
        throw MugenException("Cannot locate storyboard definition file for: " + storyBoardFile.path(), __FILE__, __LINE__);
    }

    std::string filesdir = "";

    Global::debug(1) << "Got subdir: " << filesdir << endl;

    TimeDifference diff;
    diff.startTime();
    AstRef parsed(Util::parseDef(ourDefFile.path()));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + ourDefFile.path() + " in" + diff.printTime("") << endl;

    // Default position for all layers
    bool defaultPositionSet = false;
    Mugen::Point defaultPosition;
    
    // Default clear color for all scenes
    bool clearColorSet = false;
    Graphics::Color clearColor = Graphics::makeColor(0, 0, 0);
    
    try{
        for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
            Ast::Section * section = *section_it;
            std::string head = section->getName();

            head = Util::fixCase(head);

            if (head == "info"){
                class InfoWalk: public Ast::Walker{
                public:
                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "name"){
                            string name;
                            simple.view() >> name;
                            Global::debug(1) << "Read name '" << name << "'" << endl;
                        } else if (simple == "author"){
                            string name;
                            simple.view() >> name;
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
                    SceneWalk(const Filesystem::AbsolutePath & baseDir, Storyboard & board, bool mask):
                        baseDir(baseDir),
                        board(board),
                        mask(mask){
                        }

                    const Filesystem::AbsolutePath & baseDir;
                    Storyboard & board;
                    bool mask;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "spr"){
                            std::string temp;
                            simple.view() >> temp;
                            Util::readSprites(Util::findFile(baseDir, Filesystem::RelativePath(temp)), Filesystem::AbsolutePath(), board.sprites, mask);
                        } else if (simple == "startscene"){
                            simple.view() >> board.startscene;
                            Global::debug(1) << "Starting storyboard at: '" << board.startscene << "'" << endl;
                        } else {
                            Global::debug(0) << "Warning: ignored attribute: " << simple.toString() << endl;
                        }
                    }
                };

                SceneWalk walk(baseDir, *this, mask);
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
    } catch (const Filesystem::NotFound & fail){
        ostringstream out;
        out << "Error while reading storyboard for " << file.path() << ": " << fail.getTrace();
        throw MugenException(out.str(), __FILE__, __LINE__);
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
/*    for (SpriteMap::iterator i = sprites.begin(); i != sprites.end(); ++i){
        for (map<unsigned int, Sprite *>::iterator j = i->second.begin(); j != i->second.end(); ++j){
            if (j->second){
                delete j->second;
            }
        }
    }*/
}

void Storyboard::run(bool repeat){
    class Logic: public PaintownUtil::Logic {
    public:
        Logic(InputMap<Mugen::Keys> & input, const vector<Scene*> & scenes, int startscene, bool repeat, Scene *& scene):
        input(input),
        scenes(scenes),
        quit(false),
        repeat(repeat),
        scene(scene){

            for (std::vector<Scene *>::const_iterator i = scenes.begin(); i != scenes.end(); ++i){
                if ((*i)){
                    (*i)->reset();
                }
            }

            sceneIterator = scenes.begin() + startscene;
            (*sceneIterator)->startMusic();
        }

        InputMap<Mugen::Keys> & input;
        const vector<Scene*> & scenes;
        std::vector<Scene*>::const_iterator sceneIterator;
        bool quit;
        bool repeat;
        /* shared between logic and draw */
        Scene *& scene;

        double ticks(double system){
            return Util::gameTicks(system);
        }

        bool done(){
            return quit;
        }

        void run(){
            vector<InputMap<Keys>::InputEvent> eventsHold = InputManager::getEvents(input, InputSource());
            for (vector<InputMap<Keys>::InputEvent>::iterator it = eventsHold.begin(); it != eventsHold.end(); it++){
                InputMap<Keys>::InputEvent event = *it;

                if (!event.enabled){
                    continue;
                }

                switch (event.out){
                    case A:
                    case B:
                    case C:
                    case X:
                    case Y:
                    case Z:
                    case Start:
                    case Esc:
                    case Enter: {
                        quit = true;
                        return;
                    }
                    default: break;
                }
            }

            scene = *sceneIterator;
            scene->act();

            if (scene->isDone()){
                sceneIterator++;
                if (sceneIterator == scenes.end()){
                    if (repeat){
                        sceneIterator = scenes.begin();
                    } else {
                        quit = true;
                        return;
                    }
                }
                scene = *sceneIterator;
                scene->reset();
                scene->startMusic();
            }
        }
    };

    class Draw: public PaintownUtil::Draw {
    public:
        Draw(Scene *& scene):
        scene(scene){
        }

        Scene *& scene;

        void draw(const Graphics::Bitmap & screen){
            if (scene != NULL){
                Graphics::StretchedBitmap work(320, 240, screen, Graphics::qualityFilterName(::Configuration::getQualityFilter()));
                work.start();
                scene->render(work);
                work.finish();
                // work.Stretch(screen);
                if (Global::getDebug() > 0){
                    ::Font::getDefaultFont().printf( 15, 310, Graphics::makeColor(0,255,128), screen, "Scene: Time(%i) : EndTime(%i) : Fade in(%i) : Fade out(%i)",0, scene->getTicker(),scene->getEndTime(),scene->getFadeTool().getFadeInTime(),scene->getFadeTool().getFadeOutTime() );
                }
                screen.BlitToScreen();
            }
        }
    };

    Scene * scene = NULL;
    Logic logic(input, scenes, startscene, repeat, scene);
    Draw draw(scene);
    PaintownUtil::standardLoop(logic, draw);
}
