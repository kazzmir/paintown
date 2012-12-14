#include "util/graphics/bitmap.h"

#include "game.h"
#include "mod.h"
#include "move-list.h"
#include "util/sound/music.h"
#include "util/funcs.h"
#include "util/font.h"
#include "util/pointer.h"
#include "util/parameter.h"
#include "util/gui/coordinate.h"
#include "util/gui/popup-box.h"
#include "util/gui/cutscene.h"
#include "util/menu/menu.h"
#include "util/menu/optionfactory.h"
#include "util/timedifference.h"
#include "util/menu/menu_option.h"
#include "configuration.h"
#include "../object/object.h"
#include "../object/character.h"
#include "../object/player.h"
#include "../object/animation.h"
#include "../factory/object_factory.h"
#include "../factory/heart_factory.h"
#include "paintown-engine/level/utils.h"
#include "factory/font_render.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "util/input/input-source.h"
#include "util/input/keyboard.h"
#include "globals.h"
#include "../script/script.h"
#include "util/exceptions/shutdown_exception.h"
#include "util/exceptions/exception.h"
#include "util/file-system.h"
#include "util/system.h"
#include "util/events.h"
#include "util/loading.h"
#include "util/regex.h"
#include "util/network/network.h"
#include "world.h"
#include "adventure_world.h"
#include "util/console.h"
#include "util/input/input-manager.h"
#include "util/input/input-map.h"
#include "versus_world.h"
#include "util/init.h"
#include "util/thread.h"
#include "options.h"
#include <iostream>
#include <math.h>
#include <string.h>

using namespace std;

// static int LAZY_KEY_DELAY = 300;
static bool show_loading_screen = true;

namespace Game{

const int Ticks = 90;

struct Background{
    string path;
    int z;

    Background():z(0){}
};
        
static double startingGameSpeed(){
    return 1.0;
}

static vector<Background> readBackgrounds( const Filesystem::AbsolutePath & path ){
    vector<Background> backgrounds;

    try{
        TokenReader reader;
        Token * head = reader.readTokenFromFile(path.path() + "/bgs.txt");

        if ( *head == "backgrounds" ){
            TokenView view = head->view();
            while (view.hasMore()){
                const Token * background;
                view >> background;
                if ( *background == "background" ){
                    Background b;
                    TokenView backgroundView = background->view();
                    for ( int i = 0; i < 2; i++ ){
                        const Token * next;
                        backgroundView >> next;
                        if ( *next == "path" ){
                            next->view() >> b.path;
                        } else if ( *next == "z" ){
                            next->view() >> b.z;
                        }
                    }
                    backgrounds.push_back(b);
                }
            }
        }

    } catch ( const TokenException & ex ){
        Global::debug( 0 ) << "Could not load " + path.path() + "/bgs.txt because " << ex.getTrace() << endl;
    }

    /*
       Background b1;
       b1.path = path + "/versus/bg1.png";
       b1.z = 420;
       backgrounds.push_back( b1 );
       */

    return backgrounds;
}

static string findNextFile( const char * name ){
    char buf[ 128 ];
    const char * extension = strchr( name, '.' );
    char first[ 128 ];
    strncpy( first, name, extension - name );
    first[ extension - name ] = '\0';
    unsigned int num = 0;
    sprintf( buf, "%s%u%s", first, num, extension );
    do{
        num += 1;
        sprintf( buf, "%s%u%s", first, num, extension );
        /* num != 0 prevents an infinite loop in the extremely
         * remote case that the user has 2^32 files in the directory
         */
    } while (num != 0 && Util::exists(buf));
    return string(buf);
}

static void drawHelp( const Font & font, int x, int y, Graphics::Color color, const Graphics::Bitmap & buffer ){
    font.printf( x, y, color, buffer, "Controls", 0 );
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Up: %s", 0,  Keyboard::keyToName(Configuration::getUp(0)));
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Down: %s", 0,  Keyboard::keyToName(Configuration::getDown(0)));
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Left: %s", 0,  Keyboard::keyToName(Configuration::getLeft(0)));
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Right: %s", 0,  Keyboard::keyToName(Configuration::getRight(0)));
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Jump: %s", 0,  Keyboard::keyToName(Configuration::getJump(0)));
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Attack1: %s", 0,  Keyboard::keyToName(Configuration::getAttack1(0)));
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Attack2: %s", 0,  Keyboard::keyToName(Configuration::getAttack2(0)));
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Attack3: %s", 0,  Keyboard::keyToName(Configuration::getAttack3(0)));
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Tab to hide/show minimap", 0 );
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Press F1 to view this help", 0 );
}

namespace Game{
    enum Input{
        Screenshot,
        Slowdown,
        Speedup,
        NormalSpeed,
        ReloadLevel,
        KillAllHumans,
        Pause,
        MiniMaps,
        ShowHelp,
        ShowFps,
        Quit,
        Console,
    };
}

static void doTakeScreenshot(const Graphics::Bitmap & work){
    string file = findNextFile("paintown-screenshot.png");
    Global::debug(2) << "Saved screenshot to " << file << endl;
    work.save(file);
}

class GameOptionFactory: public Paintown::OptionFactory {
public:
    GameOptionFactory(const vector<Paintown::Player *> & players):
    Paintown::OptionFactory(),
    players(players){
    }

    vector<Paintown::Player *> players;

    MenuOption * getOption(const Gui::ContextBox & parent, const Token * data) const {
        const Token * head;
        data->view() >> head;
        if (*head == "move-list"){
            return new OptionMoveList(parent, head, players);
        }

        MenuOption * get = Paintown::OptionFactory::getOption(parent, data);
        if (get != NULL){
            return get;
        }

        return Menu::OptionFactory::getOption(parent, data);
    }
};

static const Graphics::Bitmap & getScreen(){
    return *Graphics::screenParameter.current();
}

/* in-game menu */
static bool doMenu(const Token * data, const vector<Paintown::Player *> & players){
    const Graphics::Bitmap & screen_buffer = getScreen();
    // Menu::Menu menu(Filesystem::find(Filesystem::RelativePath("menu/in-game.txt")));
    GameOptionFactory optionFactory(players);
    Menu::Menu menu(data, optionFactory);
    Menu::Context context;

    for (vector<Paintown::Player*>::const_iterator it = players.begin(); it != players.end(); it++){
        Paintown::Player * player = *it;
        player->resetInput();
    }

    /* use the current screen as the background */
    /* in SDL/Allegro4 the screen buffer is always GFX_X, GFX_Y because it is
     * an intermediate buffer before scaling to the screen.
     * in Allegro5 the screen buffer is the actual screen so it may be larger
     * than GFX_X, GFX_Y.
     * Here we do a hack to rescale the screen buffer to the appropriate size.
     * The width/height of screen_buffer in Allegro5 will be GFX_X, GFX_Y because
     * thats what it was created with.
     */
    /* FIXME: scale it to the size of the menu */
    context.addBackground(Graphics::Bitmap(screen_buffer, true).scaleTo(640, 480));
    try{
        menu.run(context);
        /* im pretty sure there is no way to get the menu to return normally,
         * it should always throw an exception either because ESC was pressed
         * or because an option threw something
         */
        return false;
    } catch (const Exception::Quit & forceQuit){
        return true;
    } catch (const Exception::Return & back){
        return false;
    }
}

class StageNext: public std::exception {
public:
    virtual ~StageNext() throw () {
    }
};

class StagePrevious: public std::exception {
public:
    virtual ~StagePrevious() throw () {
    }
};

class StageIndex: public std::exception {
public:
    StageIndex(int index):
        index(index){
        }

    int index;

    virtual ~StageIndex() throw () {
    }
};

bool playLevel( World & world, const vector< Paintown::Object * > & players){
    /* 150 pixel tall console */
    Console::Console console(150);
    {
        class CommandQuit: public Console::Command {
        public:
            CommandQuit(){
            }

            string getDescription() const {
                return "quit - quit the game entirely";
            }

            string act(const string & line){
                throw ShutdownException();
            }
        };

        class CommandMemory: public Console::Command {
        public:
            CommandMemory(){
            }
            
            string getDescription() const {
                return "memory - current memory usage";
            }

            string act(const string & line){
                ostringstream out;
                out << "Memory usage: " << Util::niceSize(System::memoryUsage()) << "\n";
                return out.str();
            }
        };

        class CommandHelp: public Console::Command {
        public:
            CommandHelp(const Console::Console & console):
            console(console){
            }

            const Console::Console & console;
                
            string getDescription() const {
                return "help - this help menu";
            }

            string act(const string & line){
                ostringstream out;
                const vector<Util::ReferenceCount<Console::Command> > commands = console.getCommands();
                for (vector<Util::ReferenceCount<Console::Command> >::const_iterator it = commands.begin(); it != commands.end(); it++){
                    Util::ReferenceCount<Console::Command> command = *it;
                    out << command->getDescription() << "\n";
                }
                return out.str();
            }
        };

        class CommandClear: public Console::Command {
        public:
            CommandClear(Console::Console & console):
            console(console){
            }

            Console::Console & console;

            string getDescription() const {
                return "clear - Clear the console";
            }

            string act(const string & line){
                console.clear();
                return "";
            }
        };

        class CommandStage: public Console::Command {
        public:
            string getDescription() const {
                return "stage [next] [previous] [#] - Move to the next stage, previous stage, or stage at the given index";
            }
            
            string act(const string & line){
                istringstream in(line);
                string next;
                /* first skip the 'stage' part */
                in >> next >> next;
                if (next == "next"){
                    throw StageNext();
                } else if (next == "previous"){
                    throw StagePrevious();
                } else {
                    /* If the index is too large the game won't jump to that stage
                     * but the current game will still be aborted.
                     */
                    int which = -1;
                    istringstream number(next);
                    number >> which;
                    if (which != 0){
                        throw StageIndex(which);
                    }
                }
                return getDescription();
            }
        };

        class CommandGodMode: public Console::Command {
        public:
            CommandGodMode(const vector<Paintown::Object *> & players):
            players(players),
            last(false){
            }

            const vector<Paintown::Object*> & players;
            bool last;
                
            string getDescription() const {
                return "god-mode [on] [off] - enable invincibility";
            }

            void set(bool what){
                last = what;
                for (vector<Paintown::Object*>::const_iterator it = players.begin(); it != players.end(); it++){
                    Paintown::Character * maybe = (Paintown::Character*) *it;
                    if (maybe->isPlayer()){
                        Paintown::Player * player = (Paintown::Player*) maybe;
                        player->setInvincible(what);
                    }
                }
            }

            string act(const string & line){
                string argument = Util::captureRegex(line, Util::Regex("god-mode\\s+(\\w+)"), 0);
                if (argument == "on"){
                    set(true);
                    return "God mode enabled";
                } else if (argument == "off"){
                    set(false);
                    return "God mode disabled";
                } else {
                    ostringstream out;
                    out << "God mode is " << (last ? "enabled" : "disabled") << ". Give either 'on' or 'off' as an argument";
                    return out.str();
                }
                return "";
            }
        };

        console.addCommand("quit", Util::ReferenceCount<Console::Command>(new CommandQuit()));
        console.addCommand("help", Util::ReferenceCount<Console::Command>(new CommandHelp(console)));
        console.addAlias("?", "help");
        console.addCommand("memory", Util::ReferenceCount<Console::Command>(new CommandMemory()));
        console.addCommand("god-mode", Util::ReferenceCount<Console::Command>(new CommandGodMode(players)));
        console.addCommand("clear", Util::ReferenceCount<Console::Command>(new CommandClear(console)));
        console.addCommand("stage", Util::ReferenceCount<Console::Command>(new CommandStage()));
    }
    // bool toggleConsole = false;
    // const int consoleKey = Keyboard::Key_TILDE;

    world.getEngine()->createWorld(world);

    // int game_time = 100;
    // Global::second_counter = 0;

    /* thrown when the player runs out of lives */
    class LoseException: Exception::Base {
    public:
        LoseException():
        Exception::Base(__FILE__, __LINE__){
        }
    };

    /* user presses ESC in the game and brings up the menu. ESC in the menu
     * should go back to the game. selecting the 'exit' option should quit the game
     * to the main menu.
     */ 

    struct GameState{
        GameState():
            menu_quit(false),
            helpTime(0),
            pressed(0),
            done(false),
            show_fps(false),
            takeScreenshot(false){
            }

        bool menu_quit;
        double helpTime;
        int pressed;
        bool done;
        bool show_fps;
        bool takeScreenshot;
    };

    class Logic: public Util::Logic {
    public:
        Logic(const vector<Paintown::Object*> & players, World & world, Console::Console & console, GameState & state, Token * menuData):
        runCounter(0),
        gameSpeed(startingGameSpeed()),
        players(players),
        helped(false),
        world(world),
        console(console),
        state(state),
        menuData(menuData){
            if (Global::getDebug() > 0){
                input.set(Keyboard::Key_MINUS_PAD, 2, false, Game::Slowdown);
                input.set(Keyboard::Key_PLUS_PAD, 2, false, Game::Speedup);
                input.set(Keyboard::Key_F4, 0, true, Game::ReloadLevel);
                input.set(Keyboard::Key_F8, 200, false, Game::KillAllHumans);
            }

            input.set(Keyboard::Key_P, 10, false, Game::Pause);
            input.set(Keyboard::Key_TAB, 10, false, Game::MiniMaps);
            input.set(Keyboard::Key_TILDE, 25, false, Game::Console);
            input.set(Keyboard::Key_ESC, 0, false, Game::Quit);
            input.set(Joystick::Quit, 0, false, Game::Quit);
            input.set(Keyboard::Key_F1, 0, false, Game::ShowHelp);
            input.set(Keyboard::Key_F9, 20, false, Game::ShowFps);
            input.set(Keyboard::Key_F12, 10, false, Game::Screenshot);
        }

        InputMap<Game::Input> input;
        double runCounter;
        double gameSpeed;
        const vector<Paintown::Object*> & players;
        bool helped;
        World & world;
        Util::EventManager eventManager;
        Console::Console & console;
        GameState & state;
        Token * menuData;

        virtual bool done(){
            return state.done || state.menu_quit;
        }

        double ticks(double system){
            return system * gameSpeed * Global::ticksPerSecond(Ticks);
        }

        void doInput(GameState & state, bool & force_quit){
            if (console.isActive()){
                try{
                    console.doInput();
                } catch (const Exception::Return & r){
                    force_quit = true;
                }
            } else {
                vector<InputMap<Game::Input>::InputEvent> events = InputManager::getEvents(input, InputSource());

                bool pressed = false;
                for (vector<InputMap<Game::Input>::InputEvent>::iterator it = events.begin(); it != events.end(); it++){
                    const InputMap<Game::Input>::InputEvent & event = *it;
                    if (!event.enabled){
                        continue;
                    }

                    if (event[Game::ShowHelp]){
                        helped = true;
                        state.helpTime = state.helpTime < 260 ? 260 : state.helpTime;
                    }

                    if (event[Game::ShowFps]){
                        state.show_fps = ! state.show_fps;
                    }

                    if (event[Game::Console]){
                        console.toggle();
                    }

                    state.takeScreenshot = event[Game::Screenshot];

                    if (event[Game::Pause]){
                        /*
                           paused = ! paused;
                           world.addMessage(paused ? pausedMessage() : unpausedMessage());
                           draw = true;
                           */
                        world.changePause();
                    }

                    if (event[Game::MiniMaps]){
                        world.drawMiniMaps( ! world.shouldDrawMiniMaps() );
                    }

                    /*
                       if ( key[ Keyboard::Key_F8 ] ){
                       world.killAllHumans( player );
                       }
                       */

                    if ( Global::getDebug() > 0 ){
                        const double SPEED_INC = 0.02;
                        if (event[Game::Speedup]){
                            gameSpeed += SPEED_INC;
                            Global::debug( 3 ) << "Game speed " << gameSpeed << endl;
                        }

                        if (event[Game::Slowdown]){
                            gameSpeed -= SPEED_INC;
                            if ( gameSpeed < SPEED_INC ){
                                gameSpeed = SPEED_INC;
                            }
                            Global::debug( 3 ) << "Game speed " << gameSpeed << endl;
                        }

                        if (event[Game::NormalSpeed]){
                            gameSpeed = 1;
                            Global::debug( 3 ) << "Game speed " << gameSpeed << endl;
                        }

                        if (event[Game::ReloadLevel]){
                            try{
                                world.reloadLevel();
                            } catch ( const LoadException & le ){
                                Global::debug( 0 ) << "Could not reload world: " << le.getTrace() << endl;
                            }
                        }
                    }

                    force_quit |= event[Game::Quit];
                }

                if (!pressed){
                    state.pressed = 0;
                } else {
                    state.pressed += 1;
                    if (state.pressed > 100){
                        state.pressed = 100;
                        state.helpTime = 260;
                    }
                }

            }
        }

        vector<Paintown::Player*> getPlayers(){
            vector<Paintown::Player*> out;

            for (vector<Paintown::Object*>::const_iterator it = players.begin(); it != players.end(); it++){
                Paintown::Character * maybe = (Paintown::Character*) *it;
                if (maybe->isPlayer()){
                    out.push_back((Paintown::Player*) maybe);
                }
            }

            return out;
        }

        using Util::Logic::run;
        void run(GameState & state){
            world.act();
            console.act();

            if (!world.respawnPlayers(players)){
                throw LoseException();
            }

            if (state.helpTime > 0){
                if (helped){
                    state.helpTime -= 2;
                } else {
                    state.helpTime -= 0.5;
                }
            }

            bool force_quit = false;
            doInput(state, force_quit);

            state.done |= world.finished();
            if (force_quit){
                /*
                Paintown::PlayerCommon * maybe = (Paintown::PlayerCommon*) players[0];
                if (maybe->isPlayer()){
                    Paintown::Player * player = (Paintown::Player*) players[0];
                    state.menu_quit = state.menu_quit || doMenu(menuData, player);
                }
                */
                state.menu_quit = state.menu_quit || doMenu(menuData, getPlayers());
            }
        }

        void run(){
            run(state);
        }

        void waitForQuit(){
            InputManager::waitForRelease(input, InputSource(), Game::Quit);
        }
    };

    class Draw: public Util::Draw {
    public:
        Draw(Console::Console & console, World & world, GameState & state):
        console(console),
        world(world),
        state(state){
        }

        Console::Console & console;
        World & world;
        GameState & state;

        void draw(const Graphics::Bitmap & screen_buffer){
            run(screen_buffer, state);
        }

        void run(const Graphics::Bitmap & screen_buffer, GameState & state){
            Graphics::RestoreState graphicsState;
            /* FIXME: replace these constants */
            Graphics::StretchedBitmap work(320, 240, screen_buffer, Graphics::qualityFilterName(Configuration::getQualityFilter()));
            Graphics::TranslatedBitmap screen(world.getX(), world.getY(), screen_buffer);
            // updateFrames();

            work.start();
            work.clear();
            world.draw(&work);

            work.finish();
            // work.Stretch(screen_buffer);
            FontRender * render = FontRender::getInstance();
            render->render(&screen_buffer, work.getScaleWidth() / 2, work.getScaleHeight() / 2);

            const Font & font = Font::getFont(Global::DEFAULT_FONT, (int) (20 * work.getScaleWidth() / 2), (int)(20 * work.getScaleHeight() / 2));

            if (state.helpTime > 0){
                int x = (int)(100 * work.getScaleWidth() / 2);
                int y = (screen_buffer.getHeight() / 5 * work.getScaleHeight() / 2);
                Graphics::Color color = Graphics::makeColor(255, 255, 255);
                Graphics::Bitmap::transBlender( 0, 0, 0, (int)(state.helpTime > 255 ? 255 : state.helpTime));
                drawHelp(font, x, y, color, screen_buffer.translucent());
            }

            if (state.show_fps){
                font.printf((int)(screen_buffer.getWidth() - 120 * work.getScaleWidth() / 2), (int)(10 * work.getScaleHeight() / 2), Graphics::makeColor(255,255,255), screen_buffer, "FPS: %0.2f", 0, getFps());
            }
            console.draw(screen_buffer);

            /* getX/Y move when the world is quaking */
            // screen_buffer.BlitToScreen(world.getX(), world.getY());
            screen.BlitToScreen();

            if (state.takeScreenshot){
                state.takeScreenshot = false;
                doTakeScreenshot(work);
            }
        }

        void showScreenshots(const Graphics::Bitmap & screen_buffer){
            screen_buffer.clear();
            Sound snapshot(Storage::instance().find(Filesystem::RelativePath("sounds/snapshot.wav")).path());
            Graphics::StretchedBitmap work(320, 240, screen_buffer);
            for (deque<Graphics::Bitmap*>::const_iterator it = world.getScreenshots().begin(); it != world.getScreenshots().end(); it++){
                Graphics::Bitmap * shot = *it;
                int angle = Util::rnd(-6, 6);

                /*
                   int gap = 4;
                   int x = Util::rnd(work.getWidth() - 2 * work.getWidth() / gap) + work.getWidth() / gap;
                   int y = Util::rnd(work.getHeight() - 2 * work.getHeight() / gap) + work.getHeight() / gap;
                   double scale = 1.0 - log(world.getScreenshots().size()+1) / 9.0;
                   shot->greyScale().drawPivot(shot->getWidth() / 2, shot->getHeight() / 2, x, y, angle, scale, work);
                   */

                int x = work.getWidth() / 2;
                int y = work.getHeight() / 2;
                double scale = 0.9;
                work.start();
                shot->border(0, 1, Graphics::makeColor(64,64,64));
                shot->greyScale().drawPivot(shot->getWidth() / 2, shot->getHeight() / 2, x, y, angle, scale, work);
                work.finish();
                screen_buffer.BlitToScreen();
                snapshot.play();
                Util::restSeconds(1.5);
            }
            if (world.getScreenshots().size() > 0){
                Util::restSeconds(2);
            }

        }
    };
    
    TokenReader reader;
    Token * menuData = reader.readTokenFromFile(*Storage::instance().open(Paintown::Mod::getCurrentMod()->find(Filesystem::RelativePath("menu/in-game.txt"))));

    bool finish = true;
    GameState state;
    Logic logic(players, world, console, state, menuData);
    Draw drawer(console, world, state);
    // state.helpTime = helpTime;

    /* FIXME: begin/destroy world in a constructor/destructor so we can handle
     * exceptions being thrown, like StageNext.
     */
    world.begin();

    try{
        /* run the game */
        Util::standardLoop(logic, drawer);
        if (!state.menu_quit){
            drawer.showScreenshots(getScreen());
            world.end();
        }
    } catch (const LoseException & lose){
        /* TODO: maybe show a lose cutscene for each player */

        fadeOut(getScreen(), "Game over");
        finish = false;
    }

    world.getEngine()->destroyWorld(world);

    if (state.menu_quit){
        logic.waitForQuit();
        finish = false;
    }

    return finish;
}

static string funnyGo(){
    switch (Util::rnd(10)){
        case 0 : return "Calculating reverse theorems";
        case 1 : return "Is that a weasel?";
        case 2 : return "Rebuffering..";
        case 3 : return "Get to the choppaa!!!";
        case 4 : return "Generating dual proxies";
        case 5 : return "Keep elbows in at all times";
        case 6 : return "I LIKE TURTLES";
        default : return "Go!";
    }
}

static void initializePlayers(const vector<Paintown::Object*> & players){
    for (vector<Paintown::Object *>::const_iterator it = players.begin(); it != players.end(); it++){
        Paintown::Character * playerX = (Paintown::Character *) *it;
        playerX->setTrails(0, 0);
        playerX->setY(200);
        /* setMoving(false) sets all velocities to 0 */
        playerX->setMoving(false);
        /* but the player is falling so set it back to true */
        playerX->setMoving(true);

        playerX->setStatus(Paintown::Status_Falling);
        if (playerX->isPlayer()){
            ((Paintown::Player*) playerX)->resetInput();
        }
    }
}

struct GameData{
    GameData(vector<Paintown::Object*> players, const Filesystem::AbsolutePath & path):
        world(players, path),
        players(players){
        }

    AdventureWorld world;
    vector<Paintown::Object*> players;
};

static void showCutscene(const Filesystem::RelativePath & path){
    if (path != Filesystem::RelativePath("")){
        try{
            Gui::CutScene intro(Storage::instance().find(path));
            /* FIXME: hack */
            // intro.setResolution(320, 240);
            intro.playAll();
        } catch (const Filesystem::NotFound & fail){
            Global::debug(0) << "Could not find file while trying to play cutscene " << path.path() << ": " << fail.getTrace() << std::endl;
        } catch (const Filesystem::Exception & fail){
            Global::debug(0) << "Could not play cutscene " << path.path() << ": " << fail.getTrace() << std::endl;
        } catch (const Exception::Base & fail){
            Global::debug(0) << "Could not play cutscene " << path.path() << ": " << fail.getTrace() << std::endl;
        }
    }
}

static void showEnding(Paintown::Player * player){
    showCutscene(player->getEndingFile());
}

static void showIntro(Paintown::Player * player){
    showCutscene(player->getIntroFile());
}

static void showIntros(const vector<Paintown::Object*> & players){
    for (vector<Paintown::Object*>::const_iterator it = players.begin(); it != players.end(); it++){
        Paintown::Character * maybe = (Paintown::Character*) *it;
        if (maybe->isPlayer()){
            showIntro((Paintown::Player*) maybe);
        }
    }
}

static void showEndings(const vector<Util::Future<Paintown::Object*> * > & futurePlayers){
    for (vector<Util::Future<Paintown::Object*>*>::const_iterator fit = futurePlayers.begin(); fit != futurePlayers.end(); fit++){
        Util::Future<Paintown::Object*> * future = *fit;
        Paintown::Object * player = future->get();
        if (player != NULL){
            showEnding((Paintown::Player*) player);
        }
    }
}

static void realGame(const vector<Util::Future<Paintown::Object*> * > & futurePlayers, const Level::LevelInfo & levelInfo, const string & level, void (*setup_players)(const vector<Paintown::Object*> & players), bool firstLevel){

    class GameContext: public Loader::LoadingContext {
    public:
        GameContext(const vector<Util::Future<Paintown::Object*> * > & futurePlayers, const Filesystem::RelativePath & path, void (*setup_players)(const vector<Paintown::Object*> & players)):
            data(NULL),
            futurePlayers(futurePlayers),
            path(path),
            failed(NULL),
            setup_players(setup_players){
            }

        virtual ~GameContext(){
            /* who will delete the players contained in the data? the futures
             * passed in as `futurePlayers'
             */
        }

        virtual void failure(){
            if (failed != NULL){
                throw LoadException(*failed);
            }
        }

        virtual void load(){
            try{
                vector<Paintown::Object*> players;
                /* start the futures so they can run in parallel */
                for (vector<Util::Future<Paintown::Object*>*>::const_iterator fit = futurePlayers.begin(); fit != futurePlayers.end(); fit++){
                    Util::Future<Paintown::Object*> * future = *fit;
                    future->start();
                }
                /* then get them */
                for (vector<Util::Future<Paintown::Object*>*>::const_iterator fit = futurePlayers.begin(); fit != futurePlayers.end(); fit++){
                    Util::Future<Paintown::Object*> * future = *fit;
                    players.push_back(future->get());
                }
                setup_players(players);
                data = new GameData(players, Storage::instance().find(path));
            } catch (const LoadException & exception){
                failed = new LoadException(exception);
            } catch (const Exception::Base & exception){
                failed = new LoadException(__FILE__, __LINE__, exception, "Could not load game data");
            }
        }

        World & getWorld(){
            if (data == NULL){
                throw LoadException(__FILE__, __LINE__, "World didn't load");
            }
            return data->world;
        }

        vector<Paintown::Object*> & getPlayers(){
            if (data == NULL){
                throw LoadException(__FILE__, __LINE__, "Players didn't load");
            }
            return data->players;
        }

        Util::ReferenceCount<GameData> data;
        vector<Util::Future<Paintown::Object*> * > futurePlayers;
        Filesystem::RelativePath path;
        Util::ReferenceCount<LoadException> failed;
        void (*setup_players)(const vector<Paintown::Object*> & players);
    };

    /* Do some cleanup when this function leaves (either normally or due to an exception)
     */
    class Cleanup{
    public:
        virtual ~Cleanup(){
            Keyboard::popRepeatState();
            ObjectFactory::destroy();
            HeartFactory::destroy();
        }
    };

    Cleanup cleanup;

    bool gameState = true;
    { /* force scope so the context is destroyed before the factories */
        TimeDifference loadingTime;
        loadingTime.startTime();

        Global::clearInfo();
        Global::info("Setting up world");
        GameContext context(futurePlayers, Filesystem::RelativePath(level), setup_players);
        Loader::loadScreen(context, Level::convert(levelInfo));
        context.failure();
        Global::info("World setup");
        Global::info(funnyGo());

        Keyboard::pushRepeatState(false);

        // Music::changeSong();

        initializePlayers(context.getPlayers());

        loadingTime.endTime();
        std::ostringstream loadingDescription;
        loadingDescription << "Loading level " << level << " took";
        Global::debug(0) << loadingTime.printTime(loadingDescription.str()) << std::endl;

        /* only show intros at the start of the entire game, not for each level */
        if (firstLevel){
            showIntros(context.getPlayers());
        }

        gameState = playLevel(context.getWorld(), context.getPlayers());
    }

    if (! gameState){
        throw Exception::Return(__FILE__, __LINE__);
    }
}

static void doNothingSpecial(const vector<Paintown::Object*> & objects){
}

/* assume all the objects are Player*'s. I guess in the future they could be
 * BuddyPlayer's as well.
 */
static void setupLocalPlayers(const vector<Paintown::Object*> & objects){
    for (vector<Paintown::Object*>::const_iterator i1 = objects.begin(); i1 != objects.end(); i1++){
        for (vector<Paintown::Object*>::const_iterator i2 = objects.begin(); i2 != objects.end(); i2++){
            if (i1 != i2){
                Paintown::Player * player1 = (Paintown::Player*) *i1;
                Paintown::Player * player2 = (Paintown::Player*) *i2;
                player1->bindTo(player2);
                player2->bindTo(player1);
            }
        }
    }
}

static void doRealGame(const vector<Util::Future<Paintown::Object*> * > & futurePlayers, const Level::LevelInfo & levelInfo, void (*setup_players)(const vector<Paintown::Object*> & players)){

    levelInfo.playIntro();

    bool first = true;
    const std::vector<std::string> & levels = levelInfo.getLevels();
    for (int level = 0; level < levels.size(); level++){
        try{
            realGame(futurePlayers, levelInfo, levels[level], setup_players, first);
        } catch (const StageNext & next){
        } catch (const StagePrevious & previous){
            level -= 2;
            if (level == -2){
                level = -1;
            }
        } catch (const StageIndex & index){
            /* Stage index starts at 1, so we need to subtract 2 to get a 0-based index */
            if (index.index >= 1 && index.index <= levels.size()){
                level = index.index - 2;
            }
        }

        first = false;
    }

    /* First player endings */
    showEndings(futurePlayers);

    /* Then game ending */
    levelInfo.playEnding();
}

void realGame(const vector<Util::Future<Paintown::Object*> * > & futurePlayers, const Level::LevelInfo & levelInfo){
    doRealGame(futurePlayers, levelInfo, doNothingSpecial);
}

void realGameLocal(const vector<Util::Future<Paintown::Object*> * > & futurePlayers, const Level::LevelInfo & levelInfo){
    doRealGame(futurePlayers, levelInfo, setupLocalPlayers);
}

void fadeOut( const Graphics::Bitmap & work, const string & message ){
    Graphics::Bitmap::transBlender(0, 0, 0, 128);
    work.applyTrans(Graphics::makeColor(0, 0, 0));

    const Font & f = Font::getFont(Global::DEFAULT_FONT, 50, 50 );
    f.printf(200, 200, Graphics::makeColor(255, 0, 0), work, message, 0);
    work.BlitToScreen();

    Util::rest(2000);
}

}
