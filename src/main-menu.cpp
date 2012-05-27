#include <string.h>
#include <string>
#include <sstream>
#include <vector>

#ifdef PS3
/* For network debugging */
#include <net/net.h>
#endif

#include "util/bitmap.h"

#include "factory/collector.h"
#include "util/network/network.h"
#include "util/token_exception.h"
#include "util/system.h"
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

#include "mugen/options.h"
#include "mugen/argument.h"

#include "platformer/argument.h"
#include "asteroids/argument.h"
#include "missile-defend/argument.h"

#include <iostream>

using std::vector;
using std::endl;
using std::string;
using std::istringstream;

static void showOptions(const vector<Util::ReferenceCount<Argument> > & arguments){
    Global::debug(0) << "Paintown by Jon Rafkind" << endl;
    Global::debug(0) << "Command line options" << endl;

    for (vector<Util::ReferenceCount<Argument> >::const_iterator it = arguments.begin(); it != arguments.end(); it++){
        Util::ReferenceCount<Argument> argument = *it;
        Global::debug(0) << " " << Util::join(argument->keywords(), ", ") << argument->description() << endl;
    }

    Global::debug(0) << endl;
}

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

class HelpArgument: public Argument {
public:
    HelpArgument(const vector<Util::ReferenceCount<Argument> > & arguments):
        arguments(arguments){
        }

    const vector<Util::ReferenceCount<Argument> > & arguments;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("help");
        out.push_back("--help");
        return out;
    }
    
    string description() const {
        return " : Print the command line options and exit.";
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        showOptions(arguments);
        exit(0);
    }
};

class VersionArgument: public Argument {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("version");
        out.push_back("--version");
        out.push_back("-v");
        return out;
    }
    
    string description() const {
        return " : Print the version of Paintown and exit.";
    }
    
    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        Global::debug(0) << "Version " << Global::getVersionString() << endl;
        exit(0);
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

/* FIXME: move the network arguments to the paintown-engine directory */
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
static Filesystem::AbsolutePath systemMod(){
    TokenReader reader;
    Token * mod = reader.readTokenFromFile(*Storage::instance().open(Storage::instance().find(Filesystem::RelativePath("system.txt"))));
    if (mod != NULL){
        string name;
        if (mod->match("_/default-mod", name)){
            return Storage::instance().find(Filesystem::RelativePath(name));
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

class DefaultGame: public ArgumentAction {
public:
    void act(){
        Paintown::Mod::getCurrentMod()->playIntro();
        MainMenuOptionFactory factory;
        Menu::Menu game(mainMenuPath(), factory);
        game.run(Menu::Context());
    }
};

template <class X>
static void appendVector(vector<X> & to, const vector<X> & from){
    to.insert(to.end(), from.begin(), from.end());
}

static void setupSystemMod(){
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
            throw LoadException(__FILE__, __LINE__, "Could not load any mods");
        }
    }

}

static void setupPaintownMod(){
    /* set the game mod. check in this order
     * 1. whatever is in the configuration under (current-game ...)
     * 2. whatever is in data/system.txt
     * 3. try to load 'paintown'
     * 4. die
     */
    try{
        Filesystem::AbsolutePath currentMod(Configuration::getRootConfiguration()->getNamespace("paintown")->getProperty("mod", ""));
        if (currentMod.path() != ""){
            Paintown::Mod::loadPaintownMod(currentMod);
        }
    } catch (const Exception::Base & fail){
        Global::debug(0) << "Could not load mod " << fail.getTrace() << std::endl;
    } catch (...){
    }
    setupSystemMod();
}

/* 1. parse arguments
 * 2. initialize environment
 * 3. run main dispatcher
 * 4. quit
 */
int paintown_main(int argc, char ** argv){
    /* -1 means use whatever is in the configuration */
    int gfx = -1;

    bool music_on = true;
    // bool joystick_on = true;
    // bool mugen = false;
    // bool just_network_server = false;
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
        setenv("HOME", self.getDirectory().path().c_str(), 1);
    }

    /* Do network initialization early in case we need to use network debugging */
    netInitialize();
#endif

    vector<string> stringArgs;
    for (int q = 1; q < argc; q++){
        stringArgs.push_back(argv[q]);
    }

    vector<Util::ReferenceCount<Argument> > arguments;
    arguments.push_back(Util::ReferenceCount<Argument>(new WindowedArgument(&gfx)));
    arguments.push_back(Util::ReferenceCount<Argument>(new DataPathArgument()));
    arguments.push_back(Util::ReferenceCount<Argument>(new MusicArgument(&music_on)));
    arguments.push_back(Util::ReferenceCount<Argument>(new DebugArgument()));
    arguments.push_back(Util::ReferenceCount<Argument>(new RateLimitArgument()));
    arguments.push_back(Util::ReferenceCount<Argument>(new JoystickArgument()));
    arguments.push_back(Util::ReferenceCount<Argument>(new VersionArgument()));
    arguments.push_back(Util::ReferenceCount<Argument>(new DisableQuitArgument(&allow_quit)));
    arguments.push_back(Util::ReferenceCount<Argument>(new HelpArgument(arguments)));

    appendVector(arguments, Mugen::arguments());
    appendVector(arguments, Platformer::arguments());
    appendVector(arguments, Asteroids::arguments());
    appendVector(arguments, MissileDefend::arguments());

#ifdef HAVE_NETWORKING
    arguments.push_back(Util::ReferenceCount<Argument>(new NetworkServerArgument()));
    arguments.push_back(Util::ReferenceCount<Argument>(new NetworkJoinArgument()));
#endif
    
    vector<Util::ReferenceCount<ArgumentAction> > actions;

    /* Sort of a hack but if we are already at the end of the argument list (because some
     * argument already reached the end) then we don't increase the argument iterator
     */
    for (vector<string>::iterator it = stringArgs.begin(); it != stringArgs.end(); (it != stringArgs.end()) ? it++ : it){
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

    setupPaintownMod();
    
    InputManager input;
    Music music(music_on);

    /* If there are no actions then start the Paintown menu */
    if (actions.size() == 0){
        actions.push_back(Util::ReferenceCount<ArgumentAction>(new DefaultGame()));
    }

    Util::Parameter<Util::ReferenceCount<Menu::FontInfo> > defaultFont(Menu::menuFontParameter, Util::ReferenceCount<Menu::FontInfo>(new Menu::RelativeFontInfo(Global::DEFAULT_FONT, Configuration::getMenuFontWidth(), Configuration::getMenuFontHeight())));

    startMain(actions, allow_quit);

    Configuration::saveConfiguration();

    Global::debug(0) << "Bye!" << endl;

    return 0;
}
