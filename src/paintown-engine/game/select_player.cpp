#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/funcs.h"
#include "r-tech1/events.h"
#include "r-tech1/parameter.h"
#include "r-tech1/thread.h"
#include "r-tech1/gui/animation.h"
#include "r-tech1/configuration.h"
#include "r-tech1/init.h"
#include "r-tech1/input/keyboard.h"
#include "r-tech1/input/input-manager.h"
#include "r-tech1/input/input-map.h"
#include "r-tech1/input/input-source.h"
#include "r-tech1/font.h"
#include "r-tech1/file-system.h"
#include "r-tech1/exceptions/exception.h"
#include "r-tech1/loading.h"
#include "r-tech1/tokenreader.h"
#include "r-tech1/token.h"
#include "../object/object.h"
#include "../object/player.h"
#include "globals.h"
#include "../object/display_character.h"
// #include "select_player.h"
#include "game.h"
#include "mod.h"
#include "../level/utils.h"
#include "world.h"
#include "character-select.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>

using namespace std;

namespace Select{
    enum Input{
        Up, Down, Left,
        Right, Remap, Quit,
        Choose,
    };
}

class Selecter: public Util::Logic, public Util::Draw {
    public:
        Selecter(CharacterSelect & select, const InputSource & source, const string & message):
        select(select),
        source(source),
        is_done(false){
            // input.set(Keyboard::Key_ESC, 0, true, Quit);
            for (vector<int>::const_iterator it = source.getKeyboard().begin(); it != source.getKeyboard().end(); it++){
                int player = *it;
                input.set(Configuration::getRight(player), Select::Right);
                input.set(Configuration::getUp(player), Select::Up);
                input.set(Configuration::getDown(player), Select::Down);
                input.set(Configuration::getLeft(player), Select::Left);
                input.set(Keyboard::Key_ESC, Select::Quit);
                input.set(Keyboard::Key_TAB, Select::Remap);
                input.set(Keyboard::Key_ENTER, Select::Choose);
                input.set(Keyboard::Key_SPACE, Select::Choose);
                input.set(Configuration::getAttack1(player), Select::Choose);
            }

            if (source.useJoystick()){
                input.set(Joystick::Up, Select::Up);
                input.set(Joystick::Down, Select::Down);
                input.set(Joystick::Left, Select::Left);
                input.set(Joystick::Right, Select::Right);
                input.set(Joystick::Start, Select::Choose);
                input.set(Joystick::Button1, Select::Choose);
                input.set(Joystick::Button2, Select::Choose);
                input.set(Joystick::Button3, Select::Remap);
                input.set(Joystick::Quit, Select::Quit);
            }

            Util::ReferenceCount<MessageCollection> messages = select.getMessages("select");
            if (messages != NULL){
                messages->setReplaceMessage("select", message);
            }
            select.changeToMessages("select");
        }

        CharacterSelect & select;
        InputMap<Select::Input> input;
        const InputSource & source;
        bool is_done;
        Filesystem::AbsolutePath chosen;

        bool done(){
            return is_done;
        }

        void run(){
            std::vector<InputMap<Select::Input>::InputEvent> out = InputManager::getEvents(input, source);
            for (std::vector<InputMap<Select::Input>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
                const InputMap<Select::Input>::InputEvent & event = *it;
                if (event.enabled){
                    if (event.out == Select::Quit){
                        InputManager::waitForRelease(input, source, Select::Quit);
                        throw Exception::Return(__FILE__, __LINE__);
                    } else if (event.out == Select::Up){
                        select.moveUp(0);
                    } else if (event.out == Select::Down){
                        select.moveDown(0);
                    } else if (event.out == Select::Left){
                        select.moveLeft(0);
                    } else if (event.out == Select::Right){
                        select.moveRight(0);
                    } else if (event.out == Select::Choose){
                        chosen = select.getCurrentSelectedPath(0);
                        is_done = true;
                    } else if (event.out == Select::Remap){
                        select.swap(0);
                    }
                }
            }

            select.act();
        }

        int getRemap(){
            return select.getRemap(0);
        }

        double ticks(double system){
            return system * Global::ticksPerSecond(90);
        }

        void draw(const Graphics::Bitmap & buffer){
            buffer.clear();
            Graphics::StretchedBitmap work(640, 480, buffer, Graphics::StretchedBitmap::NoClear, Graphics::qualityFilterName(Configuration::getQualityFilter()));
            work.start();
            select.draw(work);
            work.finish();
            // buffer.BlitToScreen();
        }
};

static Filesystem::AbsolutePath doSelectPlayer2(CharacterSelect & select, const string & message, int & remap, const InputSource & source){
    remap = 0;
    Selecter run(select, source, message);
    Util::standardLoop(run, run);

    remap = run.getRemap();

    return run.chosen;
}

static string defaultSelectData(){
    /* the default paintown select screen */
    return "(select-screen"
           "(name \"paintown-select\")"
           "(animation"
           "(depth background bottom)"
           "(image 0 \"menu/paintown.png\")"
           "(velocity -.001 0)"
           "(frame"
           "(image 0)"
           "(time -1)))"
           "(grid-list"
             "(grid-size 4 5)"
             "(layout \"infinite-vertical\")"
             "(cell-dimensions 60 60)"
             "(cell-spacing 0 0)"
             "(cell-margins 20 20)"
             "(start-offset 0 40)"
             "(cursors 1)"
             "(wrap true))"
           "(cell (name back) (shape square) (color 0 0 0) (color-fill true))"
           "(cell (name top) (shape square) (color 200 200 200) (color-fill false) (thickness 2))"
           "(cell (name select0) (shape square)"
           "(gradient (low 128 0 0) (high 255 0 0) (size 30))"
           "(color-fill false) (thickness 2))"
           "(cell (name select1) (shape square) (color 0 0 255) (color-fill false) (thickness 2))"
           "(cell (name select-alternative) (shape square) (color 0 255 0) (color-fill false) (thickness 3))"
           "(more \"low\" (location 405 15) (dimensions 100 8) (direction up) (depth foreground bottom) (gradient (low 128 128 128) (high 255 255 0) (distance 20)))"
           "(more \"high\" (location 405 440) (dimensions 100 10) (direction down) (depth foreground bottom) (gradient (low 128 128 128) (high 255 255 0) (distance 20)))"
           "(sound movement sounds/beep1.wav)"
           "(auto-populate-directory \"players\")"
           "(list-window 300 0 340 480)"
           "(profile (window 0 50 300 480) (facing right) (scale 2) (depth background bottom))"
           "(font-dimensions 35 35)"
           "(messages"
           "(name select)"
           "(text"
           " (message \"%s\")"
           " (name select)"
           " (location 10 15)"
           " (color 255 255 255)"
           " (depth foreground top)"
           " (font-dimensions 35 35)"
           " (justification right))"
           "(text"
           " (message \"%s\")"
           " (location 10 50)"
           " (color 255 255 255)"
           " (depth foreground top)"
           " (font-dimensions 35 35)"
           " (justification right)"
           " (cursor-association 0))))";
}

namespace Paintown{

Filesystem::AbsolutePath Mod::selectPlayer(const string & message, const Level::LevelInfo & info, int & remap, const InputSource & source){
    try{
        CharacterSelect select(Paintown::Mod::getCurrentMod()->find(Filesystem::RelativePath("select.txt")));
        return doSelectPlayer2(select, message, remap, source);
    } catch (const Exception::Return & ignore){
        throw;
    } catch (...){
        TokenReader reader;
        CharacterSelect select(reader.readTokenFromString(defaultSelectData()));
        return doSelectPlayer2(select, message, remap, source);
    }
    // return doSelectPlayer(getDisplayPlayers(info), message, info, remap, source);
}

}
