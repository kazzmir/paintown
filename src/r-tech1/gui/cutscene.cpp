#include "r-tech1/gui/cutscene.h"

#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/init.h"
#include "r-tech1/input/input-map.h"
#include "r-tech1/exceptions/load_exception.h"
#include "r-tech1/input/input-manager.h"
#include "r-tech1/input/input-source.h"
#include "r-tech1/token.h"
#include "r-tech1/tokenreader.h"
#include "r-tech1/file-system.h"
#include "r-tech1/configuration.h"

using namespace Gui;

Scene::Scene(const Token * token):
ticks(0),
endTicks(-1){
    parseScene(token);
}

Scene::~Scene(){
}

void Scene::parseScene(const Token * token){
    if (*token != "scene"){
        throw LoadException(__FILE__, __LINE__, "Not a Scene");
    }
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "time"){
                tok->view() >> endTicks;
            } else if (*tok == "animation" || *tok == "anim"){
                addAnimation(Util::ReferenceCount<Gui::Animation>(new Gui::Animation(tok)));
            } else if (*tok == "fade"){
                fader.parseDefaults(tok);
            } else if (*tok == "file"){
                std::string file;
                tok->view() >> file;
                TokenReader reader;
                parseScene(reader.readTokenFromFile(Storage::instance().find(Filesystem::RelativePath(file)).path()));
            } else {
                Global::debug(3) << "Unhandled Scene attribute: " << std::endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Scene parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }
}

void Scene::forward(int tickCount){
    backgrounds.forward(tickCount);
    fader.act();
    // Increment ticks
    ticks += tickCount;
    // Fade out
    if (endTicks >= 0){
        const int check = endTicks - ticks;
        if (check <= fader.getFadeOutTime() && fader.getState() != Gui::FadeTool::FadeOut){
            fader.setState(Gui::FadeTool::FadeOut);
        }
    }
}

void Scene::reverse(int tickCount){
    backgrounds.reverse(tickCount);
    fader.act();
    // Increment ticks
    ticks -= tickCount;
    // Fade out
    if (ticks <= fader.getFadeInTime() && fader.getState() != Gui::FadeTool::FadeOut){
        fader.setState(Gui::FadeTool::FadeOut);
    }
}

void Scene::act(){
    forward();
}

void Scene::render(const Graphics::Bitmap & work){
    // Backgrounds
    backgrounds.render(Gui::Animation::BackgroundBottom, work);
    backgrounds.render(Gui::Animation::BackgroundMiddle, work);
    backgrounds.render(Gui::Animation::BackgroundTop, work);
    
    // Foregrounds
    backgrounds.render(Gui::Animation::ForegroundBottom, work);
    backgrounds.render(Gui::Animation::ForegroundMiddle, work);
    backgrounds.render(Gui::Animation::ForegroundTop, work);
    
    fader.draw(work);
}

void Scene::addAnimation(const Util::ReferenceCount<Gui::Animation> & animation){
    backgrounds.add(animation);
}

void Scene::reset(){
    ticks = 0;
    backgrounds.reset();
    fader.setState(Gui::FadeTool::FadeIn);
}

void Scene::setToEnd(){
    if (endTicks >= 0){
        ticks = endTicks;
    } else {
        if (backgrounds.totalTicks() != -1){
            ticks = backgrounds.totalTicks();
        }
    }
    backgrounds.setToEnd();
    fader.setState(Gui::FadeTool::FadeIn);
}

bool Scene::done() const {
    if (ticks <= 0){
        return true;
    }

    if (endTicks >= 0){
        if (ticks >= endTicks){
            return true;
        }
    } else {
        if (backgrounds.totalTicks() != -1){
            if (ticks >= backgrounds.totalTicks()){
                return true;
            }
        }
    }

    return false;
}

CutScene::CutScene():
width(640),
height(480),
current(0){
}
    
CutScene::CutScene(const Filesystem::AbsolutePath & path):
width(640),
height(480),
current(0){
    TokenReader reader;
    load(reader.readTokenFromFile(path.path().c_str()));
}

CutScene::CutScene(const Token * token):
width(640),
height(480),
current(0){
    load(token);
}

CutScene::~CutScene(){
}

void CutScene::load(const Token * token){
    if (*token != "cutscene"){
        throw LoadException(__FILE__, __LINE__, "Not a CutScene");
    }
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "name"){
                tok->view() >> name;
            } else if (*tok == "resolution"){
                int x = 640;
                int y = 480;
                tok->view() >> x >> y;
                setResolution(x, y);
            } else if (*tok == "scene"){
                scenes.push_back(Util::ReferenceCount<Scene>(new Scene(tok)));
            } else {
                Global::debug(3) << "Unhandled Cutscene attribute: " << std::endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Cutscene parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }

}

void CutScene::setName(const std::string & n){
    name = n;
}
 
void CutScene::setResolution(int w, int h){
    width = w;
    height = h;
}

void CutScene::setScene(unsigned int scene){
    if (scene >= scenes.size()){
        current = scenes.size()-1;
    } else {
        current = scene;
    }
}

enum Keys{
    Esc
};

Util::ReferenceCount<Scene> CutScene::getCurrent(){
    if (scenes.empty()){
        return Util::ReferenceCount<Scene>(NULL);
    }
    
    return scenes[current];
}

void CutScene::playAll(){
    for (unsigned int i = 0; i < scenes.size(); i++){
        playScene(i);
    }
}

void CutScene::playScene(unsigned int scene){
    class Logic: public Util::Logic {
        public:
            Logic(InputMap<Keys> & input, Util::ReferenceCount<Scene> scene):
            is_done(false),
            input(input),
            scene(scene){
            }

            bool is_done;
            InputMap<Keys> & input;
            Util::ReferenceCount<Scene> scene;
        
            bool done(){
                return is_done;
            }

        void run(){
            std::vector<InputMap<Keys>::InputEvent> out = InputManager::getEvents(input, InputSource(true));
            for (std::vector<InputMap<Keys>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
                const InputMap<Keys>::InputEvent & event = *it;
                if (event.enabled){
                    if (event.out == Esc){
                        is_done = true;
                    }
                }
            }
            
            scene->act();
            
            if (scene->done()){
                is_done = true;
            }
        }

        double ticks(double system){
            return system * Global::ticksPerSecond(90);
        }
    };

    class Draw: public Util::Draw {
        public:
            Draw(const Logic & logic, Util::ReferenceCount<Scene> scene, int width, int height):
            logic(logic),
            scene(scene),
            width(width),
            height(height){
            }
            
            const Logic & logic;
            Util::ReferenceCount<Scene> scene;
            int width, height;

            void draw(const Graphics::Bitmap & buffer){
                Graphics::StretchedBitmap work(width, height, buffer);
                work.start();
                scene->render(work);
                work.finish();
                buffer.BlitToScreen();
            }
    };

    if (scene < scenes.size()){
        InputMap<Keys> input;
        input.set(Keyboard::Key_ESC, 0, true, Esc);
        input.set(Joystick::Quit, 0, true, Esc);
        input.set(Configuration::getAttack1(0), Esc);
        input.set(Joystick::Button1, Esc);

        Logic logic(input, scenes[scene]);
        Draw draw(logic, scenes[scene], width, height);

        Util::standardLoop(logic, draw);
    }
}

void CutScene::playScene(){
    if (current < scenes.size()){
        playScene(current);
    }
}
    
void CutScene::next(){
    if (current <= scenes.size()){
        current += 1;
    }
}

bool CutScene::hasMore(){
    return (current < scenes.size());
}
