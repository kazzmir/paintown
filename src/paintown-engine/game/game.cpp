#include "util/bitmap.h"
#include "util/trans-bitmap.h"
#include "util/stretch-bitmap.h"

#include "game.h"
#include "mod.h"
#include "move-list.h"
#include "util/music.h"
#include "util/funcs.h"
#include "util/font.h"
#include "util/pointer.h"
#include "util/parameter.h"
#include "util/gui/coordinate.h"
#include "util/gui/popup-box.h"
#include "menu/menu.h"
#include "menu/optionfactory.h"
#include "menu/menu_option.h"
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
#include "exceptions/shutdown_exception.h"
#include "exceptions/exception.h"
#include "util/file-system.h"
#include "util/system.h"
#include "util/events.h"
#include "util/loading.h"
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

struct Background{
    string path;
    int z;

    Background():z(0){}
};
        
static double startingGameSpeed(){
    return 1.0;
}

/*
static void stopLoading(Util::Thread::Id thread){
    if (show_loading_screen){
        Loader::stopLoading(thread);
    }
}

static void startLoading(Util::Thread::Id * thread, const Level::LevelInfo & info ){
    if (show_loading_screen){
        Loader::startLoading(thread, (void*) &info);
    }
}
*/

static Network::Message removeMessage(int id){
    Network::Message message;

    message.id = 0;
    message << World::REMOVE;
    message << id;

    return message;
}

static vector<Background> readBackgrounds( const Filesystem::AbsolutePath & path ){
    vector<Background> backgrounds;

    try{
        TokenReader reader( path.path() + "/bgs.txt" );
        Token * head = reader.readToken();

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
    font.printf( x, y, color, buffer, "Up: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getUp() ) );
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Down: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getDown() ) );
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Left: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getLeft() ) );
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Right: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getRight() ) );
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Jump: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getJump() ) );
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Attack1: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getAttack1() ) );
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Attack2: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getAttack2() ) );
    y += font.getHeight() + 1;
    font.printf( x, y, color, buffer, "Attack3: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getAttack3() ) );
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
    string file = findNextFile("paintown-screenshot.bmp");
    Global::debug(2) << "Saved screenshot to " << file << endl;
    work.save(file);
}

/* returns false if players cannot be respawned due to running out of lives */
static bool respawnPlayers(const vector<Paintown::Object*> & players, World & world){
    bool alive = false;
    for (vector<Paintown::Object *>::const_iterator it = players.begin(); it != players.end(); it++){
        Paintown::PlayerCommon * player = (Paintown::PlayerCommon *) *it;
        if (player->getHealth() <= 0){
            if (player->spawnTime() == 0){

                /* if the player has at least 1 life left then make them lose a life
                 * and respawn them. if they have infinite lives enabled then they
                 * might have 1 life but deathReset() won't actually decrement the
                 * number of lives.
                 * if the player is reduced to 0 lives then remove them from the
                 * world entirely.
                 */
                if (player->getLives() >= 1){
                    player->deathReset();
                    if (player->getLives() > 0){
                        if (player->isPlayer()){
                            alive = true;
                        }
                        world.addMessage(removeMessage(player->getId()));
                        world.addObject(player);
                        world.addMessage(player->getCreateMessage());
                        world.addMessage(player->movedMessage());
                        world.addMessage(player->animationMessage());
                    } else {
                        world.addMessage(removeMessage(player->getId()));
                    }
                }
            }
        } else {
            alive = true;
        }
    }
    return alive;
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
        return Paintown::OptionFactory::getOption(parent, data);
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
    context.addBackground(Graphics::Bitmap(screen_buffer, true).scaleTo(screen_buffer.getWidth(), screen_buffer.getHeight()));
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

bool playLevel( World & world, const vector< Paintown::Object * > & players){
    /* 150 pixel tall console */
    Console::Console console(150);
    {
        class CommandQuit: public Console::Command{
        public:
            CommandQuit(){
            }

            string act(){
                throw ShutdownException();
            }
        };

        class CommandMemory: public Console::Command {
        public:
            CommandMemory(){
            }

            string act(){
                ostringstream out;
                out << "Memory usage: " << Util::niceSize(System::memoryUsage()) << "\n";
                return out.str();
            }
        };

        class CommandHelp: public Console::Command{
        public:
            CommandHelp(){
            }

            string act(){
                ostringstream out;
                out << "quit - quit the game entirely" << "\n";
                out << "memory - current memory usage" << "\n";
                out << "help - this help menu";
                return out.str();
            }
        };

        console.addCommand("quit", new CommandQuit());
        console.addCommand("help", new CommandHelp());
        console.addCommand("memory", new CommandMemory());
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
            return system * gameSpeed * Global::LOGIC_MULTIPLIER;
        }

        void doInput(GameState & state, bool & force_quit){
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

            try{
                console.doInput();
            } catch (const Exception::Return & r){
                force_quit = true;
            }
        }

        vector<Paintown::Player*> getPlayers(){
            vector<Paintown::Player*> out;

            for (vector<Paintown::Object*>::const_iterator it = players.begin(); it != players.end(); it++){
                Paintown::PlayerCommon * maybe = (Paintown::PlayerCommon*) *it;
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

            if (!respawnPlayers(players, world)){
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
            InputManager::waitForRelease(input, Game::Quit);
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

        void run(const Graphics::Bitmap & screen_buffer, const GameState & state){
            Graphics::RestoreState graphicsState;
            /* FIXME: replace these constants */
            Graphics::StretchedBitmap work(320, 240, screen_buffer);
            Graphics::TranslatedBitmap screen(world.getX(), world.getY(), screen_buffer);
            // updateFrames();

            work.start();
            work.clear();
            world.draw(&work);

            work.finish();
            // work.Stretch(screen_buffer);
            FontRender * render = FontRender::getInstance();
            render->render(&screen);

            const Font & font = Font::getFont(Global::DEFAULT_FONT, 20, 20 );

            if (state.helpTime > 0){
                int x = 100;
                int y = screen.getHeight() / 5;
                Graphics::Color color = Graphics::makeColor( 255, 255, 255 );
                Graphics::Bitmap::transBlender( 0, 0, 0, (int)(state.helpTime > 255 ? 255 : state.helpTime));
                drawHelp( font, x, y, color, screen.translucent());
            }

            if (state.show_fps){
                font.printf(screen.getWidth() - 120, 10, Graphics::makeColor(255,255,255), screen_buffer, "FPS: %0.2f", 0, getFps());
            }
            console.draw(screen);

            /* getX/Y move when the world is quaking */
            // screen_buffer.BlitToScreen(world.getX(), world.getY());
            screen.BlitToScreen();

            if (state.takeScreenshot){
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
    Token * menuData = reader.readToken(Paintown::Mod::getCurrentMod()->find(Filesystem::RelativePath("menu/in-game.txt")).path());

    bool finish = true;
    GameState state;
    Logic logic(players, world, console, state, menuData);
    Draw drawer(console, world, state);
    // state.helpTime = helpTime;

    world.begin();

    try{
        /* run the game */
        Util::standardLoop(logic, drawer);
        if (!state.menu_quit){
            drawer.showScreenshots(getScreen());
        }
    } catch (const LoseException & lose){
        fadeOut(getScreen(), "You lose");
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
        Paintown::PlayerCommon * playerX = (Paintown::PlayerCommon *) *it;
        playerX->setTrails(0, 0);
        playerX->setY(200);
        /* setMoving(false) sets all velocities to 0 */
        playerX->setMoving(false);
        /* but the player is falling so set it back to true */
        playerX->setMoving(true);

        playerX->setStatus(Paintown::Status_Falling);
        playerX->resetInput();
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

static void realGame(const vector<Util::Future<Paintown::Object*> * > & futurePlayers, const Level::LevelInfo & levelInfo, const string & level, void (*setup_players)(const vector<Paintown::Object*> & players)){

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
                for (vector<Util::Future<Paintown::Object*>*>::const_iterator fit = futurePlayers.begin(); fit != futurePlayers.end(); fit++){
                    Util::Future<Paintown::Object*> * future = *fit;
                    players.push_back(future->get());
                }
                setup_players(players);
                data = new GameData(players, Storage::instance().find(path));
            } catch (const LoadException & exception){
                failed = new LoadException(exception);
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

    bool gameState = true;
    { /* force scope so the context is destroyed before the factories */
        Global::clearInfo();
        Global::info("Setting up world");
        GameContext context(futurePlayers, Filesystem::RelativePath(level), setup_players);
        Loader::loadScreen(context, levelInfo);
        context.failure();
        Global::info("World setup");
        Global::info(funnyGo());

        Keyboard::pushRepeatState(false);

        // Music::changeSong();

        initializePlayers(context.getPlayers());

        gameState = playLevel(context.getWorld(), context.getPlayers());
    }

    Keyboard::popRepeatState();
    ObjectFactory::destroy();
    HeartFactory::destroy();

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

void realGame(const vector<Util::Future<Paintown::Object*> * > & futurePlayers, const Level::LevelInfo & levelInfo){
    for (vector<string>::const_iterator it = levelInfo.getLevels().begin(); it != levelInfo.getLevels().end(); it++){
        realGame(futurePlayers, levelInfo, *it, doNothingSpecial);
    }
}

void realGameLocal(const vector<Util::Future<Paintown::Object*> * > & futurePlayers, const Level::LevelInfo & levelInfo){
    for (vector<string>::const_iterator it = levelInfo.getLevels().begin(); it != levelInfo.getLevels().end(); it++){
        realGame(futurePlayers, levelInfo, *it, setupLocalPlayers);
    }
}

#if 0
void realGame(const vector<Util::Future<Paintown::Object*> * > & futurePlayers, const Level::LevelInfo & levelInfo){

    /* disables buffer input on entry, enables buffering on function exit */
    /*
    class InputBuffer{
    public:
        InputBuffer(){
            InputManager::disableBufferInput();
        }

        ~InputBuffer(){
            InputManager::enableBufferInput();
        }
    };
    */

    // InputBuffer buffer;

    // Level::LevelInfo levelInfo = Level::readLevels( levelFile );

    // global_debug = true;

    int showHelp = 800;
    for ( vector<string>::const_iterator it = levelInfo.getLevels().begin(); it != levelInfo.getLevels().end(); it++ ){
        Util::Thread::Id loading_screen_thread;
        startLoading(&loading_screen_thread, levelInfo);

        bool gameState = false;
        try {
            // vector< Object * > players;
            // players.push_back( player );

            /*
               Global::debug(0) << "Memory debug loop" << endl;
               for (int i = 0; i < 1000; i++){
               World world( players, *it );
               ObjectFactory::destroy();
               HeartFactory::destroy();
               }
               */

            Global::info("Setting up world");
            vector<Paintown::Object*> players;
            for (vector<Util::Future<Paintown::Object*>*>::const_iterator fit = futurePlayers.begin(); fit != futurePlayers.end(); fit++){
                Util::Future<Paintown::Object*> * future = *fit;
                players.push_back(future->get());
            }

            AdventureWorld world(players, Filesystem::find(Filesystem::RelativePath(*it)));
            Global::info("World setup");
            Global::info(funnyGo());

            Music::pause();
            Music::fadeIn( 0.3 );
            Music::loadSong(Filesystem::getFiles(Filesystem::find(Filesystem::RelativePath("music/")), "*"));
            Music::play();

            for ( vector< Paintown::Object * >::const_iterator it = players.begin(); it != players.end(); it++ ){
                Paintown::Player * playerX = (Paintown::Player *) *it;
                playerX->setTrails(0, 0);
                playerX->setY(200);
                /* setMoving(false) sets all velocities to 0 */
                playerX->setMoving(false);
                /* but the player is falling so set it back to true */
                playerX->setMoving(true);

                playerX->setStatus(Paintown::Status_Falling);
            }

            stopLoading(loading_screen_thread);

            gameState = playLevel(world, players, showHelp);
            showHelp = 0;
        } catch ( const LoadException & le ){
            Global::debug( 0 ) << "Could not load " << *it << " because " << le.getTrace() << endl;
            /* if the level couldn't be loaded turn off
             * the loading screen
             */
            stopLoading( loading_screen_thread );
        } catch (const ShutdownException & se){
            stopLoading(loading_screen_thread);
            throw se;
        }

        ObjectFactory::destroy();
        HeartFactory::destroy();

        if ( ! gameState ){
            return;
        }

        // fadeOut( "Next level" );
    }

    /* fix.. */
    // fadeOut( "You win!" );
}
#endif

#if 0
/* use MenuGlobal::doLevelMenu instead */
const Level::LevelInfo selectLevelSet( const string & base ){
    return MenuGlobals::doLevelMenu(base, NULL);

        Bitmap background( Global::titleScreen() );
        // Bitmap::Screen->Blit( Global::titleScreen() );

        // Bitmap background( Util::getDataPath() + "/paintown-title.png" );
        int fontY = 20;
        const Font & font = Font::getFont( Filesystem::find(DEFAULT_FONT), 20, fontY );
        vector<string> possible = Util::getFiles( base + "/", "*.txt" );
        for ( vector<string>::iterator it = possible.begin(); it != possible.end(); it++ ){
            string & path = *it;
            /*
            path.erase(0, Util::getDataPath().length() + 1);
            */
            path = Filesystem::cleanse(path);
        }

        if ( possible.size() == 0 ){
                return "no-files!!!";
        }

        if (possible.size() == 1){
            return Filesystem::find(possible[0]);
        }

        /*
        for ( vector< string >::iterator it = possible.begin(); it != possible.end(); it++ ){
                string & s = *it;
                s.insert( 0, base + "/" );
        }
        */
        int choose = 0;

        font.printf( 180, (int)(200 - fontY * 1.2), Bitmap::makeColor( 255, 255, 255 ), background, "Select a set of levels to play", 0 );
        for ( unsigned int i = 0; i < possible.size(); i++ ){
                int yellow = Bitmap::makeColor( 255, 255, 0 );
                int white = Bitmap::makeColor( 255, 255, 255 );
                unsigned int color = i == (unsigned) choose ? yellow : white;
                font.printf( 200, (int)(200 + i * fontY * 1.2), color, background, possible[ i ], 0 );
        }
        background.BlitToScreen();

        Keyboard key;
        bool done = false;

        key.setDelay( Keyboard::Key_UP, LAZY_KEY_DELAY );
        key.setDelay( Keyboard::Key_DOWN, LAZY_KEY_DELAY );
        Global::speed_counter = 0;

        key.wait();

        while ( ! done ){
                
                key.poll();
                bool draw = false;
                if ( Global::speed_counter > 0 ){
                        double think = Global::speed_counter;

                        while ( think > 0 ){
                                think--;

                                if ( key[ Keyboard::Key_UP ] ){
                                        draw = true;
                                        choose = (choose - 1 + possible.size()) % possible.size();
                        }

                                if ( key[ Keyboard::Key_DOWN ] ){
                                        draw = true;
                                        choose = (choose + 1 + possible.size()) % possible.size();
                                }

                                if ( key[ Keyboard::Key_ENTER ] ){
                                        return Filesystem::find(possible[ choose ]);
                                }

                                if ( key[ Keyboard::Key_ESC ] ){
                                        throw ReturnException();
                                }
                        }

                        Global::speed_counter = 0;
                }

                if ( draw ){
                        for ( unsigned int i = 0; i < possible.size(); i++ ){
                                int yellow = Bitmap::makeColor( 255, 255, 0 );
                                int white = Bitmap::makeColor( 255, 255, 255 );
                                unsigned int color = i == (unsigned) choose ? yellow : white;
                                font.printf( 200, (int)(200 + i * fontY * 1.2), color, background, possible[ i ], 0 );
                        }
                        background.BlitToScreen();
                }
                
                while ( Global::speed_counter == 0 ){
                        Util::rest( 1 );
                        key.poll();
                }
        }

        return "nothing-selected";
}
#endif

void fadeOut( const Graphics::Bitmap & work, const string & message ){
    /*
    Graphics::Bitmap dark( GFX_X, GFX_Y );
    dark.clear();
    */
    Graphics::Bitmap::transBlender(0, 0, 0, 128);
    work.applyTrans(Graphics::makeColor(0, 0, 0));

    const Font & f = Font::getFont(Global::DEFAULT_FONT, 50, 50 );
    f.printf(200, 200, Graphics::makeColor(255, 0, 0), work, message, 0);
    work.BlitToScreen();

    Util::rest( 2000 );
}

static bool closeFloat(double a, double b){
    const double epsilon = 0.0001;
    return fabs(a-b) < epsilon;
}

/* TODO: deprecated */
#if 0
void playVersusMode( Paintown::Character * player1, Paintown::Character * player2, int round ){

        player1->setY( 0 );
        player2->setY( 0 );
        player1->setX( 0 );
        player2->setX( 400 );

        player1->setMaxHealth( 150 );
        player2->setMaxHealth( 150 );
        player1->setHealth( 150 );
        player2->setHealth( 150 );

        // Keyboard key;

        // key.setDelay( Keyboard::Key_P, 100 );

        bool done = false;
        bool paused = false;
        double runCounter = 0;
        double gameSpeed = startingGameSpeed();

        vector< Background > backgrounds;
        backgrounds = readBackgrounds(Filesystem::find(Filesystem::RelativePath("bgs/versus/")));

        Bitmap background( 640, 480 );
        int z = 400;
        if ( backgrounds.size() != 0 ){
                Background use = backgrounds[ Util::rnd( backgrounds.size() ) ];
                Bitmap b(Filesystem::find(Filesystem::RelativePath(use.path)).path());
                b.Stretch(background);
                z = use.z;
        }
        
        VersusWorld world( z, player1, player2 );
        
        Bitmap work( 640, 480 );
        // Bitmap work( GFX_X, GFX_Y );
        Bitmap screen_buffer( GFX_X, GFX_Y );

        Music::changeSong();
        /*
        Music::pause();
        Music::fadeIn( 0.3 );
        Music::loadSong( Util::getFiles( Filesystem::find("/music/"), "*" ) );
        Music::play();
        */

        int roundColors[ 120 ];
        int showRound = sizeof( roundColors ) / sizeof(int) - 1;
        Util::blend_palette( roundColors, 60, Bitmap::makeColor( 96, 0, 0 ), Bitmap::makeColor( 200, 0, 0 ) );
        Util::blend_palette( roundColors + 60, 60, Bitmap::makeColor( 255, 0, 0 ), Bitmap::makeColor( 96, 0, 0 ) );

        while ( ! done ){

                bool draw = false;
                // key.poll();

                if ( Global::speed_counter > 0 ){
                        if ( ! paused ){
                                runCounter += Global::speed_counter * gameSpeed * Global::LOGIC_MULTIPLIER;

                                while ( runCounter >= 1.0 ){
                                        draw = true;
                                        world.act();
                                        runCounter -= 1.0;

                                        if ( player1->getHealth() <= 0 || player2->getHealth() <= 0 ){
                                                if ( player1->getHealth() <= 0 && player2->getHealth() > 0 ){
                                                        fadeOut( screen_buffer, "Player 2 wins!" );
                                                } else if ( player1->getHealth() > 0 && player2->getHealth() <= 0 ){
                                                        fadeOut( screen_buffer, "Player 1 wins!" );
                                                } else {
                                                        fadeOut( screen_buffer, "Draw!" );
                                                }
                                                return;
                                        }
                                }
                        }

                        /* FIXME */
                        /*
                        const double SPEED_INC = 0.02;
                        if ( key[Keyboard::Key_MINUS_PAD] ){
                                gameSpeed -= SPEED_INC;
                                if ( gameSpeed < SPEED_INC ){
                                        gameSpeed = SPEED_INC;
                                }
                                Global::debug( 3 ) << "Game speed " << gameSpeed << endl;
                        }

                        if ( key[ Keyboard::Key_ESC ] ){
                                throw Exception::Return(__FILE__, __LINE__);
                        }

                        if ( key[ Keyboard::Key_P ] ){
                                paused = ! paused;
                                draw = true;
                        }

                        if ( key[ Keyboard::Key_PLUS_PAD ] ){
                                gameSpeed += SPEED_INC;
                                Global::debug( 3 ) << "Game speed " << gameSpeed << endl;
                        }

                        if ( key[ Keyboard::Key_ENTER_PAD ] ){
                                gameSpeed = 1;
                                Global::debug( 3 ) << "Game speed " << gameSpeed << endl;
                        }
                        */

                        Global::speed_counter = 0;
                } else {
                        Util::rest( 1 );
                }
                
                /*
                while ( Global::second_counter > 0 ){
                        game_time--;
                        Global::second_counter--;
                        if ( game_time < 0 )
                                game_time = 0;
                }
                */
        
                if ( draw ){
                        const Font & font = Font::getFont(Global::DEFAULT_FONT);

                        background.Blit( work );
                        world.draw( &work );
                        
                        /*

                        // work.printf( 180, 1, Bitmap::makeColor(255,255,255), (FONT *)all_fonts[ JOHNHANDY_PCX ].dat, "%d", game_time );

                        int min_x = (int)(player1->getX() < player2->getX() ? player1->getX() - 50 : player2->getX() - 50);
                        int max_x = (int)(player1->getX() > player2->getX() ? player1->getX() + 50 : player2->getX() + 50);
                        int min_y = 0;
                        // int max_y = screen_buffer.getHeight();

                        while ( max_x - min_x < screen_buffer.getWidth() / 2 ){
                                max_x += 1;
                                min_x -= 1;
                        }

                        if ( min_x > screen_buffer.getWidth() / 2 ){
                                min_x = screen_buffer.getWidth() / 2;
                        }
                        if ( min_x < 0 ){
                                min_x = 0;
                        }
                        if ( max_x < screen_buffer.getWidth() / 2 ){
                                max_x = screen_buffer.getWidth() / 2;
                        }
                        if ( max_x > screen_buffer.getWidth() ){
                                max_x = screen_buffer.getWidth();
                        }
        
                        / * split is the number of pixels to show in the Y direction * /
                        int split = screen_buffer.getHeight() * (max_x - min_x) / screen_buffer.getWidth();
                        / * cut the difference into two pieces, min_y and max_y * /
                        min_y = (screen_buffer.getHeight() - split);
                        // max_y -= (screen_buffer.getHeight() - split) / 2;

                        // work.Stretch( screen_buffer, min_x, min_y, max_x - min_x, max_y - min_y, 0, 0, screen_buffer.getWidth(), screen_buffer.getHeight() );
                        
                        */

                        // work.Blit( screen_buffer );
                        const double divider = 5;
                        const double x_distance = screen_buffer.getWidth() / divider;
                        double min_x_1 = player1->getX() - x_distance;
                        double max_x_1 = player1->getX() + x_distance;
                        double min_x_2 = player2->getX() - x_distance;
                        double max_x_2 = player2->getX() + x_distance;

                        if ( min_x_1 < 0 ){
                                max_x_1 += 0 - min_x_1;
                                min_x_1 = 0;
                        }
                        if ( max_x_1 > screen_buffer.getWidth() ){
                                min_x_1 -= max_x_1 - screen_buffer.getWidth();
                                max_x_1 = screen_buffer.getWidth();
                        }
                        
                        if ( min_x_2 < 0 ){
                                max_x_2 += 0 - min_x_2;
                                min_x_2 = 0;
                        }
                        if ( max_x_2 > screen_buffer.getWidth() ){
                                min_x_2 -= max_x_2 - screen_buffer.getWidth();
                                max_x_2 = screen_buffer.getWidth();
                        }

                        if ( (min_x_1 < min_x_2 && max_x_1 > min_x_2) ||
                             (min_x_2 < min_x_1 && max_x_2 > min_x_1) ||
                             (closeFloat(min_x_1, min_x_2)) ){
                             /* the players are close enough together to show
                              * them in the same screen
                              */

                                double space = x_distance * 4 - fabs( player1->getX() - player2->getX() );
                                double p1 = player1->getX() < player2->getX() ? player1->getX() : player2->getX();
                                double p2 = player1->getX() >= player2->getX() ? player1->getX() : player2->getX();
                                double x1 = p1 - space / 2;
                                double x2 = p2 + space / 2;

                                if ( x2 > screen_buffer.getWidth() ){
                                        x1 -= x2 - screen_buffer.getWidth();
                                        x2 = screen_buffer.getWidth();
                                } else if ( x1 < 0 ){
                                        x2 += 0 - x1;
                                        x1 = 0;
                                }
                                

                                /*
                                int y1 = (int)(distance / 2 - screen_buffer.getHeight() / divider);
                                int y2 = plane + screen_buffer.getHeight();
                                */
                                double visible = screen_buffer.getHeight() * (divider - 1) / divider;
                                double y1 = world.getMinimumZ() - visible / 2;
                                double y2 = world.getMinimumZ() + visible / 2;
                                if ( y1 < 0 ){
                                        y2 += - y1;
                                        y1 = 0;
                                } else if ( y2 > screen_buffer.getHeight() ){
                                        y1 -= y2 - screen_buffer.getHeight();
                                        y2 = screen_buffer.getHeight();
                                }

                                work.Stretch( screen_buffer, (int)x1, (int)y1, (int)(x2 - x1), (int)(y2 - y1), 0, 0, screen_buffer.getWidth(), screen_buffer.getHeight() );

                        } else {
                                /* split screen */

                                /*
                                int y1 = (int)(screen_buffer.getHeight() / divider);
                                int y2 = screen_buffer.getHeight();
                                */

                                double visible = screen_buffer.getHeight() * (divider - 1) / divider;
                                double y1 = world.getMinimumZ() - visible / 2;
                                double y2 = world.getMinimumZ() + visible / 2;
                                if ( y1 < 0 ){
                                        y2 += - y1;
                                        y1 = 0;
                                } else if ( y2 > screen_buffer.getHeight() ){
                                        y1 -= y2 - screen_buffer.getHeight();
                                        y2 = screen_buffer.getHeight();
                                }
                                int p = player1->getX() < player2->getX() ? 0 : screen_buffer.getWidth() / 2;
                                work.Stretch( screen_buffer, (int)min_x_1, (int)y1, (int)(max_x_1 - min_x_1), (int)(y2 - y1), p, 0, screen_buffer.getWidth() / 2, screen_buffer.getHeight() );
                                work.Stretch( screen_buffer, (int)min_x_2, (int)y1, (int)(max_x_2 - min_x_2), (int)(y2 - y1), screen_buffer.getWidth() / 2 - p, 0, screen_buffer.getWidth() / 2, screen_buffer.getHeight() );
                        }
                             

                        if ( showRound > 0 ){
                                font.printf( screen_buffer.getWidth() / 2, screen_buffer.getHeight() / 2, roundColors[ showRound ], screen_buffer, "Round %d", 0, round );
                                showRound -= 1;
                        }

                        
                        font.printf( 10, 0, Bitmap::makeColor( 255, 255, 255 ), screen_buffer, "%s", 0, player1->getName().c_str() );
                        player1->drawLifeBar( 10, font.getHeight(), &screen_buffer );
                        font.printf( screen_buffer.getWidth() - 200, 0, Graphics::Bitmap::makeColor( 255, 255, 255 ), screen_buffer, "%s", 0, player2->getName().c_str() );
                        player2->drawLifeBar( screen_buffer.getWidth() - 200, font.getHeight(), &screen_buffer );

                        FontRender * render = FontRender::getInstance();
                        render->render( &screen_buffer );

                        if ( paused ){
                                screen_buffer.transBlender( 0, 0, 0, 128 );
                                screen_buffer.drawingMode( Graphics::Bitmap::MODE_TRANS );
                                screen_buffer.rectangleFill( 0, 0, screen_buffer.getWidth(), screen_buffer.getHeight(), Graphics::Bitmap::makeColor( 0, 0, 0 ) );
                                screen_buffer.drawingMode( Graphics::Bitmap::MODE_SOLID );
                                const Font & font = Font::getFont(Global::DEFAULT_FONT);
                                font.printf( screen_buffer.getWidth() / 2, screen_buffer.getHeight() / 2, Graphics::Bitmap::makeColor( 255, 255, 255 ), screen_buffer, "Paused", 0 );
                        }

                        /* getX/Y move when the world is quaking */
                        screen_buffer.BlitToScreen( world.getX(), world.getY() );

                        /*
                        if ( key[ Keyboard::Key_F12 ] ){
                                Global::debug( 2 ) << "Saved screenshot to scr.bmp" << endl;
                                work.save( "scr.bmp" );
                        }
                        */

                        work.clear();
                }

                // done |= key[ Keyboard::Key_ESC ] || world.finished();
                done = world.finished();
        }
}
#endif

}
