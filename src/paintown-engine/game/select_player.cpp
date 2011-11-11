#include "util/bitmap.h"
#include "util/lit_bitmap.h"
#include "util/trans-bitmap.h"
#include "paintown-engine/object/object.h"
#include "util/load_exception.h"
#include "util/funcs.h"
#include "util/events.h"
#include "util/parameter.h"
#include "paintown-engine/object/player.h"
#include "globals.h"
#include "paintown-engine/object/display_character.h"
#include "exceptions/shutdown_exception.h"
#include "util/thread.h"
#include "util/init.h"
// #include "select_player.h"
#include "game.h"
#include "mod.h"
#include "util/input/keyboard.h"
#include "util/input/input-manager.h"
#include "util/input/input-map.h"
#include "util/input/input-source.h"
#include "util/font.h"
#include "paintown-engine/level/utils.h"
#include "util/file-system.h"
#include "world.h"
#include "exceptions/exception.h"
#include "util/loading.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "character-select.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>

#include "util/gui/animation.h"

#include "configuration.h"

using namespace std;

#if 0
/* some compilers probably won't like __FILE__. #define it in that case */
static const char * DEBUG_CONTEXT = __FILE__;

struct playerInfo{
    Paintown::DisplayCharacter * guy;
    Filesystem::AbsolutePath path;
    playerInfo(Paintown::DisplayCharacter * guy, const Filesystem::AbsolutePath & path ):
    guy(guy),
    path(path){
    }
};

typedef vector<playerInfo> PlayerVector;
static PlayerVector loadPlayers(const string & path){
    PlayerVector players;
    vector<Filesystem::AbsolutePath> files = Storage::instance().getFiles(Storage::instance().find(Filesystem::RelativePath(path + "/")), "*" );
    std::sort(files.begin(), files.end());
    for ( vector<Filesystem::AbsolutePath>::iterator it = files.begin(); it != files.end(); it++ ){
        string path = (*it).path();
        Global::debug(2, DEBUG_CONTEXT) << "Found file " << path << endl;
        string file = path + "/" + path.substr(path.find_last_of('/') + 1) + ".txt";
        Global::debug(1, DEBUG_CONTEXT) << "Checking " << file << endl;
        if (Util::exists(file)){
            Global::debug(1, DEBUG_CONTEXT) << "Loading " << file << endl;
            try{
                players.push_back(playerInfo(new Paintown::DisplayCharacter(file), Filesystem::AbsolutePath(file)));
            } catch (const LoadException & le){
                Global::debug(0, DEBUG_CONTEXT) << "Could not load " << file << " because " << le.getTrace() << endl;
            }
        }
    }
    return players;
}

/*
template< class Key, class Value >
Key getNth( const map< Key, Value > & m, int i ){
	int count = 0;
	for ( typename map<Key, Value>::const_iterator it = m.begin(); it != m.end(); it++ ){
		if ( count == i ){
			return it->first;
		}
		count += 1;
	}
	return m.begin()->first;
}
*/

static vector<Paintown::DisplayCharacter*> getCharacters(PlayerVector players){
    vector<Paintown::DisplayCharacter*> characters;
    for (PlayerVector::iterator it = players.begin(); it != players.end(); it++){
        characters.push_back(it->guy);
    }

    return characters;
}
    
namespace{
class SelectAttributes{
public:
    SelectAttributes(){
    }

    SelectAttributes(const SelectAttributes & copy){
        if (copy.animation != NULL){
            animation = new Token(*copy.animation);
        }
        background = copy.background;
    }

    const SelectAttributes & operator=(const SelectAttributes & copy){
        if (copy.animation != NULL){
            animation = new Token(*copy.animation);
        }
        background = copy.background;
        return *this;
    }

    virtual ~SelectAttributes(){
    }

    Util::ReferenceCount<Token> animation;
    string background;
};

}

static SelectAttributes loadSelectAttributes(){
    SelectAttributes attributes;
    try{
        TokenReader reader;
        Token * head = reader.readTokenFromFile(Paintown::Mod::getCurrentMod()->find(Filesystem::RelativePath("select.txt")).path().c_str());
        if (*head == "select-screen"){
            TokenView view = head->view();
            while (view.hasMore()){
                const Token * next;
                view >> next;
                if (*next == "background"){
                    next->view() >> attributes.background;
                } else if (*next == "anim" || *next == "animation"){
                    attributes.animation = next->copy();
                }
            }
        } else {
            Global::debug(0) << "Expected to read a 'select-screen' node from select.txt" << std::endl;
        }
    } catch (const Filesystem::NotFound & fail){
        Global::debug(1) << "Could not load select-screen attributes. " << fail.getTrace() << std::endl;
    } catch (const TokenException & fail){
        Global::debug(0) << "Error while reading tokens from select.txt: " << fail.getTrace() << std::endl;
    }

    return attributes;
}

/* TODO: refactor */
static unsigned int choosePlayer(const PlayerVector & players, const string & message, const InputSource & source){

    /* use stupid defines becuase const member variables cannot be initialized
     * inside the class outside of the constructor
     */

#define MAXCOLOR 40
#define MAXGRADIENT 50

    class Logic: public Util::Logic {
    public:
        Logic(const PlayerVector & players, unsigned int & current, Paintown::DisplayCharacterLoader & loader, int & backgroundX, int & boxesPerLine, unsigned int & clock, Util::ReferenceCount<Gui::Animation> animation, const InputSource & source):
        current(current),
        is_done(false),
        clock(clock),
        backgroundX(backgroundX),
        boxesPerLine(boxesPerLine),
        source(source),
        players(players),
        loader(loader),
        beep(Storage::instance().find(Filesystem::RelativePath("sounds/beep1.wav")).path()),
        animation(animation){
            if (source.useKeyboard()){
                Configuration & configuration = Configuration::config(source.getKeyboard());
                input.set(configuration.getRight(), 300, false, Select::Right);
                input.set(configuration.getUp(), 300, false, Select::Up);
                input.set(configuration.getDown(), 300, false, Select::Down);
                input.set(configuration.getLeft(), 300, false, Select::Left);
                input.set(Keyboard::Key_ESC, 0, true, Select::Quit);
                input.set(Keyboard::Key_TAB, 200, false, Select::Remap);
                input.set(Keyboard::Key_ENTER, 0, false, Select::Choose);
                input.set(Keyboard::Key_SPACE, 0, false, Select::Choose);
                input.set(configuration.getAttack1(), 0, false, Select::Choose);
            }

            if (source.useJoystick()){
                Configuration & configuration = Configuration::config(source.getJoystick());
                input.set(configuration.getJoystickUp(), 300, false, Select::Up);
                input.set(configuration.getJoystickDown(), 300, false, Select::Down);
                input.set(configuration.getJoystickLeft(), 300, false, Select::Left);
                input.set(configuration.getJoystickRight(), 300, false, Select::Right);
                input.set(configuration.getJoystickAttack1(), 0, false, Select::Choose);
                input.set(configuration.getJoystickAttack2(), 0, false, Select::Choose);
                input.set(configuration.getJoystickAttack3(), 300, false, Select::Remap);
                input.set(configuration.getJoystickQuit(), 300, false, Select::Quit);
            }

            /* wait for the player to let go of the choose button */
            InputManager::waitForRelease(input, source, Select::Choose);
        }

        unsigned int & current;
        bool is_done;
        unsigned int & clock;
        int & backgroundX;
        int & boxesPerLine;
    
        InputSource source;
        InputMap<Select::Input> input;

        const PlayerVector & players;
        Paintown::DisplayCharacterLoader & loader;
        Sound beep;
        
        Util::ReferenceCount<Gui::Animation> animation;
        
        bool done(){
            return is_done;
        }

        void run(){
            Paintown::DisplayCharacter * character = players[current].guy;
            unsigned int old = current;
            clock += 1;

            if (clock % 5 == 0){
                backgroundX -= 1;
            }
            
            animation->act();

            bool choose = false;
            class Handler: public InputHandler<Select::Input> {
            public:
                Handler(bool & choose,
                        Paintown::DisplayCharacter * character,
                        Paintown::DisplayCharacterLoader & loader,
                        Sound & beep,
                        unsigned int & current,
                        const int boxesPerLine,
                        const int maximum,
                        InputMap<Select::Input> & input,
                        InputSource & source):
                    choose(choose),
                    character(character),
                    loader(loader),
                    beep(beep),
                    current(current),
                    boxesPerLine(boxesPerLine),
                    maximum(maximum),
                    input(input),
                    source(source){
                    }

                bool & choose;
                Paintown::DisplayCharacter * character;
                Paintown::DisplayCharacterLoader & loader;
                Sound & beep;
                unsigned int & current;
                const int boxesPerLine;
                const unsigned int maximum;
                InputMap<Select::Input> & input;
                InputSource source;

                virtual void release(const Select::Input & what, Keyboard::unicode_t unicode){
                }

                virtual void press(const Select::Input & what, Keyboard::unicode_t unicode){
                    switch (what){
                        case Select::Left: {
                            if (current > 0){
                                current = current - 1;
                            }
                            beep.play();
                            break;
                        }
                        case Select::Right: {
                            current = current + 1;
                            beep.play();
                            break;
                        }
                        case Select::Up: {
                            if (current >= (unsigned int)boxesPerLine){
                                current = current - boxesPerLine;
                            } else {
                                current = 0;
                            }
                            beep.play();
                            break;
                        }
                        case Select::Down: {
                            current += boxesPerLine;
                            if (current >= maximum){
                                current = maximum - 1;
                            }
                            beep.play();
                            break;
                        }
                        case Select::Quit: {
                            InputManager::waitForRelease(input, source, Select::Quit);
                            throw Exception::Return(__FILE__, __LINE__);
                        }
                        case Select::Choose: {
                            choose = true;
                            break;
                        }
                        case Select::Remap: {
                            if (character->isLoaded()){
                                character->nextMap();
                            }
                            break;
                        }
                    }
                }
            };

            Handler handler(choose, character, loader, beep, current, boxesPerLine, players.size(), input, source);
            InputManager::handleEvents(input, source, handler);

            if (current >= players.size()){
                current = players.size() - 1;
            }

            if (character->isLoaded()){
                if (character->testAnimation()){
                    character->testReset();
                }
            }

            if (current != old){
                loader.update(players[current].guy);
            }

            is_done |= choose && character->isLoaded();
        }

        unsigned int getCurrent(){
            return current;
        }

        double ticks(double system){
            return system * Global::LOGIC_MULTIPLIER;
        }

    };

    class Draw: public Util::Draw {
    public:
        Draw(const PlayerVector & players, unsigned int & current, int & backgroundX, int & boxesPerLine, const string & message, Paintown::DisplayCharacterLoader & loader, unsigned int & clock, Util::ReferenceCount<Gui::Animation> animation):
        players(players),
        current(current),
        message(message),
        backgroundX(backgroundX),
        clock(clock),
        loader(loader),
        //background(path.path()),
        animation(animation),
        temp(120, 120),
        preview(GFX_X / 2, GFX_Y / 2),
        // reflection(GFX_X / 2, GFX_Y / 2),
        startX(GFX_X / 2 - 20),
        startY(20),
        boxSize(80),
        boxesPerLine(boxesPerLine),
        boxesPerColumn((Graphics::screenParameter.current()->getHeight() - startY) / (boxSize + 10)),
        top(0){
            boxesPerLine = (Graphics::screenParameter.current()->getWidth() - startX) / (boxSize + 10);
            Graphics::blend_palette(unselectedGradient, 3, Graphics::makeColor(0, 0, 0), Graphics::makeColor(255, 0, 0));

            Graphics::blend_palette( selectedGradient, MAXCOLOR / 2, Graphics::makeColor( 0, 128, 0 ), Graphics::makeColor( 0, 255, 0 ) );
            Graphics::blend_palette( selectedGradient + MAXCOLOR / 2, MAXCOLOR / 2, Graphics::makeColor( 0, 255, 0 ), Graphics::makeColor( 0, 128, 0 ) );

            /* fade from yellow to some reddish color */
            Graphics::blend_palette(gradient, MAXGRADIENT / 2, Graphics::makeColor( 255, 255, 0 ), Graphics::makeColor(0xff, 0x33, 0x11));
            Graphics::blend_palette(gradient + MAXGRADIENT / 2, MAXGRADIENT / 2, Graphics::makeColor( 0xff, 0x33, 0x11 ), Graphics::makeColor(255, 255, 0));

        }

        const PlayerVector & players;
        unsigned int & current;
        const string & message;
        int & backgroundX;
        unsigned int & clock;
        Paintown::DisplayCharacterLoader & loader;
 
        //Graphics::Bitmap background;
        Util::ReferenceCount<Gui::Animation> animation;
        Graphics::Bitmap temp;
        Graphics::Bitmap preview;
        // Graphics::Bitmap reflection;

        const int startX;
        const int startY;
        const int boxSize;
        int & boxesPerLine;
        const int boxesPerColumn;
        unsigned int top;

        Graphics::Color selectedGradient[MAXCOLOR];
        Graphics::Color unselectedGradient[3];

        Graphics::Color gradient[MAXGRADIENT];	

        void drawCharacter(Paintown::DisplayCharacter * character, const Graphics::Bitmap & work){
            const Font & font = Font::getFont(Global::DEFAULT_FONT);
            const int stand = 100;
            character->setFacing(Paintown::Object::FACING_RIGHT);
            Paintown::Character copy(*character);
            copy.setDrawShadow(false);
            copy.setX(preview.getWidth() / 2);
            copy.setY(0);
            // copy.setZ( preview.getHeight() - stand );
            copy.setZ(preview.getHeight() - stand);
            preview.fill(Graphics::MaskColor());
            // reflection.fill(Graphics::MaskColor());
            // preview.fill( 0 );
            // reflection.fill( 0 );

            copy.drawOutline(&preview, 0, preview.getHeight() - stand - stand, 0, 0, 0, 255);
            copy.drawReflection(&preview, 0, preview.getHeight() - stand - stand, 128);
            copy.draw(&preview, 0, 0);
#if 0
            preview.drawVFlip(0, 0, reflection);

            int reflection_y = preview.getHeight() - stand - stand;
            /* draw a black solid color version of the character */
            Graphics::Bitmap::transBlender(0, 0, 0, 255);
            Graphics::LitBitmap s2(reflection);
            s2.draw(0, reflection_y, preview);

            /* then draw the original character translucently over the black */
            Graphics::Bitmap::transBlender(0, 0, 0, 128);
            /* TODO: implement a fading reflection. compute the alpha for each y line.
             * start out with alpha of 128 then slowly move towards 64 at the middle
             * of the sprite and then fade to 0 soon after, like after another 10
             * pixels.
             * alpha(y) = y < height/2 : 128 * y/height/2 * 100 ? alpha(64) * log(y)
             */
            reflection.translucent().draw(0, reflection_y, preview);
            copy.draw(&preview, 0, 0);

            // reflection.drawCharacter( 0, preview.getHeight() - stand - stand, 0, -1, preview );
            // preview.floodfill( 0, 0, Bitmap::MaskColor );
            // preview.drawTransVFlip( 0, preview.getHeight() - stand - stand, preview );

            // preview.draw( 60, 0, work );
#endif

            preview.drawStretched(-GFX_X / 2 + startX / 2, -70 + stand, GFX_X, GFX_Y, work);
            // preview.drawStretched(-320, 0, GFX_X, GFX_Y, work);

            // preview.draw( 60, 0, work );

            for (int c = 1; c >= 0; c--){
                int color = 255 - c * 190;
                int x = 10 + 5 * c;
                int y = font.getHeight() + 5 + c * 5;
                font.printf( x, y, Graphics::makeColor(color, color, color ), work, copy.getName(), 0 );
            }
        }

        void drawBoxes(const Graphics::Bitmap & work){
            int x = startX, y = startY;
            unsigned int i;
            for (i = top; i < players.size() && y + boxSize < GFX_Y; i++){
                Graphics::Bitmap box(work, x, y, boxSize, boxSize);
                Paintown::DisplayCharacter * displayed = players[i].guy;
                box.clear();
                // int color = unselectedColor;
                // int * color = i == (unsigned int) current ? selectedGradient : unselectedGradient;
                if (displayed->isLoaded()){
                    temp.clear();
                    Paintown::Character smaller(*displayed);

                    /* draw a border */
                    // box.border( 0, 3, color[ clock % maxColor ] );

                    smaller.setX( temp.getWidth() / 2 );
                    smaller.setY( 0 );
                    smaller.setZ( temp.getHeight() );
                    smaller.draw( &temp, 0, 0 );
                    temp.drawStretched(0, 0, box.getWidth(), box.getHeight(), box);
                } else {
                    /* FIXME: center the text */

                    const Font & font = Font::getFont(Global::DEFAULT_FONT, 15, 15);
                    font.printf(box.getWidth() / 2 - font.textLength(displayed->getName().c_str()) / 2, box.getHeight() / 2 - font.getHeight() / 2, Graphics::makeColor(255, 255, 255), box, displayed->getName(), 0);
                }

                if (i == current){
                    box.border(0, 3, selectedGradient[clock % MAXCOLOR]);
                } else {
                    for (int border = 0; border < 3; border++){
                        box.border(border, border + 1, unselectedGradient[border]);
                    }

                    /*
                       if (i % 2 == 0){
                       for (int border = 0; border < 3; border++){
                       box.border(border, border + 1, unselectedGradient[border]);
                       }
                    // box.border(0, 3, unselectedGradient[0]);
                    } else {
                    box.border(0, 3, unselectedGradient[2]);
                    }
                    */
                }
                x += boxSize + 10;
                if ( x + boxSize + 10 > work.getWidth() ){
                    x = startX;
                    y += (boxSize + 10);
                }
            }

            if (top > 0){
                int x1 = 80;
                int x2 = 140;
                work.triangle(startX + x1, 8, startX + x2, 8, startX + (x1 + x2) / 2, 3, gradient[clock % MAXGRADIENT]);
            }

            if (i < players.size()){
                int x1 = 80;
                int x2 = 140;
                work.triangle(startX + x1, GFX_Y - 8, startX + x2, GFX_Y - 8, startX + (x1 + x2) / 2, GFX_Y - 3, gradient[clock % MAXGRADIENT]);
            }
        }

        void draw(const Graphics::Bitmap & work){
            Paintown::DisplayCharacter * character = players[current].guy;

            /*if (backgroundX < - work.getWidth()){
                backgroundX += work.getWidth();
            }*/

            while (current < top){
                top -= boxesPerLine;
            }

            while (current >= top + boxesPerLine * boxesPerColumn){
                top += boxesPerLine;
            }
            // background.Stretch( work );
            /*background.Blit(backgroundX, 0, work);
            background.Blit(work.getWidth() + backgroundX, 0, work);*/
            work.clear();
            animation->draw(work);
            const Font & font = Font::getFont(Global::DEFAULT_FONT);

            if (character->isLoaded()){
                drawCharacter(character, work);
            }

            font.printf( 10, 10, Graphics::makeColor( 255, 255, 255 ), work, message, 0 );

            if (!loader.done()){
                const Font & font = Font::getFont(Global::DEFAULT_FONT, 10, 10 );
                font.printf(1, 1, Graphics::makeColor(200,0,0), work, "Loading...", 0);
            }

            drawBoxes(work);
            work.BlitToScreen();
        }
    };

    Paintown::DisplayCharacterLoader loader(getCharacters(players));
    Util::Thread::Id loadingThread;

    class DisplayThread: public Util::Thread::ThreadObject {
    public:
        DisplayThread(Paintown::DisplayCharacterLoader & loader):
        ThreadObject(&loader, characterLoader),
        loader(loader){
        }

        Paintown::DisplayCharacterLoader & loader;

        /* run the loader in a separate thread */
        static void * characterLoader(void * arg){
            Paintown::DisplayCharacterLoader * loader = (Paintown::DisplayCharacterLoader*) arg;
            loader->load();
            return NULL;
        }

        virtual ~DisplayThread(){
            loader.stop();
        }
    };

    DisplayThread loading(loader);

    /*
    if (!Util::Thread::createThread(&loadingThread, NULL, (Util::Thread::ThreadFunction) characterLoader, &loader)){
        throw LoadException(__FILE__, __LINE__, "Could not create loading thread");
    }
    */
    if (!loading.start()){
        throw LoadException(__FILE__, __LINE__, "Could not create loading thread");
    }

    SelectAttributes attributes = loadSelectAttributes();

    unsigned int current = 0;
    int backgroundX = 0;
    int boxesPerLine = 0;
    unsigned int clock = 0;
    Util::ReferenceCount<Gui::Animation> animation;
    Filesystem::AbsolutePath background;
    if (attributes.background != ""){
        try{
            background = Storage::instance().find(Filesystem::RelativePath(attributes.background));
        } catch (const Filesystem::NotFound & fail){
            Global::debug(0) << "Could not find background. " << fail.getTrace() << std::endl;
            background = Global::titleScreen();
        }
    } else {
        background = Global::titleScreen();
    }
    if (attributes.animation != NULL){
        // ignore above bitmap
        animation = new Gui::Animation(attributes.animation.raw());
    } else {
        animation = new Gui::Animation(background);
    }
    Logic logic(players, current, loader, backgroundX, boxesPerLine, clock, animation, source);
    Draw draw(players, current, backgroundX, boxesPerLine, message, loader, clock, animation);

    Util::standardLoop(logic, draw);
    return logic.getCurrent();
}

static Filesystem::AbsolutePath doSelectPlayer(const PlayerVector & players, const string & message, const Level::LevelInfo & info, int & remap, const InputSource & source){
    try{
        unsigned int current = 0;
        if (players.size() == 0){
            ostringstream out;
            out << "No players found in '" << info.getPlayerPath() << "'";
            throw LoadException(__FILE__, __LINE__,out.str());
        }
        /* only run the selection screen if there is more than 1 player to choose */
        if (players.size() > 1){
            current = choosePlayer(players, message, source);
        }

        /* set the map */
        remap = players[current].guy->getCurrentMap();

        /* delete all the preview characters. its ok to delete them
         * before looking up the selected player in the map
         * because 'delete' doesn't affect the map, it just changes
         * memory around.
         */
        Filesystem::AbsolutePath path = players[current].path;
        for (PlayerVector::const_iterator it = players.begin(); it != players.end(); it++){
            delete it->guy;
        }

        Global::debug(1, DEBUG_CONTEXT) << "Selected " << path.path() << ". Loading.." << endl;
        return path;
    } catch (const Exception::Return & r){
        for (PlayerVector::const_iterator it = players.begin(); it != players.end(); it++){
            delete it->guy;
        }
        throw r;
    }
}

static PlayerVector getDisplayPlayers(const Level::LevelInfo & info){
    class Context: public Loader::LoadingContext {
    public:
        Context(const string & path):
        path(path){
        }

        virtual void load(){
            players = loadPlayers(path);
        }

        string path;
        PlayerVector players;
    };

    /* hm, it would be nice to cache this I suppose */
    Context context(info.getPlayerPath());
    Loader::loadScreen(context, info, Loader::SimpleCircle);
    // context.load();

    return context.players;
}
#endif

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
            if (source.useKeyboard()){
                Configuration & configuration = Configuration::config(source.getKeyboard());
                input.set(configuration.getRight(), Select::Right);
                input.set(configuration.getUp(), Select::Up);
                input.set(configuration.getDown(), Select::Down);
                input.set(configuration.getLeft(), Select::Left);
                input.set(Keyboard::Key_ESC, Select::Quit);
                input.set(Keyboard::Key_TAB, Select::Remap);
                input.set(Keyboard::Key_ENTER, Select::Choose);
                input.set(Keyboard::Key_SPACE, Select::Choose);
                input.set(configuration.getAttack1(), Select::Choose);
            }

            if (source.useJoystick()){
                Configuration & configuration = Configuration::config(source.getJoystick());
                input.set(configuration.getJoystickUp(), Select::Up);
                input.set(configuration.getJoystickDown(), Select::Down);
                input.set(configuration.getJoystickLeft(), Select::Left);
                input.set(configuration.getJoystickRight(), Select::Right);
                input.set(configuration.getJoystickAttack1(), Select::Choose);
                input.set(configuration.getJoystickAttack2(), Select::Choose);
                input.set(configuration.getJoystickAttack3(), Select::Remap);
                input.set(configuration.getJoystickQuit(), Select::Quit);
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
            return system * Global::LOGIC_MULTIPLIER;
        }

        void draw(const Graphics::Bitmap & buffer){
            buffer.clear();
            select.draw(buffer);
            buffer.BlitToScreen();
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
        CharacterSelect select(Paintown::Mod::getCurrentMod()->find(Filesystem::RelativePath("select.txt")).path());
        return doSelectPlayer2(select, message, remap, source);
    } catch (...){
        TokenReader reader;
        CharacterSelect select(reader.readTokenFromString(defaultSelectData()));
        return doSelectPlayer2(select, message, remap, source);
    }
    // return doSelectPlayer(getDisplayPlayers(info), message, info, remap, source);
}

}
