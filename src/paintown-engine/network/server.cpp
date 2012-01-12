#ifdef HAVE_NETWORKING

#include "util/bitmap.h"
#include "util/stretch-bitmap.h"
#include "util/trans-bitmap.h"
#include "util/parameter.h"
#include "util/events.h"

#include "chat_server.h"
#include "factory/font_render.h"
#include "../factory/heart_factory.h"
#include "../factory/object_factory.h"
#include "../game/game.h"
#include "globals.h"
#include "util/init.h"
#include "../level/utils.h"
#include "util/loading.h"
#include "util/music.h"
#include "network_world.h"
#include "../object/alliance.h"
#include "../object/character.h"
#include "../object/network_character.h"
#include "../object/network_player.h"
#include "../object/object.h"
#include "../object/player.h"
#include "../game/mod.h"
#include "util/exceptions/exception.h"
#include "util/menu/menu.h"
#include "server.h"
#include <sstream>
#include "util/font.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/system.h"
#include "util/input/keyboard.h"
#include "util/input/input-manager.h"
#include "util/input/text-input.h"

#include "../game/options.h"
#include "util/menu/font-info.h"

#include "configuration.h"

using namespace std;

namespace Network{

static std::ostream & debug(int level){
    return Global::debug(level, "[server]");
}

static void set_to_true(void * what){
    bool * b = (bool *) what;
    *b = true;
}

static void do_quit(void * what){
    throw Exception::Return(__FILE__, __LINE__);
}

static int getServerPort(){
    Graphics::Bitmap background(Global::titleScreen().path());
    const int drawY = 250;
    {
        // background.BlitToScreen();
        const Font & font = Font::getFont(Global::DEFAULT_FONT, 20, 20 );
        Graphics::Bitmap black( 300, font.getHeight() * 4 );
        black.clear();
        black.border( 0, 1, Graphics::makeColor( 255, 255, 255 ) );
        Graphics::Bitmap::transBlender( 0, 0, 0, 92 );
        black.translucent().draw( 20, drawY - font.getHeight() - 20, background );
        font.printf( 40, drawY, Graphics::makeColor( 255, 255, 255 ), background, "Port:", 0 );
        font.printf( 40, drawY - font.getHeight() - 5, Graphics::makeColor( 255, 255, 255 ), background, "Enter to start. ESC to quit", 0 );
        background.BlitToScreen();
    }

    /* TODO: add filter for numbers */
    TextInput input("7887");
    input.addBlockingHandle(Keyboard::Key_ESC, do_quit, NULL);

    class Logic: public Util::Logic {
    public:
        Logic(TextInput & input, bool & draw):
        input(input),
        is_done(false),
        draw(draw){
            input.addBlockingHandle(Keyboard::Key_ENTER, set_to_true, &is_done);
            input.enable();
        }

        TextInput & input;
        bool is_done;
        bool & draw;

        bool done(){
            return is_done;
        }

        double ticks(double system){
            return system;
        }

        void run(){
            draw = input.doInput();
        }
    };

    class Draw: public Util::Draw {
    public:
        Draw(const Graphics::Bitmap & background, const TextInput & input, int drawY, bool & need_draw):
        background(background),
        input(input),
        drawY(drawY),
        need_draw(need_draw){
        }

        const Graphics::Bitmap & background;
        const TextInput & input;
        int drawY;
        bool & need_draw;

        void draw(const Graphics::Bitmap & screen){
            if (need_draw){
                Graphics::StretchedBitmap buffer(640, 480, screen);
                buffer.start();
                background.Blit(buffer);
                Graphics::Bitmap work(buffer, 100, drawY, 200, 25);
                work.clear();
                const Font & font = Font::getFont(Global::DEFAULT_FONT, 20, 20 );
                font.printf(0, 0, Graphics::makeColor(255, 255, 255), work, input.getText(), 0);
                // work.Blit(100, drawY, background);
                // background.BlitToScreen();
                buffer.finish();
                screen.BlitToScreen();
            }
        }
    };
    
    bool draw = true;
    Logic logic(input, draw);
    Draw drawer(background, input, drawY, draw);
    drawer.draw(*Graphics::screenParameter.current());

    Util::standardLoop(logic, drawer);

#if 0
    bool draw = true;
    Global::speed_counter2 = 0;
    while (! done){
        while (Global::speed_counter2 > 0){
            InputManager::poll();
            draw = input.doInput();
            Global::speed_counter2 -= 1;
        }

        if (draw){
            work.clear();
            const Font & font = Font::getFont(Global::DEFAULT_FONT, 20, 20 );
            font.printf(0, 0, Graphics::makeColor( 255, 255, 255 ), work, input.getText(), 0);
            work.Blit(100, drawY, background);
            background.BlitToScreen();
        }
        
        draw = false;

        while ( Global::speed_counter2 == 0 ){
            Util::rest(1);
        }
    }
#endif

    istringstream str(input.getText());
    int port;
    str >> port;
    return port;
}

#if 0
static void networkGame( const vector< Object * > & players, const string & levelFile, const vector< NLsocket > & sockets ){

	vector< string > levels = Level::readLevels( levelFile );

	// global_debug = true;

	int showHelp = 300;
	for ( vector< string >::iterator it = levels.begin(); it != levels.end(); it++ ){
		Global::done_loading = false;
		pthread_t loading_screen_thread;

		startLoading( &loading_screen_thread );

		networkSendLevel( sockets, *it );

		bool gameState = false;
		try {
			// vector< Object * > players;
			// players.push_back( player );
			NetworkWorld world( sockets, players, *it );

			Music::pause();
			Music::fadeOut( 0.3 );
			Util::rest( 100 );
			Music::fadeIn( 0.3 );
			Music::loadSong( Util::getFiles( Util::getDataPath() + "/music/", "*" ) );
			Music::play();

			for ( vector< Object * >::const_iterator it = players.begin(); it != players.end(); it++ ){
				Player * playerX = (Player *) *it;
				playerX->setY( 200 );
				/* setMoving(false) sets all velocities to 0 */
				playerX->setMoving( false );
				/* but the player is falling so set it back to true */
				playerX->setMoving( true );

				playerX->setStatus( Status_Falling );
				world.addMessage( playerX->movedMessage() );
			}

			stopLoading( loading_screen_thread );

			// nlDisable( NL_BLOCKING_IO );
			gameState = playLevel( world, players, showHelp );
			// nlEnable( NL_BLOCKING_IO );
			showHelp = 0;

			world.addMessage( world.finishMessage() );
			world.flushOutgoing();

		} catch ( const LoadException & le ){
			Global::debug( 0 ) << "Could not load " << *it << " because " << le.getReason() << endl;
			/* if the level couldn't be loaded turn off
			 * the loading screen
			 */
			stopLoading( loading_screen_thread );
		}

		ObjectFactory::destroy();
		HeartFactory::destroy();

		if ( ! gameState ){
			return;
		}

		fadeOut( "Next level" );
	}

	for ( vector< NLsocket >::const_iterator it = sockets.begin(); it != sockets.end(); it++ ){
		NLsocket s = *it;
		Network::send16( s, -1 );
	}

	fadeOut( "You win!" );
}
#endif

static void sendToAll( const vector< Socket > & sockets, const Message & message ){
    for ( vector< Socket >::const_iterator it = sockets.begin(); it != sockets.end(); it++ ){
        const Socket & socket = *it;
        message.send( socket );
    }
}

static int allAlliance = Paintown::ALLIANCE_FREE_FOR_ALL;
static int playerAlliance(){
    if (Configuration::getPlayMode() == Configuration::FreeForAll){
        return allAlliance++;
    } else {
        return Paintown::ALLIANCE_PLAYER;
    }
}

static void waitAllOk(const vector<Socket> & sockets){
    Global::debug(1) << "Waiting for an ok from all clients" << endl;
    Global::info("Waiting for all clients..");
    for (vector<Socket>::const_iterator it = sockets.begin(); it != sockets.end(); it++){
        bool done = false;
        while (!done){
            Message ok(*it);
            int type;
            ok >> type;
            if (type == World::OK){
                Global::debug(1) << "Received ok from client " << *it << endl;
                done = true;
            } else {
                Global::debug(1) << "Received " << type << " from client " << *it << ". Ignoring.." << endl;
            }
        }
    }
}

static void waitAllFinish(const vector<Socket> & sockets){
    for (vector<Socket>::const_iterator it = sockets.begin(); it != sockets.end(); it++){
        bool done = false;
        while (!done){
            Message ok(*it);
            int type;
            ok >> type;
            if (type == World::FINISH){
                Global::debug(1) << "Received finish from client " << *it << endl;
                done = true;
            } else {
                Global::debug(1) << "Received " << type << " from client " << *it << ". Ignoring.." << endl;
            }
        }
    }
}

static void sendAllOk(const vector<Socket> & sockets){
    Message ok;
    ok << World::OK;
    Global::debug(1) << "Sending an ok to all clients" << endl;
    sendToAll(sockets, ok);
}

static Level::LevelInfo selectLevels(){
    /* then the user selects a set of levels to play */
    Menu::Context context;

    /* FIXME: get a better Menu::Context object, one that already has
     * a font set and a background.
     */
    context.setFont(Util::ReferenceCount<Menu::FontInfo>(new Menu::RelativeFontInfo(Global::DEFAULT_FONT, 20, 20)));
    Graphics::Bitmap normalBackground(Global::titleScreen().path());
    context.addBackground(normalBackground);
    return Paintown::doLevelMenu("/levels", context);
}

/* for each client get the player they want and send back an id */
static void createClients(const vector<Client*> & clients, vector<Network::Socket> & sockets, map<Paintown::Object::networkid_t, string> & clientNames, map<Paintown::Object*, Socket> & characterToSocket, vector<Paintown::Object *> & players){
    /* clients start at id 2 because the server is 1 */
    Paintown::Object::networkid_t id = 2;
    /* all other players will send their chosen character as a
     * CREATE_CHARACTER message. after receiving it, send back the
     * network id for that character.
     */

    /* bundle up all the client infos and send them after setting up the clients */
    vector<Message> clientInfos;

    for (vector<Client*>::const_iterator it = clients.begin(); it != clients.end(); it++){
        Client * client = *it;
        debug(1) << "Setting up client " << client->getName() << endl;
        Global::info("Setting up client " + client->getName());
        Socket socket = client->getSocket();
        sockets.push_back(socket);
        debug(1) << "Read character path from " << id << endl;
        Message message(socket);
        int type;
        message >> type;
        if (type == World::CREATE_CHARACTER){
            int alliance = playerAlliance();
            Global::info(" creating " + message.path);
            Paintown::Character * client_character = new Paintown::NetworkPlayer(Storage::instance().find(Filesystem::RelativePath(message.path)), alliance);
            characterToSocket[client_character] = socket;
            /* FIXME: Don't need this line now that NetworkPlayer exists.
             * take it out at some point.
             */
            ((Paintown::NetworkCharacter *)client_character)->alwaysShowName();

            players.push_back(client_character);
            client_character->setLives(1);
            client_character->setId(id);
            Message clientId;
            clientId << World::SET_ID;
            clientId << id;
            clientId << alliance;
            clientId.send(socket);

            clientNames[id] = client->getName();

            Message info;
            info.id = 0;
            info << World::CLIENT_INFO;
            info << id;
            info << client->getName();
            clientInfos.push_back(info);

            id += 1;
        } else {
            debug(0) << "Got a bogus message: " << type << endl;
        }
    }

    Global::info("Notify all clients");
    for (vector<Message>::iterator it = clientInfos.begin(); it != clientInfos.end(); it++){
        sendToAll(sockets, *it);
    }
}

/* TODO: simplify this code */
static void playGame(vector<Client*> & clients){
    vector<Paintown::Object *> players;
    try{
        /* first the user selects his own player */
        Level::LevelInfo info;
        int remap = 0;
        Filesystem::AbsolutePath playerPath = Paintown::Mod::getCurrentMod()->selectPlayer("Pick a player", info, remap, InputSource());
        Paintown::Player * player = new Paintown::Player(playerPath, Util::ReferenceCount<InputSource>(new InputSource(0, 0)));
        player->setMap(remap);
        player->ignoreLives();
        players.push_back(player);

        Level::LevelInfo levelInfo = selectLevels();

        /* reset the alliance settings */
        allAlliance = Paintown::ALLIANCE_FREE_FOR_ALL;

        /* the server player is network id 1 */
        player->setId(1);
        /* the server's player alliance can just be ALLIANCE_PLAYER, so no
         * need to change it
         */

        /* sockets for clients. why cant we get this from the Client class? */
        vector<Network::Socket> sockets;

        /* keep track of characters and their related sockets (clients) */
        map<Paintown::Object*, Socket> characterToSocket;

        map<Paintown::Object::networkid_t, string> clientNames;

        class ClientContext: public Loader::LoadingContext {
        public:
            ClientContext(const vector<Client*> & clients,
                          vector<Network::Socket> & sockets,
                          map<Paintown::Object::networkid_t, string> & clientNames,
                          map<Paintown::Object*, Socket> & characterToSocket,
                          vector<Paintown::Object *> & players):
            clients(clients),
            sockets(sockets),
            clientNames(clientNames),
            characterToSocket(characterToSocket),
            players(players){
            }

            const vector<Client*> & clients;
            vector<Network::Socket> & sockets;
            map<Paintown::Object::networkid_t, string> & clientNames;
            map<Paintown::Object*, Socket> & characterToSocket;
            vector<Paintown::Object *> & players;

            virtual void load(){
                createClients(clients, sockets, clientNames, characterToSocket, players);

                /* send all created characters to all clients */
                for (vector<Paintown::Object *>::iterator it = players.begin(); it != players.end(); it++){
                    Paintown::Character * c = (Paintown::Character *) *it;
                    Filesystem::RelativePath path = Storage::instance().cleanse(c->getPath());
                    // path.erase( 0, Util::getDataPath().length() );
                    Message add;
                    add << World::CREATE_CHARACTER;
                    add << c->getId();
                    add << c->getAlliance();
                    add.path = path.path();
                    sendToAll(sockets, add);
                }
            }
        };

        ClientContext context(clients, sockets, clientNames, characterToSocket, players);
        Loader::loadScreen(context, Level::convert(levelInfo));

        for (vector< string >::const_iterator it = levelInfo.getLevels().begin(); it != levelInfo.getLevels().end(); it++){
            string level = *it;
            debug(1) << "Sending level '" << level << "'" << endl;
            Message loadLevel;
            loadLevel << World::LOAD_LEVEL;
            loadLevel.path = level;
            sendToAll(sockets, loadLevel);

            class GameContext: public Loader::LoadingContext {
            public:
                GameContext(const string & level, 
                            vector<Network::Socket> & sockets,
                            map<Paintown::Object*, Socket> & characterToSocket,
                            map<Paintown::Object::networkid_t, string> & clientNames,
                            vector<Paintown::Object *> & players):
                world(NULL),
                level(level),
                sockets(sockets),
                characterToSocket(characterToSocket),
                clientNames(clientNames),
                players(players){
                }
                
                NetworkWorld * world;
                string level;
                vector<Network::Socket> & sockets;
                map<Paintown::Object*, Socket> & characterToSocket;
                map<Paintown::Object::networkid_t, string> & clientNames;
                vector<Paintown::Object *> & players;

                virtual ~GameContext(){
                    delete world;
                }

                virtual void load(){
                    debug(1) << "Create network world" << endl;
                    /* TODO: handle LoadException here */
                    world = new NetworkWorld(sockets, players, characterToSocket, Storage::instance().find(Filesystem::RelativePath(level)), clientNames);

                    debug(1) << "Load music" << endl;

                    Music::changeSong();

                    /* wait for an ok from all the clients, then send another
                     * ok to continue
                     */
                    waitAllOk(sockets);
                    sendAllOk(sockets);
                }
            };

            for (vector<Paintown::Object *>::const_iterator it = players.begin(); it != players.end(); it++){
                Paintown::Character * playerX = (Paintown::Character *) *it;
                playerX->setY(200);
                /* setMoving(false) sets all velocities to 0 */
                playerX->setMoving(false);
                /* but the player is falling so set it back to true */
                playerX->setMoving(true);

                playerX->setStatus(Paintown::Status_Falling);
            }

            GameContext gameContext(level, sockets, characterToSocket, clientNames, players);
            Loader::loadScreen(gameContext, Level::convert(levelInfo));

            NetworkWorld & world = *gameContext.world;
            world.startMessageHandlers();

            bool played = Game::playLevel(world, players);

            ObjectFactory::destroy();
            HeartFactory::destroy();

            world.stopRunning();
            Message finish;
            finish << World::FINISH;
            finish.id = 0;
            Global::debug(1) << "Sending finish message to all clients" <<  endl;
            sendToAll(sockets, finish);
            world.waitForHandlers();

            Global::debug(1) << "Waiting for finish from all clients " << endl;
            waitAllFinish(sockets);

            Message ignore;
            ignore << World::IGNORE_MESSAGE;
            ignore.id = 0;
            Global::debug(1) << "Sending ignore to all clients" << endl;
            sendToAll(sockets, ignore);

            /* another ok barrier */
            Global::debug(1) << "Wait for all clients to ok" << endl;
            waitAllOk(sockets);
            sendAllOk(sockets);

            /* if the server quits, then everyone quits */
            if (!played){
                Global::debug(1) << "Server exiting early.." << endl;
                break;
            }
        }
        Network::Message gameOver;
        gameOver.id = 0;
        gameOver << World::GAME_OVER;
        sendToAll(sockets, gameOver);
    } catch ( const LoadException & le ){
        debug(0) << "Load exception: " + le.getTrace() << endl;
    } catch ( const Exception::Return & re ){
    } catch ( const NetworkException & ne ){
        debug(0) << "Network excetion: " + ne.getMessage() << endl;
    }

    for ( vector< Paintown::Object * >::iterator it = players.begin(); it != players.end(); it++ ){
        delete *it;
    }

    for ( vector<Client*>::const_iterator it = clients.begin(); it != clients.end(); it++ ){
        Network::close((*it)->getSocket());
    }
}

static void popup( const Font & font, const string & message ){
	int length = font.textLength( message.c_str() ) + 20; 
        Graphics::Bitmap background(Global::titleScreen().path());
        Graphics::Bitmap area( background, background.getWidth() / 2 - length / 2, 220, length, font.getHeight() * 3 );
        Graphics::Bitmap::transBlender( 0, 0, 0, 128 );
	// area.drawingMode( Bitmap::MODE_TRANS );
	area.translucent().rectangleFill( 0, 0, area.getWidth(), area.getHeight(), Graphics::makeColor( 64, 0, 0 ) );
	// area.drawingMode( Bitmap::MODE_SOLID );
        Graphics::Color color = Graphics::makeColor( 255, 255, 255 );
	area.rectangle( 0, 0, area.getWidth() - 1, area.getHeight() - 1, color );
	font.printf( 10, area.getHeight() / 2, Graphics::makeColor( 255, 255, 255 ), area, message, 0 );
        background.BlitToScreen();
}

void networkServer(){

    // const int startingLives = 4;
    int port = getServerPort();

    // Keyboard key;

    debug(1) << "Port is " << port << endl;

    const Font & font = Font::getFont(Global::DEFAULT_FONT, 20, 20 );
    try{
        /*
#ifdef _WIN32
Network::blocking( false );
#endif
*/
        debug(1) << "Get socket" << endl;
        Network::Socket server = Network::open(port);

        /*
        // NLsocket server = nlOpen( port, NL_RELIABLE_PACKETS );
        if ( server == NL_INVALID ){
        Global::debug( 0 ) << "hawknl error: " << nlGetSystemErrorStr( nlGetSystemError() ) << endl;
        throw ReturnException();
        }
        */

        debug(1) << "Run chat server" << endl;

        ChatServer chat("server", server);
        chat.run();
        /*
#ifdef _WIN32
Network::blocking( true );
#endif
*/
        vector<Client*> clients = chat.getConnectedClients();
        if (! clients.empty()){
            debug(1) << "Start game with " << clients.size() << " clients" << endl;
            playGame(clients);
        } else {
            // key.poll();
            popup(font, "No clients connected" );
            InputManager::waitForKeys(Keyboard::Key_ENTER, Keyboard::Key_ESC, InputSource());
            // key.wait();
            // key.readKey();
        }
        Network::close( server );
    } catch ( const NetworkException & ne ){
        debug(0) << "Network error: " << ne.getMessage() << endl;
        // key.poll();
        popup(font, "Network error: " + ne.getMessage());
        InputManager::waitForKeys(Keyboard::Key_ENTER, Keyboard::Key_ESC, InputSource());
        /*
        key.wait();
        key.readKey();
        */
    }
    return;
}

}

#endif
