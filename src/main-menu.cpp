#include <string.h>
#include <string>
#include <sstream>
#include <vector>

#include "util/bitmap.h"

#include "factory/collector.h"
#include "util/network/network.h"
#include "util/token_exception.h"
#include "util/system.h"
#include "mugen/exception.h"
#include "mugen/menu.h"
#include "mugen/game.h"
#include "mugen/options.h"
#include "util/music.h"
#include "util/menu/menu.h"
#include "util/menu/menu-exception.h"
#include "util/menu/optionfactory.h"
#include "util/input/input-manager.h"
#include "paintown-engine/game/options.h"
#include "paintown-engine/game/mod.h"
#include "paintown-engine/network/client.h"
#include "paintown-engine/network/server.h"
#include "platformer/game/options.h"
#include "util/exceptions/shutdown_exception.h"
#include "util/exceptions/exception.h"
#include "util/timedifference.h"
#include "util/funcs.h"
#include "util/ftalleg.h"
#include "util/file-system.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/parameter.h"
#include "globals.h"
#include "util/debug.h"
#include "configuration.h"
#include "util/init.h"
#include "util/main.h"
#include "util/argument.h"
#include "mugen/config.h"

#include <iostream>

using std::vector;
using std::endl;
using std::string;
using std::istringstream;

class WindowedArgument: public Argument {
public:
    WindowedArgument(int * gfx):
    gfx(gfx){
    }

    int * gfx;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("-w");
        out.push_back("fullscreen");
        out.push_back("nowindowed");
        out.push_back("no-windowed");
        return out;
    }

    string description() const {
        return " : Start in fullscreen mode";
    }
    
    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        *gfx = Global::FULLSCREEN;
        return current;
    }
};

class DataPathArgument: public Argument {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("-d");
        out.push_back("--data");
        out.push_back("data");
        out.push_back("datapath");
        out.push_back("data-path");
        out.push_back("path");
        return out;
    }

    string description() const {
        std::ostringstream out;
        out << " <path> : Use data path of <path>. Default is " << Util::getDataPath2().path();
        return out.str();
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        current++;
        if (current != end){
            Util::setDataPath(*current);
        }
        return current;
    }
};

class DisableQuitArgument: public Argument {
public:
    DisableQuitArgument(bool * quit):
    quit(quit){
    }

    bool * quit;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("disable-quit");
        return out;
    }
    
    string description() const {
        return " : Don't allow the game to exit using the normal methods";
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        *quit = false;
        return current;
    }
};

class MusicArgument: public Argument {
public:
    MusicArgument(bool * enabled):
        enabled(enabled){
        }

    bool * enabled;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("-m");
        out.push_back("music");
        out.push_back("nomusic");
        out.push_back("no-music");
        return out;
    }

    string description() const {
        return " : Turn off music";
    }
    
    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        *enabled = false;
        return current;
    }
};

class DebugArgument: public Argument {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("-l");
        out.push_back("--debug");
        out.push_back("debug");
        return out;
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        current++;
        if (current != end){
            istringstream i(*current);
            int f;
            i >> f;
            Global::setDebug(f);
        }
        return current;
    }
    
    string description() const {
        return " # : Enable debug statements. Higher numbers gives more debugging. Default is 0. Negative numbers are allowed. Example: -l 3";
    }
};

class RateLimitArgument: public Argument {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("fps");
        out.push_back("rate-limit");
        return out;
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        Global::rateLimit = false;
        return current;
    }

    string description() const {
        return " : Don't rate limit the game to the default fps (40). This is mainly useful for benchmarking graphics capabilities.";
    }
};

class JoystickArgument: public Argument {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("joystick");
        out.push_back("nojoystick");
        out.push_back("no-joystick");
        return out;
    }

    string description() const {
        return " : Disable joystick input";
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & refs){
        Configuration::setJoystickEnabled(false);
        return current;
    }
};

static Filesystem::AbsolutePath mainMenuPath(){
    string menu = Paintown::Mod::getCurrentMod()->getMenu();
    return Storage::instance().find(Filesystem::RelativePath(menu));
}

static void setMugenMotif(const Filesystem::AbsolutePath & path){
    std::string motif;
    try {
        *Mugen::Configuration::get("motif") >> motif;
    } catch (const std::ios_base::failure & ex){
        motif.clear();
    }
    if (!motif.empty()){
        Mugen::Data::getInstance().setMotif(Filesystem::RelativePath(motif));
    } else {
        TokenReader reader;
        Token * head = reader.readToken(path.path());
        const Token * motif = head->findToken("menu/option/mugen/motif");
        if (motif != NULL){
            string path;
            motif->view() >> path;
            Mugen::Data::getInstance().setMotif(Filesystem::RelativePath(path));
        }
    }
}

class MugenArgument: public Argument {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("mugen");
        out.push_back("--mugen");
        return out;
    }

    string description() const {
        return " : Go directly to the mugen menu";
    }

    class Run: public ArgumentAction {
    public:
        virtual void act(){
            setMugenMotif(mainMenuPath());
            Mugen::run();
        }
    };

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        actions.push_back(Util::ReferenceCount<ArgumentAction>(new Run()));
        return current;
    }
};

static bool parseMugenInstant(string input, string * player1, string * player2, string * stage){
    unsigned int comma = input.find(',');
    if (comma == string::npos){
        Global::debug(0) << "Expected three arguments separated by a comma, only 1 was given: " << input << endl;
        return false;
    }
    *player1 = input.substr(0, comma);
    input.erase(0, comma + 1);
    comma = input.find(',');

    if (comma == string::npos){
        Global::debug(0) << "Expected three arguments separated by a comma, only 2 were given: " << input << endl;
        return false;
    }

    *player2 = input.substr(0, comma);
    input.erase(0, comma + 1);
    *stage = input;

    return true;
}

struct MugenInstant{
    enum Kind{
        None,
        Training,
        Watch,
        Arcade,
        Script
    };

    MugenInstant():
        enabled(false),
        kind(None){
        }

    bool enabled;
    string player1;
    string player2;
    string player1Script;
    string player2Script;
    string stage;
    Kind kind;
}; 

class MugenTrainingArgument: public Argument {
public:

    MugenInstant data;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("mugen:training");
        return out;
    }

    string description() const {
        return " <player 1 name>,<player 2 name>,<stage> : Start training game with the specified players and stage";
    }

    class Run: public ArgumentAction {
    public:

        Run(MugenInstant data):
            data(data){
            }

        MugenInstant data;

        void act(){
            setMugenMotif(mainMenuPath());
            Global::debug(0) << "Mugen training mode player1 '" << data.player1 << "' player2 '" << data.player2 << "' stage '" << data.stage << "'" << endl;
            Mugen::Game::startTraining(data.player1, data.player2, data.stage);
        }
    };

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        current++;
        if (current != end){
            data.enabled = parseMugenInstant(*current, &data.player1, &data.player2, &data.stage);
            data.kind = MugenInstant::Training;
            actions.push_back(Util::ReferenceCount<ArgumentAction>(new Run(data)));
        } else {
            Global::debug(0) << "Expected an argument. Example: mugen:training kfm,ken,falls" << endl;
        }
        return current;
    }
};

static void splitString(const string & subject, char split, string & left, string & right){
    size_t find = subject.find(split);
    if (find != string::npos){
        left = subject.substr(0, find);
        right = subject.substr(find + 1);
    }
}

class MugenScriptArgument: public Argument {
public:
    MugenInstant data;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("mugen:script");
        return out;
    }

    string description() const {
        return " <player 1 name>:<player 1 script>,<player 2 name>:<player 2 script>,<stage> : Start a scripted mugen game where each player reads its input from the specified scripts";
    }

    class Run: public ArgumentAction {
    public:

        Run(MugenInstant data):
            data(data){
            }

        MugenInstant data;

        void act(){
            setMugenMotif(mainMenuPath());
            Global::debug(0) << "Mugen scripted mode player1 '" << data.player1 << "' with script '" << data.player1Script << "' player2 '" << data.player2 << "' with script '" << data.player2Script << "' stage '" << data.stage << "'" << endl;
            Mugen::Game::startScript(data.player1, data.player1Script, data.player2, data.player2Script, data.stage);
        }
    };
                
    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        current++;
        if (current != end){
            data.enabled = parseMugenInstant(*current, &data.player1, &data.player2, &data.stage);
            string player, script;
            splitString(data.player1, ':', player, script);
            data.player1 = player;
            data.player1Script = script;

            splitString(data.player2, ':', player, script);
            data.player2 = player;
            data.player2Script = script;

            data.kind = MugenInstant::Script;
        } else {
            Global::debug(0) << "Expected an argument. Example: mugen:script kfm:kfm-script.txt,ken:ken-script.txt,falls" << endl;
        }

        return current;
    }
};

class MugenWatchArgument: public Argument {
public:
    MugenInstant data;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("mugen:watch");
        return out;
    }

    string description() const {
        return " <player 1 name>,<player 2 name>,<stage> : Start watch game with the specified players and stage";
    }

    class Run: public ArgumentAction {
    public:

        Run(MugenInstant data):
            data(data){
            }

        MugenInstant data;

        void act(){
            setMugenMotif(mainMenuPath());
            Global::debug(0) << "Mugen watch mode player1 '" << data.player1 << "' player2 '" << data.player2 << "' stage '" << data.stage << "'" << endl;
            Mugen::Game::startWatch(data.player1, data.player2, data.stage);
        }
    };

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        current++;
        if (current != end){
            data.enabled = parseMugenInstant(*current, &data.player1, &data.player2, &data.stage);
            data.kind = MugenInstant::Watch;
            actions.push_back(Util::ReferenceCount<ArgumentAction>(new Run(data)));
        } else {
            Global::debug(0) << "Expected an argument. Example: mugen:watch kfm,ken,falls" << endl;
        }

        return current;
    }
};

class MugenArcadeArgument: public Argument {
public:
    MugenInstant data;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("mugen:arcade");
        return out;
    }

    string description() const {
        return " <player 1 name>,<player 2 name>,<stage> : Start an arcade mugen game between two players";
    }

    class Run: public ArgumentAction {
    public:

        Run(MugenInstant data):
            data(data){
            }

        MugenInstant data;

        void act(){
            setMugenMotif(mainMenuPath());
            Global::debug(0) << "Mugen arcade mode player1 '" << data.player1 << "' player2 '" << data.player2 << "' stage '" << data.stage << "'" << endl;
            Mugen::Game::startArcade(data.player1, data.player2, data.stage);
        }
    };

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        current++;
        if (current != end){
            data.enabled = parseMugenInstant(*current, &data.player1, &data.player2, &data.stage);
            data.kind = MugenInstant::Arcade;

            actions.push_back(Util::ReferenceCount<ArgumentAction>(new Run(data)));
        } else {
            Global::debug(0) << "Expected an argument. Example: mugen:arcade kfm,ken,falls" << endl;
        }

        return current;
    }
};

class NetworkServerArgument: public Argument {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("server");
        out.push_back("network-server");
        return out;
    }

    string description() const {
        return " : Go straight to the network server";
    }

    class Run: public ArgumentAction {
    public:
        void act(){
#ifdef HAVE_NETWORKING
            Network::networkServer();
#endif
        }
    };

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        actions.push_back(Util::ReferenceCount<ArgumentAction>(new Run()));
        return current;
    }

};

struct NetworkJoin{
    NetworkJoin():
        enabled(false){
        }

    bool enabled;
    string name;
    string host;
    string port;
}; 

class NetworkJoinArgument: public Argument {
public:
    NetworkJoin data;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("network-join");
        return out;
    }

    string description() const {
        return " [<name>,<server ip>,<port>]: Join a network game directly. If not given, ip and port will be read from the configuration file if one exists otherwise they default to 127.0.0.1:7887. The name will be randomly generated if not given. Do not put spaces between the commas in the optional arguments.";
    }

    vector<string> split(string input, char splitter){
        size_t find = input.find(splitter);
        vector<string> out;
        while (find != string::npos){
            out.push_back(input.substr(0, find));
            input.erase(0, find + 1);
            find = input.find(splitter);
        }
        out.push_back(input);
        return out;
    }

    void parseNetworkJoin(const string & input, string & port, string & host, string & name){
        vector<string> args = split(input, ',');
        if (args.size() > 2){
            port = args[2];
        }
        if (args.size() > 1){
            host = args[1];
        }
        if (args.size() > 0){
            name = args[0];
        }
    }

    class Run: public ArgumentAction {
    public:
        Run(NetworkJoin join):
            join(join){
            }

        NetworkJoin join;

        void act(){
#ifdef HAVE_NETWORKING
            string port = join.port;
            string host = join.host;
            string name = join.name;
            if (port == ""){
                /* FIXME: replace 7887 with a constant */
                port = Configuration::getRootConfiguration()->getProperty(Network::propertyLastClientPort, "7887");
            }
            if (host == ""){
                host = Configuration::getRootConfiguration()->getProperty(Network::propertyLastClientHost, "127.0.0.1");
            }
            if (name == ""){
                name = Configuration::getRootConfiguration()->getProperty(Network::propertyLastClientName, "player");
            }
            Global::debug(1) << "Client " << name << " " << host << " " << port << endl;
            try{
                Network::runClient(name, host, port);
            } catch (const Network::NetworkException & fail){
                Global::debug(0) << "Error running the network client: " << fail.getMessage() << endl;
            }
#endif
        }
    };

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        data.enabled = true;
        string port;
        string host;
        string name;
        current++;
        if (current != end){
            parseNetworkJoin(*current, port, host, name);
            data.port = port;
            data.host = host;
            data.name = name;
        }

        actions.push_back(Util::ReferenceCount<ArgumentAction>(new Run(data)));
        return current;
    }
};

static const char * closestMatch(const string & s1, vector<const char *> args){
    const char * good = NULL;
    int minimum = -1;
    for (vector<const char *>::iterator it = args.begin(); it != args.end(); it++){
        const char * compare = *it;
        if (strlen(compare) > 2){
            int distance = Util::levenshtein(s1.c_str(), compare);
            if (distance != -1 && (minimum == -1 || distance < minimum)){
                minimum = distance;
                good = compare;
            }
        }
    }

    return good;
}

/*
static bool isArg(const string & s1, const char * s2[], int num){
    for (int i = 0; i < num; i++){
        if (strcasecmp(s1.c_str(), s2[i]) == 0){
            return true;
        }
    }

    return false;
}
*/

/* {"a", "b", "c"}, 3, ',' => "a, b, c"
 */
/*
static const char * all(const char * args[], const int num, const char separate = ','){
    static char buffer[1<<10];
    strcpy(buffer, "");
    for (int i = 0; i < num; i++){
        char fuz[10];
        sprintf(fuz, "%c ", separate);
        strcat(buffer, args[i]);
        if (i != num - 1){
            strcat(buffer, fuz);
        }
    }
    return buffer;
}
*/

static void showOptions(const vector<Util::ReferenceCount<Argument> > & arguments){
    Global::debug(0) << "Paintown by Jon Rafkind" << endl;
    Global::debug(0) << "Command line options" << endl;

    for (vector<Util::ReferenceCount<Argument> >::const_iterator it = arguments.begin(); it != arguments.end(); it++){
        Util::ReferenceCount<Argument> argument = *it;
        Global::debug(0) << " " << Util::join(argument->keywords(), ", ") << argument->description() << endl;
    }

    Global::debug(0) << endl;
}

/*
static void hack(){
    Filesystem::AbsolutePath fontsDirectory = Filesystem::find(Filesystem::RelativePath("fonts"));
    Global::debug(1, "hack") << "Font directory " << fontsDirectory.path() << endl;
    vector<string> ttfFonts = Util::getFiles(fontsDirectory, "*.ttf");
    Global::debug(1, "hack") << "Fonts: " << ttfFonts.size() << endl;
}
*/

/*
static void runMugenTraining(const string & player1, const string & player2, const string & stage){
    Global::debug(0) << "Mugen training mode player1 '" << player1 << "' player2 '" << player2 << "' stage '" << stage << "'" << endl;
    Mugen::Game::startTraining(player1, player2, stage);
}

static void runMugenArcade(const string & player1, const string & player2, const string & stage){
    Global::debug(0) << "Mugen arcade mode player1 '" << player1 << "' player2 '" << player2 << "' stage '" << stage << "'" << endl;
    Mugen::Game::startArcade(player1, player2, stage);
}

static void runMugenScript(const string & player1, const string & player1Script, const string & player2, const string & player2Script, const string & stage){
    Global::debug(0) << "Mugen scripted mode player1 '" << player1 << "' with script '" << player1Script << "' player2 '" << player2 << "' with script '" << player2Script << "' stage '" << stage << "'" << endl;
    Mugen::Game::startScript(player1, player1Script, player2, player2Script, stage);
}

static void runMugenWatch(const string & player1, const string & player2, const string & stage){
    Global::debug(0) << "Mugen watch mode player1 '" << player1 << "' player2 '" << player2 << "' stage '" << stage << "'" << endl;
    Mugen::Game::startWatch(player1, player2, stage);
}
*/

class MainMenuOptionFactory: public Menu::OptionFactory {
public:
    MainMenuOptionFactory(){
    }

    Paintown::OptionFactory paintownFactory;
    Platformer::OptionFactory platformerFactory;
    Mugen::OptionFactory mugenFactory;

    virtual MenuOption * getOption(const Gui::ContextBox & parent, const Token *token) const {
        MenuOption * get = paintownFactory.getOption(parent, token);
        if (get != NULL){
            return get;
        }

        get = platformerFactory.getOption(parent, token);
        if (get != NULL){
            return get;
        }

        get = mugenFactory.getOption(parent, token);
        if (get != NULL){
            return get;
        }

        return Menu::OptionFactory::getOption(parent, token);
    }
};

/* gets the mod name from system.txt */
static string systemMod(){
    TokenReader reader;
    Token * mod = reader.readTokenFromFile(Storage::instance().find(Filesystem::RelativePath("system.txt")).path().c_str());
    if (mod != NULL){
        string name;
        if (mod->match("_/default-mod", name)){
            return name;
        }
        throw LoadException(__FILE__, __LINE__, "No `default-mod' token");
    }
    throw LoadException(__FILE__, __LINE__, "Could not get system mod");
}

static int startMain(const vector<Util::ReferenceCount<ArgumentAction> > & actions, bool allow_quit){
    while (true){
        bool normal_quit = false;
        try{
            for (vector<Util::ReferenceCount<ArgumentAction> >::const_iterator it = actions.begin(); it != actions.end(); it++){
                Util::ReferenceCount<ArgumentAction> action = *it;
                action->act();
            }
            normal_quit = true;
        } catch (const Filesystem::Exception & ex){
            Global::debug(0) << "There was a problem loading the main menu. Error was:\n  " << ex.getTrace() << endl;
        } catch (const TokenException & ex){
            Global::debug(0) << "There was a problem with the token. Error was:\n  " << ex.getTrace() << endl;
            return -1;
        } catch (const LoadException & ex){
            Global::debug(0) << "There was a problem loading the main menu. Error was:\n  " << ex.getTrace() << endl;
            return -1;
        } catch (const Exception::Return & ex){
        } catch (const ShutdownException & shutdown){
            Global::debug(1) << "Forced a shutdown. Cya!" << endl;
        } catch (const MugenException & m){
            Global::debug(0) << "Mugen exception: " << m.getReason() << endl;
        } catch (const ReloadMenuException & ex){
            Global::debug(1) << "Menu Reload Requested. Restarting...." << endl;
            continue;
        } catch (const ftalleg::Exception & ex){
            Global::debug(0) << "Freetype exception caught. Error was:\n" << ex.getReason() << endl;
        } catch (const Exception::Base & base){
            // Global::debug(0) << "Freetype exception caught. Error was:\n" << ex.getReason() << endl;
            Global::debug(0) << "Base exception: " << base.getTrace() << endl;
/* android doesn't have bad_alloc for some reason */
// #ifndef ANDROID
        } catch (const std::bad_alloc & fail){
            Global::debug(0) << "Failed to allocate memory. Usage is " << System::memoryUsage() << endl;
// #endif
        } catch (...){
            Global::debug(0) << "Uncaught exception!" << endl;
        }

        if (allow_quit && normal_quit){
            break;
        } else if (normal_quit && !allow_quit){
        } else if (!normal_quit){
            break;
        }
    }
    return 0;
}

#if 0
/* dispatch to the top level function */
static int startMain2(bool just_network_server, const NetworkJoin & networkJoin, const MugenInstant & mugenInstant, bool mugen, bool allow_quit){
    while (true){
        bool normal_quit = false;
        try{
            /* fadein from white */
            if (just_network_server){
#ifdef HAVE_NETWORKING
                Network::networkServer();
#endif
            } else if (networkJoin.enabled){
#ifdef HAVE_NETWORKING
                string port = networkJoin.port;
                string host = networkJoin.host;
                string name = networkJoin.name;
                if (port == ""){
                    /* FIXME: replace 7887 with a constant */
                    port = Configuration::getRootConfiguration()->getProperty(Network::propertyLastClientPort, "7887");
                }
                if (host == ""){
                    host = Configuration::getRootConfiguration()->getProperty(Network::propertyLastClientHost, "127.0.0.1");
                }
                if (name == ""){
                    name = Configuration::getRootConfiguration()->getProperty(Network::propertyLastClientName, "player");
                }
                Global::debug(1) << "Client " << name << " " << host << " " << port << endl;
                try{
                    Network::runClient(name, host, port);
                } catch (const Network::NetworkException & fail){
                    Global::debug(0) << "Error running the network client: " << fail.getMessage() << endl;
                }
#endif
            } else if (mugen){
                setMugenMotif(mainMenuPath());
                Mugen::run();
            } else if (mugenInstant.enabled && mugenInstant.kind == MugenInstant::Script){
                setMugenMotif(mainMenuPath());
                runMugenScript(mugenInstant.player1, mugenInstant.player1Script, mugenInstant.player2, mugenInstant.player2Script, mugenInstant.stage);
            } else if (mugenInstant.enabled && mugenInstant.kind == MugenInstant::Arcade){
                setMugenMotif(mainMenuPath());
                runMugenArcade(mugenInstant.player1, mugenInstant.player2, mugenInstant.stage);
            } else if (mugenInstant.enabled && mugenInstant.kind == MugenInstant::Training){
                setMugenMotif(mainMenuPath());
                runMugenTraining(mugenInstant.player1, mugenInstant.player2, mugenInstant.stage);
            } else if (mugenInstant.enabled && mugenInstant.kind == MugenInstant::Watch){
                setMugenMotif(mainMenuPath());
                runMugenWatch(mugenInstant.player1, mugenInstant.player2, mugenInstant.stage);
            } else {
                /* Start the intro for the current mod before the game starts */
                Paintown::Mod::getCurrentMod()->playIntro();

                MainMenuOptionFactory factory;
                Menu::Menu game(mainMenuPath(), factory);
                game.run(Menu::Context());
            }
            normal_quit = true;
        } catch (const Filesystem::Exception & ex){
            Global::debug(0) << "There was a problem loading the main menu. Error was:\n  " << ex.getTrace() << endl;
        } catch (const TokenException & ex){
            Global::debug(0) << "There was a problem with the token. Error was:\n  " << ex.getTrace() << endl;
            return -1;
        } catch (const LoadException & ex){
            Global::debug(0) << "There was a problem loading the main menu. Error was:\n  " << ex.getTrace() << endl;
            return -1;
        } catch (const Exception::Return & ex){
        } catch (const ShutdownException & shutdown){
            Global::debug(1) << "Forced a shutdown. Cya!" << endl;
        } catch (const MugenException & m){
            Global::debug(0) << "Mugen exception: " << m.getReason() << endl;
        } catch (const ReloadMenuException & ex){
            Global::debug(1) << "Menu Reload Requested. Restarting...." << endl;
            continue;
        } catch (const ftalleg::Exception & ex){
            Global::debug(0) << "Freetype exception caught. Error was:\n" << ex.getReason() << endl;
        } catch (const Exception::Base & base){
            // Global::debug(0) << "Freetype exception caught. Error was:\n" << ex.getReason() << endl;
            Global::debug(0) << "Base exception: " << base.getTrace() << endl;
/* android doesn't have bad_alloc for some reason */
// #ifndef ANDROID
        } catch (const std::bad_alloc & fail){
            Global::debug(0) << "Failed to allocate memory. Usage is " << System::memoryUsage() << endl;
// #endif
        } catch (...){
            Global::debug(0) << "Uncaught exception!" << endl;
        }

        if (allow_quit && normal_quit){
            break;
        } else if (normal_quit && !allow_quit){
        } else if (!normal_quit){
            break;
        }
    }

    return 0;
}
#endif

class DefaultGame: public ArgumentAction {
public:
    void act(){
        Paintown::Mod::getCurrentMod()->playIntro();
        MainMenuOptionFactory factory;
        Menu::Menu game(mainMenuPath(), factory);
        game.run(Menu::Context());
    }
};

/* 1. parse arguments
 * 2. initialize environment
 * 3. run main dispatcher
 * 4. quit
 */
int paintown_main(int argc, char ** argv){
    /* -1 means use whatever is in the configuration */
    int gfx = -1;

    bool music_on = true;
    bool joystick_on = true;
    // bool mugen = false;
    bool just_network_server = false;
    bool allow_quit = true;
    Collector janitor;
    // NetworkJoin networkJoin;
    // MugenInstant mugenInstant;

    System::startMemoryUsage();

    Global::setDebug(0);
    vector<const char *> all_args;

#ifdef PS3
    /* find the directory that contains the binary and set the data path
     * to that directory + our data path
     */
    if (argc > 0){
        Path::AbsolutePath self(argv[0]);
        Util::setDataPath(self.getDirectory().join(Path::RelativePath(Util::getDataPath2().path())).path());
        /* set the home directory to wherever we started up from */
        setenv("HOME", argv[0], 1);
    }
#endif

    vector<string> stringArgs;
    for (int q = 1; q < argc; q++){
        stringArgs.push_back(argv[q]);
    }

    vector<Util::ReferenceCount<Argument> > arguments;
    arguments.push_back(Util::ReferenceCount<Argument>(new WindowedArgument(&gfx))); /* done */
    arguments.push_back(Util::ReferenceCount<Argument>(new DataPathArgument())); /* done */
    arguments.push_back(Util::ReferenceCount<Argument>(new MusicArgument(&music_on))); /* done */
    arguments.push_back(Util::ReferenceCount<Argument>(new DebugArgument())); /* done */
    arguments.push_back(Util::ReferenceCount<Argument>(new RateLimitArgument())); /* done */
    arguments.push_back(Util::ReferenceCount<Argument>(new JoystickArgument())); /* done */
    arguments.push_back(Util::ReferenceCount<Argument>(new MugenArgument())); /* done */
    arguments.push_back(Util::ReferenceCount<Argument>(new MugenTrainingArgument())); /* done */
    arguments.push_back(Util::ReferenceCount<Argument>(new MugenScriptArgument())); /* done */
    arguments.push_back(Util::ReferenceCount<Argument>(new MugenWatchArgument())); /* done */
    arguments.push_back(Util::ReferenceCount<Argument>(new MugenArcadeArgument())); /* done */
    arguments.push_back(Util::ReferenceCount<Argument>(new DisableQuitArgument(&allow_quit))); /* done */
#ifdef HAVE_NETWORKING
    arguments.push_back(Util::ReferenceCount<Argument>(new NetworkServerArgument()));
    arguments.push_back(Util::ReferenceCount<Argument>(new NetworkJoinArgument()));
#endif
    
    vector<Util::ReferenceCount<ArgumentAction> > actions;

    for (vector<string>::iterator it = stringArgs.begin(); it != stringArgs.end(); it++){
        for (vector<Util::ReferenceCount<Argument> >::iterator arg = arguments.begin(); arg != arguments.end(); arg++){
            Util::ReferenceCount<Argument> argument = *arg;
            if (argument->isArg(*it)){
                it = argument->parse(it, stringArgs.end(), actions);

                /* Only parse one argument */
                break;
            }
        }
    }

    showOptions(arguments);

    Global::debug(0) << "Debug level: " << Global::getDebug() << endl;

    /* time how long init takes */
    TimeDifference diff;
    diff.startTime();
    /* init initializes practically everything including
     *  graphics
     *  sound
     *  input
     *  network
     *  configuration
     *  ...
     */
    if (! Global::init(gfx)){
        Global::debug(0) << "Could not initialize system" << endl;
        return -1;
    } else {
        // Configuration::setFullscreen((gfx == Global::FULLSCREEN ? true : false));
    }
    diff.endTime();
    Global::debug(0) << diff.printTime("Init took") << endl;

    // Graphics::Bitmap screen(Graphics::getScreenBuffer());
    Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, Graphics::getScreenBuffer());

    /* set the game mod. check in this order
     * 1. whatever is in the configuration under (current-game ...)
     * 2. whatever is in data/system.txt
     * 3. try to load 'paintown'
     * 4. die
     */
    try{
        Paintown::Mod::loadPaintownMod(Configuration::getCurrentGame());
    } catch (...){
        if (Configuration::getCurrentGame() != ""){
            Global::debug(0) << "Could not load mod " << Configuration::getCurrentGame() << std::endl;
        }
        try{
            /* load the mod listed in data/system.txt */
            Paintown::Mod::loadPaintownMod(systemMod());
        } catch (...){
            try{
                /* failed to find anything, pray that data/paintown is there */
                Paintown::Mod::loadDefaultMod();
            } catch (...){
                Global::debug(0) << "Could not find the paintown default mod" << std::endl;
                /* Maybe just set up some ultra default thing? */
                return 0;
            }
        }
    }

    InputManager input;
    Music music(music_on);

    /* Testing hack
    mugenInstant.enabled = true;
    mugenInstant.kind = MugenInstant::Watch;
    mugenInstant.player1 = "kfm";
    mugenInstant.player2 = "kfm";
    mugenInstant.stage = "kfm";
    */

    if (actions.size() == 0){
        actions.push_back(Util::ReferenceCount<ArgumentAction>(new DefaultGame()));
    }

    Util::Parameter<Util::ReferenceCount<Menu::FontInfo> > defaultFont(Menu::menuFontParameter, Util::ReferenceCount<Menu::FontInfo>(new Menu::RelativeFontInfo(Global::DEFAULT_FONT, Configuration::getMenuFontWidth(), Configuration::getMenuFontHeight())));

    startMain(actions, allow_quit);

    Configuration::saveConfiguration();

    Global::debug(0) << "Bye!" << endl;

    return 0;
}
