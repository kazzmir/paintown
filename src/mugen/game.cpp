#include "util/graphics/bitmap.h"
#include "game.h"

#include <stdio.h>
#include <ostream>
#include <sstream>
#include "util/font.h"
#include "util/token.h"
#include "util/configuration.h"
#include "globals.h"
#include "util/input/keyboard.h"
#include "util/input/input-manager.h"
#include "util/input/input-source.h"
#include "util/input/input-map.h"
#include "util/exceptions/load_exception.h"
#include "util/funcs.h"
#include "util/system.h"
#include "util/timedifference.h"
#include "util/events.h"
#include "util/gui/fadetool.h"
#include "util/exceptions/exception.h"
#include "util/network/network.h"
#include "search.h"
#include "util/loading.h"

#include "character.h"
#include "util.h"
#include "font.h"
#include "menu.h"
#include "stage.h"
#include "character-select.h"
#include "characterhud.h"
#include "storyboard.h"
#include "behavior.h"
#include "network-behavior.h"
#include "parse-cache.h"
#include "config.h"

#include "options.h"

#include "versus.h"

#include "world.h"
#include "util/lz4/lz4.h"

namespace PaintownUtil = ::Util;

using namespace Mugen;
using std::vector;
using std::string;
using std::ostringstream;

Game::Game(const PlayerType & playerType, const GameType & gameType, const Filesystem::AbsolutePath & systemFile):
playerType(playerType),
gameType(gameType),
systemFile(systemFile),
motifDirectory(systemFile.getDirectory()){
}

Game::~Game(){
}

void Game::run(Searcher & searcher){
    // ParseCache cache;
    /* Make sure searcher is running */
    searcher.start();

    try{
        switch (gameType){
            default:
            case Arcade: {
                doArcade(searcher);
                break;
            }
            case Versus: {
                doVersus(searcher);
                break;
            }
            case NetworkVersusServer: {
                doNetworkVersus(true, searcher);
                break;
            }
            case NetworkVersusClient: {
                doNetworkVersus(false, searcher);
                break;
            }
            case TeamArcade: {
                //select.run("Team Arcade" , 1, true, &screen);
                startDemo(searcher);
                break;
            }
            case TeamVersus: {
                //gameInfo = select.run("Team Versus" , 2, true, &screen);
                break;
            }
            case TeamCoop: {
                //gameInfo = select.run("Team Cooperative" , 1, true, &screen);
                break;
            }
            case Survival: {
                doSurvival(searcher);
                break;
            }
            case SurvivalCoop: {
                //gameInfo = select.run("Survival Cooperative" , 1, true, &screen);
                break;
            }
            case Training: {
                doTraining(searcher);
                break;
            }
            case Watch: {
                doWatch(searcher);
                break;
            }
            case Demo: {
                startDemo(searcher);
                break;
            }
        }
    } catch (const CanceledException & cancel){
        /* its ok */
    } catch (const MugenException e){
        std::ostringstream out;
        out << "Press ENTER to continue\n";
        out << "\n";
        out << "We are very sorry but an error has occured while trying to load MUGEN.";
        Graphics::Bitmap screen(*Graphics::screenParameter.current());
        PaintownUtil::showError(screen, e, out.str());
        InputManager::waitForKeys(Keyboard::Key_ENTER, Keyboard::Key_ESC, InputSource());
    }

    /* Continue the searcher */
    searcher.start();
}

RunMatchOptions::RunMatchOptions():
    ticker(0),
    endTime(-1),
    demoMode(false),
    player1Behavior(NULL),
    player2Behavior(NULL){
        fader.setState(Gui::FadeTool::EndFade);
    }
    
    // For demo mode
RunMatchOptions::RunMatchOptions(int endTime):
    ticker(0),
    endTime(endTime),
    demoMode(endTime != -1),
    player1Behavior(NULL),
    player2Behavior(NULL){
        fader.setFadeInTime(1);
        fader.setFadeOutTime(60);
    }
    
RunMatchOptions::RunMatchOptions(const RunMatchOptions & copy):
    ticker(copy.ticker),
    endTime(copy.endTime),
    demoMode(copy.demoMode),
    player1Behavior(copy.player1Behavior),
    player2Behavior(copy.player2Behavior){
        fader.setFadeInTime(1);
        fader.setFadeOutTime(60);
    }
    
    const RunMatchOptions & RunMatchOptions::operator=(const RunMatchOptions & copy){
        ticker = copy.ticker;
        endTime = copy.endTime;
        demoMode = copy.demoMode;
        player1Behavior = copy.player1Behavior;
        player2Behavior = copy.player2Behavior;
        fader.setFadeInTime(1);
        fader.setFadeOutTime(60);
        return *this;
    }
    
    void RunMatchOptions::act(){
        if (demoMode){
            if (InputManager::anyInput() && fader.getState() != Gui::FadeTool::FadeOut){
                ticker = endTime;
                fader.setState(Gui::FadeTool::FadeOut);
            }
            if (ticker < endTime){
                ticker++;
            }
            if (ticker == endTime && fader.getState() != Gui::FadeTool::FadeOut){
                fader.setState(Gui::FadeTool::FadeOut);
            }
            fader.act();
            if (fader.getState() == Gui::FadeTool::EndFade){
                throw QuitGameException();
            }
        }
    }
    
    void RunMatchOptions::draw(const Graphics::Bitmap & work){
        if (demoMode){
            fader.draw(work);
        }
    }
    
    bool RunMatchOptions::isDemoMode() const{
        return demoMode;
    }
    
    void RunMatchOptions::setBehavior(HumanBehavior * player1, HumanBehavior * player2){
        player1Behavior = player1;
        player2Behavior = player2;
    }
    
    HumanBehavior * RunMatchOptions::getPlayer1Behavior() const {
        return player1Behavior;
    }
    
    HumanBehavior * RunMatchOptions::getPlayer2Behavior() const {
        return player2Behavior;
    }

/*
static Filesystem::AbsolutePath maybeFindRandom(const std::string & name, std::vector<Filesystem::AbsolutePath> & all){
    if (name == "_"){
        if (all.size() > 0){
            Filesystem::AbsolutePath front = all[0];
            all.erase(all.begin());
            return front;
        }
        throw MugenException("No def files found", __FILE__, __LINE__);
    } else {
        return Filesystem::findInsensitive(Filesystem::RelativePath("mugen/chars/" + name + "/" + name + ".def"));
    }
}
*/

static Character * doLoad(const Filesystem::AbsolutePath & path){
    TimeDifference timer;

    Filesystem::AbsolutePath use = path;
    if (Storage::isContainer(path)){
        Storage::instance().addOverlay(path, path.getDirectory());
        use = Filesystem::AbsolutePath(Path::removeExtension(path.path()) + ".def");
    }

    try{
        Character * guy = new Character(use, Stage::Player1Side);
        Global::stream_type & out = Global::debug(0);
        out << "Loading player " << use.path();
        out.flush();
        timer.startTime();
        guy->load();
        timer.endTime();
        out << timer.printTime(" took") << std::endl;
        return guy;
    } catch (const MugenException & fail){
        try{
            if (Storage::isContainer(path)){
                Storage::instance().removeOverlay(path, path.getDirectory());
            }
        } catch (const Filesystem::Exception & ignore){
        }
        throw;
    }

    return NULL;
}

static bool maybeCharacter(const Filesystem::AbsolutePath & path){
    try{
        return Mugen::Util::probeDef(path, "Files", "cmd") != "";
    } catch (...){
    }
    return false;
}

static Character * makeCharacter(const std::string & name, bool random, std::vector<Filesystem::AbsolutePath> & all){
    if (random){
        while (all.size() > 0){
            int choice = PaintownUtil::rnd(all.size());
            try{
                return doLoad(all[choice]);
            } catch (const MugenException & failed){
                Global::debug(0) << "Failed to load " << all[choice].path() << " because " << failed.getReason() << std::endl;
                all.erase(all.begin() + choice);
            } catch (const Filesystem::NotFound & failed){
                Global::debug(0) << "Failed to load " << all[choice].path() << " because " << failed.getTrace() << std::endl;
                all.erase(all.begin() + choice);
            }
        }
        throw MugenException("No characters left to choose from!", __FILE__, __LINE__);
    } else {
        /* First check if the path is a def file */
        try{
            Filesystem::AbsolutePath path(Storage::instance().find(Filesystem::RelativePath("mugen/chars/" + name)));
            if (maybeCharacter(path)){
                return doLoad(path);
            }
        } catch (const Filesystem::NotFound & fail){
        }

        /* Then check if its a container (.7z, .zip) and mount it */
        try{
            Filesystem::AbsolutePath zip = Storage::instance().findContainer(Filesystem::RelativePath("mugen/chars/" + name));
            Storage::instance().addOverlay(zip, zip.getDirectory());
        } catch (const Filesystem::NotFound & fail){
        }

        std::vector<Filesystem::AbsolutePath> allDef = Storage::instance().getFilesRecursive(Storage::instance().find(Filesystem::RelativePath("mugen/chars/").join(Filesystem::RelativePath(name))), "*.def");

        /* Search for a file that looks like it probably contains a character as
         * opposed to a cutscene or whatever.
         */
        for (std::vector<Filesystem::AbsolutePath>::iterator it = allDef.begin(); it != allDef.end(); it++){
            Global::debug(1) << "Maybe load from def file " << it->path() << std::endl;
            if (maybeCharacter(*it)){
                return doLoad(*it);
            }
        }

        throw MugenException("No suitable .def files for " + name, __FILE__, __LINE__);
    }
}

/*! Wrap characters for teams by type for use by futures and game modes */
class CharacterTeam{
public:
    CharacterTeam(const Mugen::ArcadeData::CharacterCollection & collection, const Stage::teams & side):
    collection(collection),
    side(side){
        loaded[0] = loaded[1] = loaded[2] = loaded[3] = false;
        switch (collection.getType()){
            case Mugen::ArcadeData::CharacterCollection::Turns4:
                fourth = PaintownUtil::ReferenceCount<Character>(new Character(collection.getFourth().getDef(), side));
            case Mugen::ArcadeData::CharacterCollection::Turns3:
                third = PaintownUtil::ReferenceCount<Character>(new Character(collection.getThird().getDef(), side));
            case Mugen::ArcadeData::CharacterCollection::Turns2:
            case Mugen::ArcadeData::CharacterCollection::Simultaneous:
                second = PaintownUtil::ReferenceCount<Character>(new Character(collection.getSecond().getDef(), side));
            default:
            case Mugen::ArcadeData::CharacterCollection::Single:
                first = PaintownUtil::ReferenceCount<Character>(new Character(collection.getFirst().getDef(), side));
                break;
        }
    }
    
    void load(){
        switch (collection.getType()){
            case Mugen::ArcadeData::CharacterCollection::Turns4:
                if (!loaded[3]){
                    fourth->load(collection.getFourth().getAct());
                    loaded[3] = true;
                }
            case Mugen::ArcadeData::CharacterCollection::Turns3:
                if (!loaded[2]){
                    third->load(collection.getThird().getAct());
                    loaded[2] = true;
                }
            case Mugen::ArcadeData::CharacterCollection::Turns2:
            case Mugen::ArcadeData::CharacterCollection::Simultaneous:
                if (!loaded[1]){
                    second->load(collection.getSecond().getAct());
                    loaded[1] = true;
                }
            default:
            case Mugen::ArcadeData::CharacterCollection::Single:
                if (!loaded[0]){
                    first->load(collection.getFirst().getAct());
                    loaded[0] = true;
                }
                break;
        }
    }
    
    Character & getFirst(){
        return *first;
    }
    
    Character & getSecond(){
        return *second;
    }
    
    Character & getThird(){
        return *third;
    }
    
    Character & getFourth(){
        return *fourth;
    }
    
protected:
    const Mugen::ArcadeData::CharacterCollection & collection;
    const Stage::teams & side;
    bool loaded[4];
    PaintownUtil::ReferenceCount<Character> first;
    PaintownUtil::ReferenceCount<Character> second;
    PaintownUtil::ReferenceCount<Character> third;
    PaintownUtil::ReferenceCount<Character> fourth;
};

class PlayerLoader: public PaintownUtil::Future<int> {
public:
    PlayerLoader(CharacterTeam & player1, CharacterTeam & player2):
        alive(true),
        player1(player1),
        player2(player2){
            /* compute is a virtual function, is the virtual table set up
                * by the time start() tries to call it? or is that a race condition?
                */
            // start();
    }

    /* Communicate that the thread is dead */
    PaintownUtil::Thread::LockObject lock;
    volatile bool alive;
    CharacterTeam & player1;
    CharacterTeam & player2;

    virtual bool checkDead(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        return !alive;
    }

    virtual void compute(){
        ParseCache cache;

        if (checkDead()){
            return;
        }
        
        // Load player 1
        player1.load();

        if (checkDead()){
            return;
        }

        // Load player 2
        player2.load();
        // NOTE is this needed anymore?
#ifdef WII
        /* FIXME: this is a hack, im not sure why its even required but fopen() will hang on sfp_lock_acquire
            * in another thread without locking and unlocking the sfp lock.
            * related things
            *  newlib/libc/stdio/fopen.c -- calls sfp_lock_acquire
            *  newlib/libc/sys/linux/sys/libc-lock.h - uses pthread's as the lock implementation
            */
        /*
        __sfp_lock_acquire();
        __sfp_lock_release();
        */
#endif
    }

    virtual ~PlayerLoader(){
        lock.acquire();
        alive = false;
        lock.release();

        /* we have to wait till the done flag is set because otherwise
         * this object will be partially destroyed while the Future destructor
         * is running.
         */
        while (!isDone()){
            PaintownUtil::rest(1);
        }
    }
};

static PaintownUtil::ReferenceCount<PlayerLoader> preLoadCharacters(CharacterTeam & player1, CharacterTeam & player2){
    
    PaintownUtil::ReferenceCount<PlayerLoader> playerLoader = PaintownUtil::ReferenceCount<PlayerLoader>(new PlayerLoader(player1, player2));
    playerLoader->start();
    
    return playerLoader;
}

void prepareStage(PaintownUtil::ReferenceCount<PlayerLoader> playerLoader, Mugen::Stage & stage){
    try{
        Loader::Info info("Loading...", Filesystem::AbsolutePath());
        info.setPosition(25, 25);

        class Context: public Loader::LoadingContext {
        public:
            Context(PaintownUtil::ReferenceCount<PlayerLoader> playerLoader, Mugen::Stage & stage):
                playerLoader(playerLoader),
                stage(stage),
                fail(NULL){
                }

            virtual void maybeFail(){
                if (fail != NULL){
                    fail->throwSelf();
                }
            }

            virtual ~Context(){
                delete fail;
            }

            virtual void load(){
                try{
                    /* future */
                    int ok = playerLoader->get();

                    
                    /*! FIXME 
                    * this is only handling one on one matches, it will need to accomodate teams at a later point 
                    */
                    // Load stage
                    stage.addPlayer1(&playerLoader->player1.getFirst());
                    stage.addPlayer2(&playerLoader->player2.getFirst());
                    stage.load();
                } catch (const MugenException & fail){
                    this->fail = new MugenException(fail);
                } catch (const LoadException & fail){
                    this->fail = new LoadException(fail);
                } catch (const Filesystem::NotFound & fail){
                    this->fail = new Filesystem::NotFound(fail);
                }
            }

            PaintownUtil::ReferenceCount<PlayerLoader> playerLoader;
            Mugen::Stage & stage;
            Exception::Base * fail;
        };

        Context context(playerLoader, stage);
        /* FIXME: the wii has problems loading stuff in a background thread
         * while the load screen is going on.
         */
#ifdef WII
        context.load();
#else
        Loader::loadScreen(context, info);
#endif
        context.maybeFail();
    } catch (const MugenException & e){
        throw e;
    }
}

/* Does the setup work to start a game (arcade, watch, training, etc)
 */
class StartGameMode{
public:
    StartGameMode(const std::string & player1Name, const std::string & player2Name, const std::string & stageName){
        vector<string> players;
        players.push_back(player1Name);
        players.push_back(player2Name);
        initialize(players, stageName);
    }

    StartGameMode(const std::string & player1Name,
                  const std::string & player2Name,
                  const std::string & player3Name,
                  const std::string & player4Name,
                  const std::string & stageName){
        vector<string> players;
        players.push_back(player1Name);
        players.push_back(player2Name);
        players.push_back(player3Name);
        players.push_back(player4Name);
        initialize(players, stageName);
    }


    PaintownUtil::ReferenceCount<Character> getPlayer1(){
        return players[0];
    }
    
    PaintownUtil::ReferenceCount<Character> getPlayer2(){
        return players[1];
    }

    virtual void initialize(const vector<string> & playerNames, const string & stageName){
        ParseCache cache;
        /* Find regular files */
        std::vector<Filesystem::AbsolutePath> allCharacters = Storage::instance().getFilesRecursive(Storage::instance().find(Filesystem::RelativePath("mugen/chars/")), "*.def");

        /* Find container files */
        std::vector<Filesystem::AbsolutePath> zipCharacters = Storage::instance().getContainerFilesRecursive(Storage::instance().find(Filesystem::RelativePath("mugen/chars/")));
        allCharacters.insert(allCharacters.end(), zipCharacters.begin(), zipCharacters.end());
        std::random_shuffle(allCharacters.begin(), allCharacters.end());

        for (vector<string>::const_iterator it = playerNames.begin(); it != playerNames.end(); it++){
            string name = *it;
            bool random = name == "_";
            PaintownUtil::ReferenceCount<Character> player(makeCharacter(name, random, allCharacters));
            players.push_back(player);
        }

        stage = new Stage(Storage::instance().find(Filesystem::RelativePath("mugen/stages/" + stageName + ".def")));
        {
            TimeDifference timer;
            std::ostream & out = Global::debug(0);
            out << "Loading stage " << stageName;
            out.flush();
            timer.startTime();
            stage->load();
            timer.endTime();
            out << timer.printTime(" took") << std::endl;
        }

        if (players.size() > 0){
            stage->addPlayer1(players[0].raw());
        }

        if (players.size() > 1){
            stage->addPlayer2(players[1].raw());
        }

        /* TODO: handle players 3 and 4 */
    }

    virtual void run() = 0;

    virtual ~StartGameMode(){
    }

    vector<PaintownUtil::ReferenceCount<Character> > players;
    PaintownUtil::ReferenceCount<Stage> stage;
};

class StartArcade: public StartGameMode {
public:
    StartArcade(const std::string & player1Name,
                const std::string & player2Name,
                const std::string & stageName):
    StartGameMode(player1Name, player2Name, stageName){
    }

    virtual void run(){
        HumanBehavior player1Behavior(getPlayer1Keys(), getPlayer1InputLeft());
        LearningAIBehavior player2Behavior(30);
        getPlayer1()->setBehavior(&player1Behavior);
        getPlayer2()->setBehavior(&player2Behavior);

        RunMatchOptions options;
        options.setBehavior(&player1Behavior, NULL);

        /* Reset has to be called after addPlayer (done in initialize) */
        stage->reset();

        Game::runMatch(stage.raw(), "", options);
    }
};

void Game::startArcade(const std::string & player1Name, const std::string & player2Name, const std::string & stageName){
    StartArcade arcade(player1Name, player2Name, stageName);
    arcade.run();
}

#ifdef HAVE_NETWORKING
class NetworkObserver: public StageObserver {
public:
    NetworkObserver():
    StageObserver(){
    }

    virtual void start() = 0;
    virtual void kill() = 0;
};

static Token * filterTokens(Token * start){
    if (start->isData()){
        return start->copy();
    }

    Token * out = new Token(start->getName());
    for (vector<Token*>::const_iterator it = start->getTokens()->begin(); it != start->getTokens()->end(); it++){
        Token * use = filterTokens(*it);
        if (use->isData() || use->numTokens() > 0){
            *out << use;
        }
    }
    return out;
}

class NetworkBuffer{
public:
    NetworkBuffer(){
        length = 0;
        actualLength = 128;
        buffer = new char[actualLength];
    }

    ~NetworkBuffer(){
        delete[] buffer;
    }

    NetworkBuffer & operator<<(int16_t data){
        checkBuffer(sizeof(data));
        Network::dump16(buffer + length, data);
        length += sizeof(data);
        return *this;
    }

    virtual void add(char * data, int size){
        checkBuffer(size);
        memcpy(buffer + length, data, size);
        length += size;
    }

    char * getBuffer(){
        return buffer;
    }

    /* make sure N bytes can be stored */
    void checkBuffer(int bytes){
        if (length + bytes >= actualLength){
            increaseBuffer(bytes);
        }
    }

    int getLength() const {
        return length;
    }

    void increaseBuffer(int minimum){
        int newLength = PaintownUtil::max(actualLength * 2, actualLength + minimum);
        char * next = new char[newLength];
        memcpy(next, buffer, length);
        delete[] buffer;
        buffer = next;
        actualLength = newLength;
    }

protected:
    int length;
    char * buffer;
    int actualLength;
};

static const int16_t NetworkMagic = 0xd97f; 

class Packet{
public:
    enum Type{
        Input
    };

    Packet(Type type):
    type(type){
    }

    Type type;
};

class InputPacket: public Packet {
public:
    InputPacket(const vector<string> & inputs):
    Packet(Input),
    inputs(inputs){
    }

    vector<string> inputs;
};

PaintownUtil::ReferenceCount<Packet> readPacket(const Network::Socket & socket){
    int16_t type = Network::read16(socket);
    switch (type){
        case Packet::Input: {
            int16_t inputCount = Network::read16(socket);
            vector<string> inputs;
            for (int i = 0; i < inputCount; i++){
                int16_t size = Network::read16(socket);
                string input = Network::readStr(socket, size);
                inputs.push_back(input);
            }
            return PaintownUtil::ReferenceCount<Packet>(new InputPacket(inputs));
            break;
        }
        default: {
            throw MugenException("Unknown packet type", __FILE__, __LINE__);
        }
    }
    return PaintownUtil::ReferenceCount<Packet>(NULL);
}

class NetworkServerObserver: public NetworkObserver {
public:
    NetworkServerObserver(Network::Socket reliable, Network::Socket unreliable):
    NetworkObserver(),
    reliable(reliable),
    unreliable(unreliable),
    thread(this, send),
    clientThread(this, clientInput),
    alive_(true),
    count(0){
    }

    PaintownUtil::Thread::LockObject lock;
    Network::Socket reliable;
    Network::Socket unreliable;
    PaintownUtil::Thread::ThreadObject thread;
    PaintownUtil::Thread::ThreadObject clientThread;
    vector<PaintownUtil::ReferenceCount<World> > states;
    bool alive_;
    uint32_t count;

    void sendState(char * data, int compressed, int uncompressed){
        Global::debug(0, "server") << "Send " << compressed << " bytes" << std::endl;
        NetworkBuffer buffer;
        buffer << (int16_t) NetworkMagic;
        buffer << (int16_t) compressed;
        buffer << (int16_t) uncompressed;
        buffer.add(data, compressed);

        Network::sendBytes(reliable, (uint8_t*) buffer.getBuffer(), buffer.getLength());
    }

    bool hasStates(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        return states.size() > 0;
    }

    PaintownUtil::ReferenceCount<World> getState(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        PaintownUtil::ReferenceCount<World> out = states.front();
        states.erase(states.begin());
        return out;
    }

    bool alive(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        return alive_;
    }

    void kill(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        alive_ = false;
    }

    void doSend(){
        while (alive()){
            if (hasStates()){
                PaintownUtil::ReferenceCount<World> snapshot = getState();

                Token * test = snapshot->serialize();
                Token * filtered = filterTokens(test);
                // Global::debug(0) << "Snapshot: " << filtered->toString() << std::endl;
                string compact = filtered->toStringCompact();
                // Global::debug(0) << "Size: " << compact.size() << std::endl;
                char * out = new char[LZ4_compressBound(compact.size())];
                int compressed = LZ4_compress(compact.c_str(), out, compact.size());

                sendState(out, compressed, compact.size());

                // Global::debug(0) << "Compressed size: " << compressed << std::endl;
                delete[] out;

                delete test;
                delete filtered;

            } else {
                PaintownUtil::rest(1);
            }
        }
    }

    void addState(const PaintownUtil::ReferenceCount<World> & state){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        states.clear();
        states.push_back(state);
    }

    static void * send(void * self_){
        NetworkServerObserver * self = (NetworkServerObserver*) self_;
        self->doSend();
        return NULL;
    }

    void doClientInput(){
        while (alive()){
            int16_t magic = Network::read16(unreliable);
            if (magic != NetworkMagic){
                Global::debug(0) << "Garbage packet: " << magic << std::endl;
                continue;
            }

            PaintownUtil::ReferenceCount<Packet> packet = readPacket(unreliable);
        }
    }

    static void * clientInput(void * self_){
        NetworkServerObserver * self = (NetworkServerObserver*) self_;
        self->doClientInput();
        return NULL;
    }
    
    virtual void start(){
        thread.start();
    }
    
    virtual void beforeLogic(Stage & stage){
        count += 1;
        if (count % 30 == 0){
            addState(stage.snapshotState());
        }
    }

    virtual void afterLogic(Stage & stage){
    }
};

class NetworkClientObserver: public NetworkObserver {
public:
    NetworkClientObserver(Network::Socket socket, Network::Socket unreliable):
    NetworkObserver(),
    socket(socket),
    unreliable(unreliable),
    thread(this, receive),
    alive_(true){
    }

    Network::Socket socket;
    Network::Socket unreliable;
    PaintownUtil::Thread::ThreadObject thread;
    PaintownUtil::ReferenceCount<World> world;
    PaintownUtil::Thread::LockObject lock;
    bool alive_;

    void setWorld(const PaintownUtil::ReferenceCount<World> & world){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        this->world = world;
    }

    PaintownUtil::ReferenceCount<World> getWorld(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        PaintownUtil::ReferenceCount<World> out = world;
        world = NULL;
        return out;
    }

    bool alive(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        return alive_;
    }

    virtual void kill(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        alive_ = false;
    }

    void doReceive(){
        while (alive()){

            int16_t magic = Network::read16(socket);
            if (magic != NetworkMagic){
                Global::debug(0) << "Garbage packet: " << magic << std::endl;
                continue;
            }

            int16_t compressed = Network::read16(socket);
            int16_t uncompressed = Network::read16(socket);
            uint8_t * data = new uint8_t[compressed];
            Network::readBytes(socket, data, compressed);
            uint8_t * what = new uint8_t[uncompressed + 1];
            what[uncompressed] = '\0';
            LZ4_uncompress((const char *) data, (char *) what, uncompressed);
            TokenReader reader;
            std::string use((const char *) what);
            Token * head = reader.readTokenFromString(use);
            // Global::debug(0) << "Client received token " << head->toString() << std::endl;
            if (head != NULL){
                PaintownUtil::ReferenceCount<World> world(World::deserialize(head));
                setWorld(world);
            }
        }
    }

    static void * receive(void * self){
        ((NetworkClientObserver*)self)->doReceive();
        return NULL;
    }
    
    virtual void start(){
        thread.start();
    }
    
    virtual void beforeLogic(Stage & stage){
        PaintownUtil::ReferenceCount<World> next = getWorld();
        if (next != NULL){
            stage.updateState(*next);
        }
    }

    virtual void afterLogic(Stage & stage){
    }
};

#endif

/* FIXME: redo this as a StartGameMode class */
void Game::startNetworkVersus(const string & player1Name, const string & player2Name, const string & stageName, bool server, int port){
#ifdef HAVE_NETWORKING
    /* This has its own parse cache because its started by the main menu and not
     * by Game::run()
     */
    // ParseCache cache;
    std::vector<Filesystem::AbsolutePath> allCharacters = Storage::instance().getFilesRecursive(Storage::instance().find(Filesystem::RelativePath("mugen/chars/")), "*.def");
    std::random_shuffle(allCharacters.begin(), allCharacters.end());
    bool random1 = player1Name == "_";
    bool random2 = player2Name == "_";
    PaintownUtil::ReferenceCount<Character> player1;
    PaintownUtil::ReferenceCount<Character> player2;

    player1 = makeCharacter(player1Name, random1, allCharacters);
    player2 = makeCharacter(player2Name, random2, allCharacters);

    Mugen::Stage stage(Storage::instance().find(Filesystem::RelativePath("mugen/stages/" + stageName + ".def")));
    {
        TimeDifference timer;
        std::ostream & out = Global::debug(0);
        out << "Loading stage " << stageName;
        out.flush();
        timer.startTime();
        stage.load();
        timer.endTime();
        out << timer.printTime(" took") << std::endl;
    }

    Network::reuseSockets(true);

    Network::Socket socket = 0;
    if (server){
        Network::Socket remote = Network::openReliable(port);
        Network::listen(remote);
        Global::debug(0) << "Waiting for a connection on port " << port << std::endl;
        socket = Network::accept(remote);
        Network::close(remote);
        Global::debug(0) << "Got a connection" << std::endl;
    } else {
        Global::debug(0) << "Connecting to 127.0.0.1 on port " << port << std::endl;
        socket = Network::connectReliable("127.0.0.1", port); 
        Global::debug(0) << "Connected" << std::endl;
    }

    HumanBehavior player1Behavior(getPlayer1Keys(), getPlayer1InputLeft());
    DummyBehavior player2Behavior;
    // NetworkLocalBehavior player1Behavior(&local1Behavior, socket);
    // NetworkRemoteBehavior player2Behavior(socket);
    
    // Set regenerative health
    player1->setRegeneration(false);
    player2->setRegeneration(false);
    PaintownUtil::ReferenceCount<NetworkObserver> observer;
    if (server){
        player1->setBehavior(&player1Behavior);
        player2->setBehavior(&player2Behavior);
        Network::Socket server = Network::openUnreliable(port);
        Network::listen(server);
        Network::Socket udp = Network::accept(server);
        Network::close(server);
        observer = PaintownUtil::ReferenceCount<NetworkObserver>(new NetworkServerObserver(socket, udp));
        stage.setObserver(observer.convert<StageObserver>());
    } else {
        player2->setBehavior(&player1Behavior);
        player1->setBehavior(&player2Behavior);
        Global::debug(0) << "Connecting to udp" << std::endl;
        Network::Socket udp = Network::connectUnreliable("127.0.0.1", port);
        observer = PaintownUtil::ReferenceCount<NetworkObserver>(new NetworkClientObserver(socket, udp));
        stage.setObserver(observer.convert<StageObserver>());
    }
    
    RunMatchOptions options;
    
    options.setBehavior(&player1Behavior, NULL);

    /* server is player1 */
    if (server){
        stage.addPlayer1(player1.raw());
        stage.addPlayer2(player2.raw());
    } else {
        stage.addPlayer1(player1.raw());
        stage.addPlayer2(player2.raw());
    }

    stage.reset();
    int time = Mugen::Data::getInstance().getTime();
    Mugen::Data::getInstance().setTime(-1);

    if (server){
        int sync = Network::read16(socket);
        Network::send16(socket, sync);
    } else {
        Network::send16(socket, 0);
        Network::read16(socket);
    }

    observer->start();

    /*
    if (!Network::blocking(socket, false)){
        Global::debug(0) << "Could not set socket to be non-blocking" << std::endl;
    }
    */

    /*
    player1Behavior.begin();
    player2Behavior.begin();
    */

    /*
    if (!Network::noDelay(socket, true)){
        Global::debug(0) << "Could not set no delay!" << std::endl;
    }
    */

    try{
        runMatch(&stage, "", options);
    } catch (const QuitGameException & ex){
    }
    Mugen::Data::getInstance().setTime(time);

    observer->kill();

    Network::close(socket);

#endif

    throw QuitGameException();
}

class StartTraining: public StartGameMode {
public:
    StartTraining(const std::string & player1Name,
                const std::string & player2Name,
                const std::string & stageName):
    StartGameMode(player1Name, player2Name, stageName){
    }

    virtual void run(){
        HumanBehavior player1Behavior(getPlayer1Keys(), getPlayer1InputLeft());
        DummyBehavior dummyBehavior;
        // Set regenerative health
        getPlayer1()->setRegeneration(true);
        getPlayer2()->setRegeneration(true);
        getPlayer1()->setBehavior(&player1Behavior);
        getPlayer2()->setBehavior(&dummyBehavior);

        RunMatchOptions options;

        options.setBehavior(&player1Behavior, NULL);

        stage->reset();
        int time = Mugen::Data::getInstance().getTime();
        Mugen::Data::getInstance().setTime(-1);
        try{
            Game::runMatch(stage.raw(), "", options);
        } catch (const QuitGameException & ex){
        }
        Mugen::Data::getInstance().setTime(time);

        // Is this exception needed?
        // throw QuitGameException();
    }
};

void Game::startTraining(const std::string & player1Name, const std::string & player2Name, const std::string & stageName){
    StartTraining training(player1Name, player2Name, stageName);
    training.run();
}

class StartWatch: public StartGameMode {
public:
    StartWatch(const std::string & player1Name,
               const std::string & player2Name,
               const std::string & stageName):
    StartGameMode(player1Name, player2Name, stageName){
    }

    virtual void run(){
        LearningAIBehavior player1Behavior(30);
        LearningAIBehavior player2Behavior(30);

        // Set regenerative health
        getPlayer1()->setBehavior(&player1Behavior);
        getPlayer2()->setBehavior(&player2Behavior);

        stage->reset();
        Game::runMatch(stage.raw());
    }
};

void Game::startWatch(const std::string & player1Name, const std::string & player2Name, const std::string & stageName){
    StartWatch watch(player1Name, player2Name, stageName);
    watch.run();
}

class StartTeam: public StartGameMode {
public:
    StartTeam(const std::string & player1Name,
               const std::string & player2Name,
               const std::string & player3Name,
               const std::string & player4Name,
               const std::string & stageName):
    StartGameMode(player1Name, player2Name, player3Name, player4Name, stageName){
    }

    virtual void run(){
        /* FIXME: implement team stuff. Set up stage to be in team mode */
        LearningAIBehavior player1Behavior(30);
        LearningAIBehavior player2Behavior(30);

        // Set regenerative health
        getPlayer1()->setBehavior(&player1Behavior);
        getPlayer2()->setBehavior(&player2Behavior);

        stage->reset();
        Game::runMatch(stage.raw());
    }
};

void Game::startTeam(const std::string & player1Name, const std::string & player2Name, const std::string & player3Name, const std::string & player4Name, const std::string & stageName){
    StartTeam team(player1Name, player2Name, player3Name, player4Name, stageName);
    team.run();
}

class StartScript: public StartGameMode {
public:
    StartScript(const std::string & player1Name,
                const std::string & player1Script,
                const std::string & player2Name,
                const std::string & player2Script,
                const std::string & stageName):
    StartGameMode(player1Name, player2Name, stageName),
    player1Script(player1Script),
    player2Script(player2Script){
    }

    std::string player1Script;
    std::string player2Script;

    virtual void run(){
        Filesystem::AbsolutePath player1Path(player1Script);
        Filesystem::AbsolutePath player2Path(player2Script);
        ScriptedBehavior player1Behavior(player1Path);
        ScriptedBehavior player2Behavior(player2Path);

        // Set regenerative health
        getPlayer1()->setBehavior(&player1Behavior);
        getPlayer2()->setBehavior(&player2Behavior);

        stage->reset();
        Game::runMatch(stage.raw());
    }
};

void Game::startScript(const std::string & player1Name, const string & player1Script, const std::string & player2Name, const string & player2Script, const std::string & stageName){
    StartScript script(player1Name, player1Script, player2Name, player2Script, stageName);
    script.run();
}

void Game::doTraining(Searcher & searcher){
    int time = Mugen::Data::getInstance().getTime();
    Mugen::Data::getInstance().setTime(-1);
    while (true){
        Mugen::CharacterSelect select(systemFile);
        select.init();
        if (playerType == Mugen::Player1){
            select.setMode(Mugen::Training, Mugen::CharacterSelect::Player1);
        } else {
            select.setMode(Mugen::Training, Mugen::CharacterSelect::Player2);
        }
        InputMap<Mugen::Keys> keys1 = Mugen::getPlayer1Keys();
        InputMap<Mugen::Keys> keys2 = Mugen::getPlayer2Keys();
        HumanBehavior behavior = HumanBehavior(keys1, keys2);
        if (playerType == Player1){
            behavior = HumanBehavior(keys1, getPlayer1InputLeft());
        } else {
            behavior = HumanBehavior(keys2, getPlayer2InputLeft());
        }
        DummyBehavior dummyBehavior;
        PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = select.getLogic(keys1, keys2, searcher);
        PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = select.getDraw();
        PaintownUtil::standardLoop(*logic, *draw);
        
        if (select.wasCanceled()){
            Mugen::Data::getInstance().setTime(time);
            return;
        }
        
        Mugen::ArcadeData::CharacterCollection player1Collection(Mugen::ArcadeData::CharacterCollection::Single);
        Mugen::ArcadeData::CharacterCollection player2Collection(Mugen::ArcadeData::CharacterCollection::Single);
        
        if (playerType == Mugen::Player1){
            player1Collection = select.getPlayer1().getCollection();
            player2Collection = select.getPlayer1().getOpponentCollection();
        } else {
            player2Collection = select.getPlayer2().getCollection();
            player1Collection = select.getPlayer2().getOpponentCollection();
        }
        // Prepares futures
        CharacterTeam player1(player1Collection, Stage::Player1Side);
        CharacterTeam player2(player2Collection, Stage::Player2Side);
        PaintownUtil::ReferenceCount<PlayerLoader> loader = preLoadCharacters(player1, player2);
        
        {
            VersusMenu versus(systemFile);
            versus.init(player1Collection, player2Collection);
            PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = versus.getLogic(keys1, keys2);
            PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = versus.getDraw();
            PaintownUtil::standardLoop(*logic, *draw);
            if (versus.wasCanceled()){
                continue;
            }
        }
        
        RunMatchOptions options;
        
        if (playerType == Player1){
            player1.getFirst().setBehavior(&behavior);
            player2.getFirst().setBehavior(&dummyBehavior);
            options.setBehavior(&behavior, NULL);
        } else {
            player1.getFirst().setBehavior(&dummyBehavior);
            player2.getFirst().setBehavior(&behavior);
            options.setBehavior(NULL, &behavior);
        }
        player1.getFirst().setRegeneration(true);
        player2.getFirst().setRegeneration(true);
        Mugen::Stage stage(select.getStage());
        // Prepares stage
        prepareStage(loader, stage);
        stage.reset();
        searcher.pause();
        try {
            runMatch(&stage, "", options);
        } catch (const Exception::Return & ex){
        } catch (const QuitGameException & ex){
        }
    }
    Mugen::Data::getInstance().setTime(time);
}

void Game::doWatch(Searcher & searcher){
    while (true){
        Mugen::CharacterSelect select(systemFile);
        select.init();
        if (playerType == Mugen::Player1){
            select.setMode(Mugen::Watch, Mugen::CharacterSelect::Player1);
        } else {
            select.setMode(Mugen::Watch, Mugen::CharacterSelect::Player2);
        }
        InputMap<Mugen::Keys> keys1 = Mugen::getPlayer1Keys();
        InputMap<Mugen::Keys> keys2 = Mugen::getPlayer2Keys();
        LearningAIBehavior player1AIBehavior(Mugen::Data::getInstance().getDifficulty());
        LearningAIBehavior player2AIBehavior(Mugen::Data::getInstance().getDifficulty());
        PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = select.getLogic(keys1, keys2, searcher);
        PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = select.getDraw();
        PaintownUtil::standardLoop(*logic, *draw);
        
        if (select.wasCanceled()){
            return;
        }
        //! FIXME Get first for now, later support team/turns
        Mugen::ArcadeData::CharacterCollection player1Collection(Mugen::ArcadeData::CharacterCollection::Single);
        Mugen::ArcadeData::CharacterCollection player2Collection(Mugen::ArcadeData::CharacterCollection::Single);
        if (playerType == Mugen::Player1){
            player1Collection = select.getPlayer1().getCollection();
            player2Collection = select.getPlayer1().getOpponentCollection();
        } else {
            player2Collection = select.getPlayer2().getCollection();
            player1Collection = select.getPlayer2().getOpponentCollection();
        }
        
        // Prepares futures
        CharacterTeam player1(player1Collection, Stage::Player1Side);
        CharacterTeam player2(player2Collection, Stage::Player2Side);
        PaintownUtil::ReferenceCount<PlayerLoader> loader = preLoadCharacters(player1, player2);
        
        {
            VersusMenu versus(systemFile);
            versus.init(player1Collection, player2Collection);
            PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = versus.getLogic(keys1, keys2);
            PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = versus.getDraw();
            PaintownUtil::standardLoop(*logic, *draw);
            if (versus.wasCanceled()){
                continue;
            }
        }
        
        player1.getFirst().setBehavior(&player1AIBehavior);
        player2.getFirst().setBehavior(&player2AIBehavior);
        Mugen::Stage stage(select.getStage());
        // Prepare stage
        prepareStage(loader, stage);
        stage.reset();
        searcher.pause();
        try {
            runMatch(&stage);
        } catch (const Exception::Return & ex){
        } catch (const QuitGameException & ex){
        }
    }
}

static Filesystem::AbsolutePath findStoryBoard(const string & name, const Filesystem::AbsolutePath & file, const AstRef & parsed, const string & section, const string & item){
    try{
        return Storage::instance().lookupInsensitive(file.getDirectory(), Filesystem::RelativePath(Mugen::Util::probeDef(parsed, section, item)));
    } catch (const MugenException & fail){
        Global::debug(0) << "Failed to get " << name << " from " << file.path() << " " << fail.getReason() << std::endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Failed to get " << name << " from " << file.path() << " " << fail.getTrace() << std::endl;
    }

    return Filesystem::AbsolutePath();
}

namespace Mugen{

class GameScreens{
public:
    GameScreens(const Filesystem::AbsolutePath & playerDef, const Filesystem::AbsolutePath & path):
    displayWinScreen(false),
    defaultEndingEnabled(false),
    gameOverEnabled(false),
    creditsEnabled(false){
        AstRef playerParsed(Mugen::Util::parseDef(playerDef));
        intro = findStoryBoard("intro", playerDef, playerParsed, "arcade", "intro.storyboard");
        ending = findStoryBoard("ending", playerDef, playerParsed, "arcade", "ending.storyboard");

        AstRef parsed(Util::parseDef(path));

        try{
            // Win screen if player has ending it will not show this
            if (Util::probeDef(parsed, "win screen", "enabled") == "1"){
                displayWinScreen = true;
            }
        } catch (const MugenException & fail){
            Global::debug(0) << "Failed to get win screen from " << path.path() << " " << fail.getReason() << std::endl;
        } catch (const Filesystem::NotFound & fail){
            Global::debug(0) << "Failed to get win screen from " << path.path() << " " << fail.getTrace() << std::endl;
        }

        try{
            // Get Default ending
            if (Util::probeDef(parsed, "default ending", "enabled") == "1"){
                defaultEndingEnabled = true;
                defaultEnding = Data::getInstance().getFileFromMotif(Filesystem::RelativePath(Util::probeDef(parsed, "default ending", "storyboard")));
            }
        } catch (const MugenException & fail){
            Global::debug(0) << "Failed to get ending screen from " << path.path() << " " << fail.getReason() << std::endl;
        } catch (const Filesystem::NotFound & fail){
            Global::debug(0) << "Failed to get ending screen from " << path.path() << " " << fail.getTrace() << std::endl;
        }

        try{
            // Get Game Over
            if (Util::probeDef(parsed, "game over screen", "enabled") == "1"){
                gameOverEnabled = true;
                gameOver = Data::getInstance().getFileFromMotif(Filesystem::RelativePath(Util::probeDef(parsed, "game over screen", "storyboard")));
            }
        } catch (const MugenException & fail){
            Global::debug(0) << "Failed to get game over screen from " << path.path() << fail.getReason() << std::endl;
        } catch (const Filesystem::NotFound & fail){
            Global::debug(0) << "Failed to get game over screen from " << path.path() << fail.getTrace() << std::endl;
        }

        try{
            // Get credits
            if (Util::probeDef(parsed, "end credits", "enabled") == "1"){
                creditsEnabled = true;
                credits = Data::getInstance().getFileFromMotif(Filesystem::RelativePath(Util::probeDef(parsed, "end credits", "storyboard")));
            }
        } catch (const MugenException & fail){
            Global::debug(0) << "Failed to get end credits from " << path.path() << " " << fail.getReason() << std::endl;
        } catch (const Filesystem::NotFound & fail){
            Global::debug(0) << "Failed to get end credits from " << path.path() << " " << fail.getTrace() << std::endl;
        }
    }

    void playIntro(const InputMap<Keys> & keys){
        // Run intro before we begin game
        if (!intro.isEmpty()){
            try{
                Storyboard story(intro, true);
                story.setInput(keys);
                story.run();
            } catch (...){
                Global::debug(0) << "Failed to load storyboard " << intro.path() << " for some reason" << std::endl;
            }
        }
    }

    void playEnding(const InputMap<Keys> & keys){
        // Game is over and player has won display ending storyboard
        if (displayWinScreen && ending.isEmpty()){
            // Need to parse that and display it for now just ignore

            // Show Default ending if enabled
            if (defaultEndingEnabled){
                if (!defaultEnding.isEmpty()){
                    Storyboard story(defaultEnding, true);
                    story.setInput(keys);
                    story.run();
                }
            }
        } else if (defaultEndingEnabled && ending.isEmpty()){
            if (!defaultEnding.isEmpty()){
                Storyboard story(defaultEnding, true);
                story.setInput(keys);
                story.run();
            }
        } else if (!ending.isEmpty()){
            Storyboard story(ending, true);
            story.setInput(keys);
            story.run();
        } 
        if (creditsEnabled){                    
            // credits
            if (!credits.isEmpty()){
                Storyboard story(defaultEnding, true);
                story.setInput(keys);
                story.run();
            }
        }
    }

    void playGameOver(const InputMap<Keys> & keys){
        if (gameOverEnabled && !gameOver.isEmpty()){
            Storyboard story(gameOver, true);
            story.setInput(keys);
            story.run();
        }
    }

protected:
    Filesystem::AbsolutePath intro;
    Filesystem::AbsolutePath ending;
    bool displayWinScreen;
    bool defaultEndingEnabled;
    Filesystem::AbsolutePath defaultEnding;
    bool gameOverEnabled;
    Filesystem::AbsolutePath gameOver;
    bool creditsEnabled;
    Filesystem::AbsolutePath credits;
};

static PaintownUtil::ReferenceCount<Mugen::CharacterSelect> doSelectScreen(const Filesystem::AbsolutePath & systemFile, const GameType & gameType, const CharacterSelect::PlayerType & playerType, Searcher & searcher, InputMap<Keys> & keys1, InputMap<Keys> & keys2){
    PaintownUtil::ReferenceCount<Mugen::CharacterSelect> select = PaintownUtil::ReferenceCount<Mugen::CharacterSelect>(new Mugen::CharacterSelect(systemFile));
    select->init();
    select->setMode(gameType, playerType);

    PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = select->getLogic(keys1, keys2, searcher);
    PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = select->getDraw();
    PaintownUtil::standardLoop(*logic, *draw);

    if (select->wasCanceled()){
        throw CanceledException();
    }

    /* Wait for at least 1 stage to be found */
    while (Data::getInstance().autoSearch() &&
           select->stageCount() == 0 &&
           !searcher.stagesDone()){
        PaintownUtil::rest(1);
    }

    return select;
    
}

static void showLoadPlayers(const Filesystem::AbsolutePath & systemFile, const Mugen::ArcadeData::CharacterCollection & player1Collection, const Mugen::ArcadeData::CharacterCollection & player2Collection, InputMap<Keys> & keys1, InputMap<Keys> & keys2){

    VersusMenu versus(systemFile);
    versus.init(player1Collection, player2Collection);
    PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = versus.getLogic(keys1, keys2);
    PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = versus.getDraw();
    PaintownUtil::standardLoop(*logic, *draw);
    if (versus.wasCanceled()){
        throw CanceledException();
    }
}

CharacterSelect::PlayerType characterSelectType(const Mugen::PlayerType & type){
    switch (type){
        case Mugen::Player1: {
            return CharacterSelect::Player1;
            break;
        }
        case Mugen::Player2: {
            return CharacterSelect::Player2;
            break;
        }
        case Mugen::CPU:
        case Mugen::NoControl: {
            throw MugenException("Should not be able to start an arcade match with type CPU or NoControl", __FILE__, __LINE__);
            break;
        }
        default: throw MugenException("Internal error: unknown player type given", __FILE__, __LINE__);
    }
}

Mugen::ArcadeData::CharacterCollection & choosePlayerCollection(PlayerType type, ArcadeData::CharacterCollection & player1, ArcadeData::CharacterCollection & player2){
    switch (type){
        case Player1: return player1;
        case Player2: return player2;
        case CPU:
        case NoControl:{
            throw MugenException("Unsupported player collection", __FILE__, __LINE__);
        }
    }

    return player1;
}

Mugen::ArcadeData::CharacterCollection & chooseEnemyCollection(PlayerType type, ArcadeData::CharacterCollection & player1, ArcadeData::CharacterCollection & player2){
    switch (type){
        case Player1: return player2;
        case Player2: return player1;
        case CPU:
        case NoControl:{
            throw MugenException("Unsupported player collection", __FILE__, __LINE__);
        }
    }

    return player2;
}

/* Returns true if the player wins the match */
bool runArcade(const Filesystem::AbsolutePath & systemFile, ArcadeData::CharacterCollection & playerCollection, ArcadeData::CharacterCollection & enemyCollection, HumanBehavior & behavior, InputMap<Keys> & keys1, InputMap<Keys> & keys2, RunMatchOptions & options, Searcher & searcher, PlayerType playerType, InputMap<Keys> & playerKeys){
    Filesystem::AbsolutePath musicOverride;
    LearningAIBehavior AIBehavior(Mugen::Data::getInstance().getDifficulty());

    Filesystem::AbsolutePath stagePath = enemyCollection.getFirst().getStage();
    if (stagePath.path() == ""){
        throw MugenException("No stages available", __FILE__, __LINE__);
    }

    /* Keep rematching until the player wins or they stop continuing */
    while (true){
        /* FIXME: player is always on p1 side, is that ok? */
        PaintownUtil::ReferenceCount<CharacterTeam> player(new CharacterTeam(playerCollection, Stage::Player1Side));
        PaintownUtil::ReferenceCount<CharacterTeam> enemy(PaintownUtil::ReferenceCount<CharacterTeam>(new CharacterTeam(enemyCollection, Stage::Player2Side)));

        player->getFirst().setBehavior(&behavior);
        enemy->getFirst().setBehavior(&AIBehavior);

        PaintownUtil::ReferenceCount<PlayerLoader> loader = preLoadCharacters(*player, *enemy);
        showLoadPlayers(systemFile, playerCollection, enemyCollection, keys1, keys2);

        // FIXME use override music later
        Mugen::Stage stage(stagePath);
        prepareStage(loader, stage);
        stage.reset();

        int wins = player->getFirst().getMatchWins();
        searcher.pause();
        try{
            Game::runMatch(&stage, "", options);
        } catch (...){
            searcher.start();
            throw;
        }

        if (player->getFirst().getMatchWins() > wins){
            return true;
        } else {
            if (stage.doContinue(playerType, playerKeys)){
                PaintownUtil::ReferenceCount<Mugen::CharacterSelect> select = doSelectScreen(systemFile, Arcade, characterSelectType(playerType), searcher, keys1, keys2);

                if (playerType == Mugen::Player1){
                    playerCollection = select->getPlayer1().getCollection();
                } else {
                    playerCollection = select->getPlayer2().getCollection();
                }

            } else {
                return false;
            }
        }
    }
}

}

void Game::doArcade(Searcher & searcher){
    InputMap<Mugen::Keys> keys1 = Mugen::getPlayer1Keys();
    InputMap<Mugen::Keys> keys2 = Mugen::getPlayer2Keys();
    InputMap<Mugen::Keys> playerKeys;
    HumanBehavior behavior(keys1, keys2);
    
    Mugen::ArcadeData::CharacterCollection player1Collection(Mugen::ArcadeData::CharacterCollection::Single);
    Mugen::ArcadeData::CharacterCollection player2Collection(Mugen::ArcadeData::CharacterCollection::Single);
    
    // Match data
    Mugen::ArcadeData::MatchPath match;
    
    RunMatchOptions options;

    {
        PaintownUtil::ReferenceCount<Mugen::CharacterSelect> select = doSelectScreen(systemFile, Arcade, characterSelectType(playerType), searcher, keys1, keys2);

        match = select->getArcadePath();

        switch (playerType){
            case Mugen::Player1: {
                player1Collection = select->getPlayer1().getCollection();
                playerKeys = keys1;
                behavior = HumanBehavior(getPlayer1Keys(), getPlayer1InputLeft());
                options.setBehavior(&behavior, NULL);
                break;
            }
            case Mugen::Player2: {
                player2Collection = select->getPlayer2().getCollection();
                playerKeys = keys2;
                behavior = HumanBehavior(getPlayer2Keys(), getPlayer2InputLeft());
                options.setBehavior(NULL, &behavior);
                break;
            }
            case Mugen::CPU:
            case Mugen::NoControl: {
                break;
            }
        }
    }
    
    Mugen::ArcadeData::CharacterCollection & playerCollection = choosePlayerCollection(playerType, player1Collection, player2Collection);
    Mugen::ArcadeData::CharacterCollection & enemyCollection = chooseEnemyCollection(playerType, player1Collection, player2Collection);
    
    GameScreens screens(playerCollection.getFirst().getDef(), systemFile);
    screens.playIntro(playerKeys);

    try{
        bool quit = false;
    
        while (!quit){
            enemyCollection = match.next();
            
            quit = ! runArcade(systemFile, playerCollection, enemyCollection, behavior, keys1, keys2, options, searcher, playerType, playerKeys);

            if (!quit && !match.hasMore()){
                screens.playEnding(playerKeys);
                quit = true;
            }
        }

        screens.playGameOver(playerKeys);
    } catch (const Exception::Return & ex){
    } catch (const QuitGameException & ex){
    }
}

void Game::doVersus(Searcher & searcher){
    while (true){
        Mugen::CharacterSelect select(systemFile);
        select.init();
        select.setMode(Mugen::Versus, Mugen::CharacterSelect::Both);
        InputMap<Mugen::Keys> keys1 = Mugen::getPlayer1Keys();
        InputMap<Mugen::Keys> keys2 = Mugen::getPlayer2Keys();
        HumanBehavior behavior1 = HumanBehavior(keys1, getPlayer1InputLeft());
        HumanBehavior behavior2 = HumanBehavior(keys2, getPlayer2InputLeft());
        PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = select.getLogic(keys1, keys2, searcher);
        PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = select.getDraw();
        PaintownUtil::standardLoop(*logic, *draw);
        
        RunMatchOptions options;
        options.setBehavior(&behavior1, &behavior2);
        
        if (select.wasCanceled()){
            return;
        }
        Mugen::ArcadeData::CharacterCollection player1Collection(Mugen::ArcadeData::CharacterCollection::Single);
        Mugen::ArcadeData::CharacterCollection player2Collection(Mugen::ArcadeData::CharacterCollection::Single);
        player1Collection = select.getPlayer1().getCollection();
        player2Collection = select.getPlayer2().getCollection();
        
        // Prepares futures
        CharacterTeam player1(player1Collection, Stage::Player1Side);
        CharacterTeam player2(player2Collection, Stage::Player2Side);
        PaintownUtil::ReferenceCount<PlayerLoader> loader = preLoadCharacters(player1, player2);
        
        {
            VersusMenu versus(systemFile);
            versus.init(player1Collection, player2Collection);
            PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = versus.getLogic(keys1, keys2);
            PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = versus.getDraw();
            PaintownUtil::standardLoop(*logic, *draw);
            if (versus.wasCanceled()){
                continue;
            }
        }
        
        player1.getFirst().setBehavior(&behavior1);
        player2.getFirst().setBehavior(&behavior2);
        Mugen::Stage stage(select.getStage());
        prepareStage(loader, stage);
        stage.reset();
        searcher.pause();
        try {
            runMatch(&stage, "", options);
        } catch (const Exception::Return & ex){
        } catch (const QuitGameException & ex){
        }
    }
}

static int getNumber(const std::string & number){
    return atoi(number.c_str());
}

void Game::doNetworkVersus(bool isServer, Searcher & searcher){
#ifdef HAVE_NETWORKING
    // Keys
    InputMap<Mugen::Keys> keys1 = Mugen::getPlayer1Keys();
    InputMap<Mugen::Keys> keys2 = Mugen::getPlayer2Keys();
    // Local behaviors
    HumanBehavior behavior1 = HumanBehavior(keys1, getPlayer1InputLeft());
    PaintownUtil::ReferenceCount<NetworkLocalBehavior> player1LocalBehavior, player2LocalBehavior;
    PaintownUtil::ReferenceCount<NetworkRemoteBehavior> player1RemoteBehavior, player2RemoteBehavior;
    
    try {
        /*! TODO Network Local Behavior
            create or bind to socket */
        int port = 8473;
        std::string host = "localhost";
        Network::Socket socket;
        if (isServer){
            port = getNumber(OptionMenu::doInputDialog("Enter Port", "8473", true, Graphics::makeColor(0,0,0), 128, true));
            Network::Socket remote = Network::openReliable(port);
            Network::listen(remote);
            Global::debug(0) << "Waiting for a connection on port " << port << std::endl;
            socket = Network::accept(remote);
            Network::close(remote);
            Global::debug(0) << "Got a connection" << std::endl;
            
            player1LocalBehavior = PaintownUtil::ReferenceCount<NetworkLocalBehavior>(new NetworkLocalBehavior(&behavior1, socket));
            player2RemoteBehavior = PaintownUtil::ReferenceCount<NetworkRemoteBehavior>(new NetworkRemoteBehavior(socket));
        } else {
            std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
            OptionMenu options(list);
            port = getNumber(OptionMenu::doInputDialog("Enter Port", "8473", true, Graphics::makeColor(0,0,0), 128, true, &options));
            host = OptionMenu::doInputDialog("Enter Hostname", "000.000.000.000", true, Graphics::makeColor(0,0,0), 128, true, &options);
            Global::debug(0) << "Connecting to " << host << " on port " << port << std::endl;
            socket = Network::connectReliable(host, port); 
            Global::debug(0) << "Connected" << std::endl;
            player2LocalBehavior = PaintownUtil::ReferenceCount<NetworkLocalBehavior>(new NetworkLocalBehavior(&behavior1, socket));
            player1RemoteBehavior = PaintownUtil::ReferenceCount<NetworkRemoteBehavior>(new NetworkRemoteBehavior(socket));
        }
    
        while (true){
            // Discuss before fight
            
            Mugen::CharacterSelect select(systemFile);
            select.init();
            // FIXME Make characterselect network useable
            select.setMode(Mugen::Arcade, Mugen::CharacterSelect::Player1);
            select.overrideTitle((isServer ? "(Server Mode) " : "(Client Mode - Host: " + host + ") ") + " Select Player");
            
            PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = select.getLogic(keys1, keys2, searcher);
            PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = select.getDraw();
            PaintownUtil::standardLoop(*logic, *draw);
            
            RunMatchOptions options;
            
            if (select.wasCanceled()){
                return;
            }

#ifdef HAVE_NETWORKING
            if (isServer){
                int sync = Network::read16(socket);
                Network::send16(socket, sync);
            } else {
                Network::send16(socket, 0);
                Network::read16(socket);
            }

            if (!Network::blocking(socket, false)){
                Global::debug(0) << "Could not set socket to be non-blocking" << std::endl;
            }
#endif

            Mugen::ArcadeData::CharacterCollection player1Collection(Mugen::ArcadeData::CharacterCollection::Single);
            Mugen::ArcadeData::CharacterCollection player2Collection(Mugen::ArcadeData::CharacterCollection::Single);
            if (isServer){
                // Get remote character
                const std::string & remotePath = Mugen::NetworkUtil::readMessage(socket);
                Mugen::ArcadeData::CharacterInfo remoteCharacter(Storage::instance().find(Filesystem::RelativePath(remotePath)));
                player2Collection.setFirst(remoteCharacter);
                options.setBehavior(&behavior1, NULL);
                // Send chosen character
                player1Collection = select.getPlayer1().getCollection();
                std::string localPath = Storage::instance().cleanse(player1Collection.getFirst().getDef()).path();
                Mugen::NetworkUtil::sendMessage(localPath, socket);
            } else {
                // Send chosen character
                player2Collection = select.getPlayer1().getCollection();
                std::string localPath = Storage::instance().cleanse(player1Collection.getFirst().getDef()).path();
                Mugen::NetworkUtil::sendMessage(localPath, socket);
                // Get remote character
                const std::string & remotePath = Mugen::NetworkUtil::readMessage(socket);
                Mugen::ArcadeData::CharacterInfo remoteCharacter(Storage::instance().find(Filesystem::RelativePath(remotePath)));
                player1Collection.setFirst(remoteCharacter);
                options.setBehavior(NULL, &behavior1);
            }
            
            // Prepares futures
            CharacterTeam player1(player1Collection, Stage::Player1Side);
            CharacterTeam player2(player2Collection, Stage::Player2Side);
            PaintownUtil::ReferenceCount<PlayerLoader> loader = preLoadCharacters(player1, player2);
            
            {
                VersusMenu versus(systemFile);
                versus.init(player1Collection, player2Collection);
                PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = versus.getLogic(keys1, keys2);
                PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = versus.getDraw();
                PaintownUtil::standardLoop(*logic, *draw);
                if (versus.wasCanceled()){
                    continue;
                }
            }
            if (isServer){
                player1.getFirst().setBehavior(player1LocalBehavior.raw());
                player2.getFirst().setBehavior(player2RemoteBehavior.raw());
            } else {
                player1.getFirst().setBehavior(player1RemoteBehavior.raw());
                player2.getFirst().setBehavior(player2LocalBehavior.raw());
            }
            Mugen::Stage stage(select.getStage());
            prepareStage(loader, stage);
            stage.reset();
            
            if (isServer){
                player1LocalBehavior->begin();
                player2RemoteBehavior->begin();
            } else {
                player2LocalBehavior->begin();
                player1RemoteBehavior->begin();
            }
            searcher.pause();
            try {
                runMatch(&stage, "", options);
            } catch (const Exception::Return & ex){
            } catch (const QuitGameException & ex){
            }
            searcher.start();
        }
    } catch (const Network::NetworkException & ex){
        Global::debug(0) << "Problem with network connection! Reason: " << ex.getMessage();
    }
#endif
}

void Game::startDemo(Searcher & searcher){
    
    // Display select screen?
    bool showSelectScreen = false;
    bool showVersusScreen = false;
    int endTime = 1500;
    bool displayFightBars = false;
    AstRef parsed(Mugen::Util::parseDef(systemFile));
    if (Util::probeDef(parsed, "demo mode", "select.enabled") == "1"){
        showSelectScreen = true;
    }
    if (Util::probeDef(parsed, "demo mode", "vsscreen.enabled") == "1"){
        showVersusScreen = true;
    }
    {
        std::string temp = Util::probeDef(parsed, "demo mode", "fight.endtime");
        if (!temp.empty()){
            endTime = atoi(temp.c_str());
        }
    }
    if (Util::probeDef(parsed, "demo mode", "fight.bars.display") == "1"){
        showSelectScreen = true;
    }
    /*select.enabled = 1        ;Set to 1 to display select screen, 0 to disable
    vsscreen.enabled = 1      ;Set to 1 to display versus screen, 0 to disable
    title.waittime = 600      ;Time to wait at title before starting demo mode
    fight.endtime = 1500      ;Time to display the fight before returning to title
    fight.playbgm = 0         ;Set to 1 to enable in-fight BGM, 0 to disable
    fight.stopbgm = 1         ;Set to 1 to stop title BGM (only if playbgm = 0)
    fight.bars.display = 0    ;Set to 1 to display lifebar, 0 to disable
    intro.waitcycles = 1      ;Cycles to wait before intro cutscene is played again
    debuginfo = 0             ;Set to 0 to hide debugging info (debug mode only)*/

    
    Mugen::ArcadeData::CharacterCollection player1Collection(Mugen::ArcadeData::CharacterCollection::Single);
    Mugen::ArcadeData::CharacterCollection player2Collection(Mugen::ArcadeData::CharacterCollection::Single);
    Filesystem::AbsolutePath stagePath;
    
    InputMap<Mugen::Keys> keys1;
    InputMap<Mugen::Keys> keys2;
    LearningAIBehavior behavior1(Mugen::Data::getInstance().getDifficulty());
    LearningAIBehavior behavior2(Mugen::Data::getInstance().getDifficulty());
    
    // Display Select screen?
    if (showSelectScreen){
        // Clear it before continuing
        InputManager::waitForClear();
        Mugen::CharacterSelect select(systemFile);
        select.init();
        select.setMode(Mugen::Versus, Mugen::CharacterSelect::Demo);
        PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = select.getLogic(keys1, keys2, searcher);
        PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = select.getDraw();
        PaintownUtil::standardLoop(*logic, *draw);
        
        if (select.wasCanceled()){
            return;
        }
        player1Collection = select.getPlayer1().getCollection();
        player2Collection = select.getPlayer2().getCollection();
        stagePath = select.getStage();
    } else {
        class Collections{
        public:
            Collections(){
            }
            ~Collections(){
            }
            void addCharacter(const Filesystem::AbsolutePath & path){
                PaintownUtil::Thread::ScopedLock scoped(lock);
                try {
                    // Check for name of character ... if it doesn't find this it probably isn't a character
                    Util::probeDef(path, "info", "name");
                    allCharacters.push_back(path);
                } catch (const MugenException & ex){
                }
            }
            
            void addStage(const Filesystem::AbsolutePath & path){
                PaintownUtil::Thread::ScopedLock scoped(lock);
                try {
                    // Just like character check for name of stage ... if it doesn't find this it probably isn't a stage
                    Util::probeDef(path, "info", "name");
                    allStages.push_back(path);
                } catch (const MugenException & ex){
                }
            }

            const Filesystem::AbsolutePath getCharacter(){
                PaintownUtil::Thread::ScopedLock scoped(lock);
                return allCharacters[PaintownUtil::rnd(allCharacters.size())];
            }

            const Filesystem::AbsolutePath getStage(){
                PaintownUtil::Thread::ScopedLock scoped(lock);
                return allStages[PaintownUtil::rnd(allStages.size())];
            }
            
            bool isEmpty(){
                PaintownUtil::Thread::ScopedLock scoped(lock);
                return (allCharacters.empty() || allStages.empty());
            }
            
        private:
            std::vector<Filesystem::AbsolutePath> allCharacters;
            std::vector<Filesystem::AbsolutePath> allStages;
            PaintownUtil::Thread::LockObject lock;
        } collections;
        
        class Subscriber: public Searcher::Subscriber {
        public:
            Subscriber(Collections & collections):
            collections(collections){
            }
            
            Collections & collections;
        
            virtual ~Subscriber(){
            }
        
            virtual void receiveCharacters(const std::vector<Filesystem::AbsolutePath> & paths){
                for (std::vector<Filesystem::AbsolutePath>::const_iterator it = paths.begin(); it != paths.end(); it++){
                    const Filesystem::AbsolutePath & path = *it;
                    collections.addCharacter(path);
                }
            }

            virtual void receiveStages(const std::vector<Filesystem::AbsolutePath> & paths){
                for (std::vector<Filesystem::AbsolutePath>::const_iterator it = paths.begin(); it != paths.end(); it++){
                    const Filesystem::AbsolutePath & path = *it;
                    collections.addStage(path);
                }
            }
        } subscription(collections);
        
        class WithSubscription{
        public:
            WithSubscription(Searcher & search, Searcher::Subscriber & subscription):
            subscribeThread(PaintownUtil::Thread::uninitializedValue),
            search(search),
            subscription(subscription){
                if (!PaintownUtil::Thread::createThread(&subscribeThread, NULL, (PaintownUtil::Thread::ThreadFunction) subscribe, this)){
                    doSubscribe();
                }
            }

            PaintownUtil::Thread::Id subscribeThread;

            /* Start the subscription in a thread so that characters that are already found
            * will be added in a separate thread instead of the main one
            */
            static void * subscribe(void * me){
                WithSubscription * self = (WithSubscription*) me;
                self->doSubscribe();
                return NULL;
            }

            void doSubscribe(){
                // Only subscribe if auto search is enabled
                if (Data::getInstance().autoSearch()){
                    search.subscribe(&subscription);
                }
            }

            Searcher & search;
            Searcher::Subscriber & subscription;

            virtual ~WithSubscription(){
                /* Make sure we wait for the initial join to finish before trying to unsubscribe */
                PaintownUtil::Thread::joinThread(subscribeThread);
                search.unsubscribe(&subscription);
            }
        } withSubscription(searcher, subscription);
        
        Filesystem::AbsolutePath path1;
        Filesystem::AbsolutePath path2;
        
        int currentTime = System::currentSeconds();
        Global::debug(1) << "Waiting for search..." << std::endl;
        int i = 0;
        while (collections.isEmpty()){
            if (System::currentSeconds() >= (currentTime + 4)){
                Global::debug(1) << "Couldn't find characters or stages within a reasonable time, Aborting." << std::endl;
                return;
            }
            PaintownUtil::rest(1);
            Global::debug(1) << "Time elapsed: " << i << std::endl;
        }
        
        path1 = collections.getCharacter();
        path2 = collections.getCharacter();
        
        Global::debug(1) << "Got player1: " << path1.path() << std::endl;
        Global::debug(1) << "Got player2: " << path2.path() << std::endl;
        
        player1Collection.setFirst(Mugen::ArcadeData::CharacterInfo(path1));
        player2Collection.setFirst(Mugen::ArcadeData::CharacterInfo(path2));
        
        stagePath = collections.getStage();
    }
    
    // Prepares futures
    CharacterTeam player1(player1Collection, Stage::Player1Side);
    CharacterTeam player2(player2Collection, Stage::Player2Side);
    PaintownUtil::ReferenceCount<PlayerLoader> loader = preLoadCharacters(player1, player2);
    
    if (showVersusScreen){
        InputManager::waitForClear();
        VersusMenu versus(systemFile, true);
        versus.init(player1Collection, player2Collection);
        PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = versus.getLogic(keys1, keys2);
        PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = versus.getDraw();
        PaintownUtil::standardLoop(*logic, *draw);
        if (versus.wasCanceled()){
            return;
        }
    }
    
    // Load it up
    player1.getFirst().setBehavior(&behavior1);
    player2.getFirst().setBehavior(&behavior2);
    Mugen::Stage stage(stagePath);
    prepareStage(loader, stage);
    stage.reset();
    searcher.pause();
    try {
        InputManager::waitForClear();
        
        runMatch(&stage, "", RunMatchOptions(endTime));
    } catch (const Exception::Return & ex){
    } catch (const QuitGameException & ex){
    }
}

template<typename T>
static T getRandom(const std::vector<T> & vec){
    return vec[PaintownUtil::rnd(vec.size())];
}

void Game::doSurvival(Searcher & searcher){
    InputMap<Mugen::Keys> keys1 = Mugen::getPlayer1Keys();
    InputMap<Mugen::Keys> keys2 = Mugen::getPlayer2Keys();
    InputMap<Mugen::Keys> playerKeys;
    HumanBehavior behavior(keys1, keys2);
    LearningAIBehavior AIBehavior(Mugen::Data::getInstance().getDifficulty());
    
    Mugen::ArcadeData::CharacterCollection player1Collection(Mugen::ArcadeData::CharacterCollection::Single);
    Mugen::ArcadeData::CharacterCollection player2Collection(Mugen::ArcadeData::CharacterCollection::Single);
    
    PaintownUtil::ReferenceCount<CharacterTeam> player1 = PaintownUtil::ReferenceCount<CharacterTeam>(NULL);
    PaintownUtil::ReferenceCount<CharacterTeam> player2 = PaintownUtil::ReferenceCount<CharacterTeam>(NULL);
    
    std::vector<Mugen::ArcadeData::CharacterInfo> characters;
    
    Filesystem::AbsolutePath stagePath;
    
    RunMatchOptions options;
    
    // Scoped so it doesn't persist
    {
        Mugen::CharacterSelect select(systemFile);
        select.init();
        if (playerType == Mugen::Player1){
            select.setMode(Mugen::Survival, Mugen::CharacterSelect::Player1);
        } else {
            select.setMode(Mugen::Survival, Mugen::CharacterSelect::Player2);
        }
        PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = select.getLogic(keys1, keys2, searcher);
        PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = select.getDraw();
        PaintownUtil::standardLoop(*logic, *draw);
        
        if (select.wasCanceled()){
            return;
        }
    
        if (playerType == Mugen::Player1){
            player1Collection = select.getPlayer1().getCollection();
            playerKeys = keys1;
            behavior = HumanBehavior(getPlayer1Keys(), getPlayer1InputLeft());
            player2Collection = select.getPlayer1().getOpponentCollection();
            options.setBehavior(&behavior, NULL);
        } else {
            player2Collection = select.getPlayer2().getCollection();
            playerKeys = keys2;
            behavior = HumanBehavior(getPlayer2Keys(), getPlayer2InputLeft());
            player1Collection = select.getPlayer2().getOpponentCollection();
            options.setBehavior(NULL, &behavior);
        }
        
        characters = select.getCharacters();
        stagePath = select.getStage();
    }
    
    int wins = 0;
    bool playerLoaded = false;
    
    while (true){
        if (playerType == Mugen::Player1){
            switch (player2Collection.getType()){
                case Mugen::ArcadeData::CharacterCollection::Turns4:
                    player2Collection.setFourth(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                case Mugen::ArcadeData::CharacterCollection::Turns3:
                    player2Collection.setThird(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                case Mugen::ArcadeData::CharacterCollection::Turns2:
                case Mugen::ArcadeData::CharacterCollection::Simultaneous:
                    player2Collection.setSecond(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                case Mugen::ArcadeData::CharacterCollection::Single:
                default:
                    player2Collection.setFirst(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                    break;
            }
        } else {
            switch (player1Collection.getType()){
                case Mugen::ArcadeData::CharacterCollection::Turns4:
                    player1Collection.setFourth(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                case Mugen::ArcadeData::CharacterCollection::Turns3:
                    player1Collection.setThird(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                case Mugen::ArcadeData::CharacterCollection::Turns2:
                case Mugen::ArcadeData::CharacterCollection::Simultaneous:
                    player1Collection.setSecond(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                case Mugen::ArcadeData::CharacterCollection::Single:
                default:
                    player1Collection.setFirst(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                    break;
            }
        }
        
        PaintownUtil::ReferenceCount<CharacterTeam> ourPlayer = PaintownUtil::ReferenceCount<CharacterTeam>(NULL);
        if (playerType == Mugen::Player1){
            if (!playerLoaded){
                player1 = PaintownUtil::ReferenceCount<CharacterTeam>(new CharacterTeam(player1Collection, Stage::Player1Side));
                ourPlayer = player1;
                player1->getFirst().setBehavior(&behavior);
                playerLoaded = true;
                options.setBehavior(&behavior, NULL);
            }
            
            player2 = PaintownUtil::ReferenceCount<CharacterTeam>(new CharacterTeam(player2Collection, Stage::Player2Side));
            player2->getFirst().setBehavior(&AIBehavior);
        } else {
            if (!playerLoaded){
                player2 = PaintownUtil::ReferenceCount<CharacterTeam>(new CharacterTeam(player2Collection, Stage::Player2Side));
                ourPlayer = player2;
                player2->getFirst().setBehavior(&behavior);
                playerLoaded = true;
                options.setBehavior(NULL, &behavior);
            }
            
            player1 = PaintownUtil::ReferenceCount<CharacterTeam>(new CharacterTeam(player1Collection, Stage::Player1Side));
            player1->getFirst().setBehavior(&AIBehavior);
        }
        
        PaintownUtil::ReferenceCount<PlayerLoader> loader = preLoadCharacters(*player1, *player2);
        
        {
            VersusMenu versus(systemFile);
            versus.init(player1Collection, player2Collection);
            PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = versus.getLogic(keys1, keys2);
            PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = versus.getDraw();
            PaintownUtil::standardLoop(*logic, *draw);
            if (versus.wasCanceled()){
                return;
            }
        }
        
        // FIXME use override music later
        Mugen::Stage stage(stagePath);
        prepareStage(loader, stage);
        // FIXME Add mode in stage to keep from reverting the wins and allowing matches to last 1 round
        stage.getGameInfo()->setWins(1, 0);
        stage.reset();
        try {
            searcher.pause();
            runMatch(&stage, "", options);
            if (ourPlayer->getFirst().getMatchWins()> wins){
                wins++;
                // Reset player for next match
                ourPlayer->getFirst().resetPlayer();
            } else {
                break;
            }
        } catch (const Exception::Return & ex){
            return;
        } catch (const QuitGameException & ex){
            return;
        }
    }
    // FIXME show total matches won (very similar to Stage::doContinue)
    Global::debug(0) << "Matches Survived: " << wins << std::endl;
}
