#include <string.h>
#include <string>
#include <sstream>
#include <vector>

#ifdef PS3
/* For network debugging */
#include <net/net.h>
#endif

/* Default debug level. On most pc's it should be 0 to get normal warnings/info.
 * On consoles set it to -1 to disable all text output.
 * If you are debugging then higher values like 1 or 2 are ok.
 */
#if defined(XENON) || defined(DEVKITPRO)
//static const int DEFAULT_DEBUG = -1;
static const int DEFAULT_DEBUG = 3;
#else
static const int DEFAULT_DEBUG = 0;
#endif

#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/graphics/texture-cache.h"

#include "factory/collector.h"
#include "r-tech1/network/network.h"
#include "r-tech1/token_exception.h"
#include "r-tech1/system.h"
#include "r-tech1/sound/music.h"
#include "r-tech1/menu/menu.h"
#include "r-tech1/menu/menu-exception.h"
#include "r-tech1/menu/optionfactory.h"
#include "r-tech1/input/input-manager.h"
#include "paintown-engine/game/options.h"
#include "paintown-engine/game/mod.h"
#include "paintown-engine/network/client.h"
#include "paintown-engine/network/server.h"
#include "r-tech1/exceptions/shutdown_exception.h"
#include "r-tech1/exceptions/exception.h"
#include "r-tech1/exceptions/load_exception.h"
#include "r-tech1/timedifference.h"
#include "r-tech1/funcs.h"
#include "r-tech1/ftalleg.h"
#include "r-tech1/file-system.h"
#include "r-tech1/tokenreader.h"
#include "r-tech1/token.h"
#include "r-tech1/parameter.h"
#include "r-tech1/version.h"
#include "r-tech1/debug.h"
#include "r-tech1/configuration.h"
#include "r-tech1/init.h"
#include "r-tech1/main.h"
#include "r-tech1/argument.h"
#include "r-tech1/context.h"

#include "paintown-engine/game/argument.h"

#include "mugen/options.h"
#include "mugen/argument.h"

#include <iostream>

using std::vector;
using std::endl;
using std::string;
using std::istringstream;

#if !defined(WINDOWS) || !defined(_WIN32)
Filesystem::AbsolutePath Filesystem::configFile(){
    std::ostringstream str;
    /* what if HOME isn't set? */
    str << getenv("HOME") << "/.paintownrc";
    return Filesystem::AbsolutePath(str.str());
}

Filesystem::AbsolutePath Filesystem::userDirectory(){
    std::ostringstream str;
    char * home = getenv("HOME");
    if (home == NULL){
        str << "/tmp/paintown";
    } else {
        str << home << "/.paintown/";
    }
    return Filesystem::AbsolutePath(str.str());
}
#endif

static void showOptions(const vector<Util::ReferenceCount<Argument::Parameter> > & arguments){
    Global::debug(0) << "Paintown by Jon Rafkind" << endl;
    Global::debug(0) << "Command line options" << endl;

    for (vector<Util::ReferenceCount<Argument::Parameter> >::const_iterator it = arguments.begin(); it != arguments.end(); it++){
        Util::ReferenceCount<Argument::Parameter> argument = *it;
        Global::debug(0) << " " << Util::join(argument->keywords(), ", ") << argument->description() << endl;
    }

    Global::debug(0) << endl;
}

class WindowedArgument: public Argument::Parameter {
public:
    WindowedArgument(Global::InitConditions * conditions):
    conditions(conditions){
    }

    Global::InitConditions * conditions;

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
    
    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        conditions->graphics = Global::InitConditions::Fullscreen;
        return current;
    }
};

class SoftwareRendererArgument: public Argument::Parameter {
public:
    SoftwareRendererArgument(Global::InitConditions * conditions):
    conditions(conditions){
    }

    Global::InitConditions * conditions;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("--software");
        out.push_back("software");
        return out;
    }

    string description() const {
        return " : Use a software renderer instead of opengl/hardware";
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        conditions->softwareRenderer = true;
        return current;
    }
};

class SoundArgument: public Argument::Parameter {
public:
    SoundArgument(Global::InitConditions * conditions):
    conditions(conditions){
    }

    Global::InitConditions * conditions;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("-s");
        out.push_back("nosound");
        return out;
    }

    string description() const {
        return " : Disable sound";
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        conditions->sound = false;
        return current;
    }
};

class HelpArgument: public Argument::Parameter {
public:
    HelpArgument(const vector<Util::ReferenceCount<Argument::Parameter> > & arguments):
        arguments(arguments){
        }

    const vector<Util::ReferenceCount<Argument::Parameter> > & arguments;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("help");
        out.push_back("-h");
        out.push_back("--help");
        return out;
    }
    
    string description() const {
        return " : Print the command line options and exit.";
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        showOptions(arguments);
        exit(0);
    }
};

class VersionArgument: public Argument::Parameter {
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
    
    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        Global::debug(0) << "Version " << Version::getVersionString() << endl;
        exit(0);
    }
};

class DataPathArgument: public Argument::Parameter {
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

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        current++;
        if (current != end){
            Util::setDataPath(*current);
        }
        return current;
    }
};

class DisableQuitArgument: public Argument::Parameter {
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

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        *quit = false;
        return current;
    }
};

class MusicArgument: public Argument::Parameter {
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
    
    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        *enabled = false;
        return current;
    }
};

class DebugArgument: public Argument::Parameter {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("-l");
        out.push_back("--debug");
        out.push_back("debug");
        return out;
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
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

class DebugFileArgument: public Argument::Parameter {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("debug-log");
        return out;
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        current++;
        Global::logToFile();
        return current;
    }
    
    string description() const {
        return " # : Send debug output to a file.";
    }
};

class RateLimitArgument: public Argument::Parameter {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("fps");
        out.push_back("rate-limit");
        return out;
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        Global::rateLimit = false;
        return current;
    }

    string description() const {
        return " : Don't rate limit the game to the default fps (40). This is mainly useful for benchmarking graphics capabilities.";
    }
};

class JoystickArgument: public Argument::Parameter {
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

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & refs){
        Configuration::setJoystickEnabled(false);
        return current;
    }
};

static Filesystem::AbsolutePath mainMenuPath(){
    string menu = Paintown::Mod::getCurrentMod()->getMenu();
    return Storage::instance().find(Filesystem::RelativePath(menu));
}

/* FIXME: move the network arguments to the paintown-engine directory */
class NetworkServerArgument: public Argument::Parameter {
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

    class Run: public Argument::Action {
    public:
        void act(){
#ifdef HAVE_NETWORKING
            Network::networkServer();
#endif
        }
    };

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        actions.push_back(Util::ReferenceCount<Argument::Action>(new Run()));
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

class NetworkJoinArgument: public Argument::Parameter {
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

    class Run: public Argument::Action {
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
                port = Configuration::getProperty(Network::propertyLastClientPort, "7887");
            }
            if (host == ""){
                host = Configuration::getProperty(Network::propertyLastClientHost, "127.0.0.1");
            }
            if (name == ""){
                name = Configuration::getProperty(Network::propertyLastClientName, "player");
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

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
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

        actions.push_back(Util::ReferenceCount<Argument::Action>(new Run(data)));
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
    Mugen::OptionFactory mugenFactory;

    virtual MenuOption * getOption(const Gui::ContextBox & parent, const Token *token) const {
        MenuOption * get = paintownFactory.getOption(parent, token);
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

static int startMain(const vector<Util::ReferenceCount<Argument::Action> > & actions, bool allow_quit){
    while (true){
        bool normal_quit = false;
        try{
            for (vector<Util::ReferenceCount<Argument::Action> >::const_iterator it = actions.begin(); it != actions.end(); it++){
                Util::ReferenceCount<Argument::Action> action = *it;
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
            DebugLog << "Base exception: " << base.getTrace() << endl;
/* android doesn't have bad_alloc for some reason */
// #ifndef ANDROID
#ifndef WINDOWS
        } catch (const std::bad_alloc & fail){
            Global::debug(0) << "Failed to allocate memory. Usage is " << System::memoryUsage() << endl;
// #endif
#endif
        } catch (...){
            DebugLog << "Uncaught exception!" << endl;
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

class DefaultGame: public Argument::Action {
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
        /* TODO: move this to some util class because its used here and in mugen */
        std::string mod;
        try {
            Token * all = Configuration::getProperty("paintown/mod");
            if (all != NULL){
                Global::debug(1) << "Mod is " << all->toString() << std::endl;
                string zip, mount;

                /* Its either just a path to a file or an entry in a zip file or other container.
                 * If its a zip entry then get the zip file, the mount point, and the entry path then
                 * mount the zip.
                 */
                if (all->match("mod/file", mod)){
                } else if (all->match("mod/container", zip, mount, mod)){
                    Storage::instance().addOverlay(Filesystem::AbsolutePath(zip),
                                                   Filesystem::AbsolutePath(mount));
                }
            }
        } catch (const Filesystem::Exception & ex){
            Global::debug(0) << "Could not load mugen motif because " << ex.getTrace() << std::endl;
            mod.clear();
        }

        /* Found a motif */
        if (!mod.empty()){
            Paintown::Mod::loadPaintownMod(Filesystem::AbsolutePath(mod));
            return;
        }
    } catch (const Exception::Base & fail){
        Global::debug(0) << "Could not load mod " << fail.getTrace() << std::endl;
    } catch (...){
    }
    setupSystemMod();
}

/* Sort arguments based on the first letter of the keywords */
static bool argumentSorter(const Util::ReferenceCount<Argument::Parameter> & arg1, const Util::ReferenceCount<Argument::Parameter> & arg2){
    vector<string> keywords1 = arg1->keywords();
    vector<string> keywords2 = arg2->keywords();

    if (keywords1.size() > 0 && keywords2.size() > 0){
        /* Check first letter in the first string */
        return keywords1[0][0] < keywords2[0][0];
    }
    return false;
}

static void sortArguments(vector<Util::ReferenceCount<Argument::Parameter> > & arguments){
    sort(arguments.begin(), arguments.end(), argumentSorter);
}

static void setUpTouch(const Util::ReferenceCount<DeviceInput::Touch> & touch){
    if (touch == nullptr){
        return;
    }

    int screenWidth = Graphics::Bitmap::getScreenWidth();
    int screenHeight = Graphics::Bitmap::getScreenHeight();
    int buttonSize = (int) (0.06 * sqrt(screenWidth * screenWidth + screenHeight * screenHeight));

    /* dpad
     *
     *  X
     * X X
     *  X
     */
    int center_x = buttonSize + buttonSize / 2;
    int center_y = screenHeight - buttonSize - buttonSize / 2;

    int x, y;

    x = center_x;
    y = center_y - buttonSize;

    touch->setZone(DeviceInput::Touch::Up, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);

    x = center_x;
    y = center_y + buttonSize;
    touch->setZone(DeviceInput::Touch::Down, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);

    x = center_x - buttonSize;
    y = center_y;
    touch->setZone(DeviceInput::Touch::Left, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);
    
    x = center_x + buttonSize;
    y = center_y;
    touch->setZone(DeviceInput::Touch::Right, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);

    /* X X X
     * X X X
     */
    int spacing = (int) (buttonSize * 0.1);
    center_x = screenWidth - buttonSize * 2 - spacing;
    center_y = screenHeight - buttonSize - buttonSize / 2;

    x = center_x - buttonSize - buttonSize / 2;
    y = center_y - buttonSize / 2 - spacing;
    touch->setZone(DeviceInput::Touch::Button1, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);
    
    x = center_x;
    y = center_y - buttonSize / 2 - spacing;
    touch->setZone(DeviceInput::Touch::Button2, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);
    
    x = center_x + buttonSize + buttonSize / 2;
    y = center_y - buttonSize / 2 - spacing;
    touch->setZone(DeviceInput::Touch::Button3, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);

    x = center_x - buttonSize - buttonSize / 2;
    y = center_y + buttonSize / 2 + spacing;
    touch->setZone(DeviceInput::Touch::Button4, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);
    
    x = center_x;
    y = center_y + buttonSize / 2 + spacing;
    touch->setZone(DeviceInput::Touch::Button5, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);
    
    x = center_x + buttonSize + buttonSize / 2;
    y = center_y + buttonSize / 2 + spacing;
    touch->setZone(DeviceInput::Touch::Button6, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);

    x = screenWidth - buttonSize / 2 - spacing;
    y = buttonSize;
    touch->setZone(DeviceInput::Touch::Quit, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);
}

static bool hasData(){
    try{
        Global::debug(0) << "Checking for installed data" << std::endl;
        Filesystem::AbsolutePath path = Storage::instance().find(Filesystem::RelativePath("installed"));
        Global::debug(0) << "Found installed data" << std::endl;
        return true;
    } catch (Filesystem::NotFound & fail){
        Global::debug(0) << "data not installed yet" << std::endl;
    }
    return false;
    /*
    Filesystem::AbsolutePath installed = Filesystem::AbsolutePath("/sdcard/paintown/data/installed");
    return Storage::instance().systemExists(installed);
    */
}

/* 1. parse arguments
 * 2. initialize environment
 * 3. run main dispatcher
 * 4. quit
 */
int rtech_main(int argc, char ** argv){
    Util::setDataPath(DATA_PATH);

    /* -1 means use whatever is in the configuration */
    Global::InitConditions conditions;

#ifdef ANDROID
    while (!hasData()){
        sleep(1);
    }
#endif

    std::shared_ptr<Util::Context> mainContext = std::make_shared<Util::Context>();

    // scope for collector
    {


        Util::AutoCancel cancel(mainContext->autoCancel());

    bool music_on = true;
    // bool joystick_on = true;
    // bool mugen = false;
    // bool just_network_server = false;
    bool allow_quit = true;
    Collector janitor;
    // NetworkJoin networkJoin;
    // MugenInstant mugenInstant;

    Version::setVersion(3, 6, 1);

#ifndef WINDOWS
    System::startMemoryUsage();
#endif

    Global::setDebug(DEFAULT_DEBUG);
    Global::setDefaultDebugContext("paintown");
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

#ifdef ANDROID
    stringArgs.push_back("mugen");
#endif

    vector<Util::ReferenceCount<Argument::Parameter> > arguments;
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new WindowedArgument(&conditions)));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new SoftwareRendererArgument(&conditions)));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new DataPathArgument()));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new SoundArgument(&conditions)));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new MusicArgument(&music_on)));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new DebugArgument()));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new DebugFileArgument()));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new RateLimitArgument()));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new JoystickArgument()));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new VersionArgument()));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new DisableQuitArgument(&allow_quit)));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new HelpArgument(arguments)));

    appendVector(arguments, Paintown::arguments());
    appendVector(arguments, Mugen::arguments());
    /*
    appendVector(arguments, Platformer::arguments());
    appendVector(arguments, Asteroids::arguments());
    */

#ifdef HAVE_NETWORKING
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new NetworkServerArgument()));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new NetworkJoinArgument()));
#endif

    sortArguments(arguments);
    
    vector<Util::ReferenceCount<Argument::Action> > actions;

    /* Sort of a hack but if we are already at the end of the argument list (because some
     * argument already reached the end) then we don't increase the argument iterator
     */
    for (vector<string>::iterator it = stringArgs.begin(); it != stringArgs.end(); (it != stringArgs.end()) ? it++ : it){
        for (vector<Util::ReferenceCount<Argument::Parameter> >::iterator arg = arguments.begin(); arg != arguments.end(); arg++){
            Util::ReferenceCount<Argument::Parameter> argument = *arg;
            if (argument->isArg(*it)){
                it = argument->parse(it, stringArgs.end(), actions);

                /* Only parse one argument */
                break;
            }
        }
    }

    Global::debug(0) << "Give 'help' to see all the command line options." << endl;
    // showOptions(arguments);

    Global::debug(0) << "Debug level: " << Global::getDebug() << endl;
    
    Global::debug(0) << "Paintown version " << Version::getVersionString() << endl;

    /* time how long init takes */
    /* FIXME: we count time using SDL_GetTicks or whatever in allegro but it will
     * return an uninitialized value before init so we get messed up time
     * when we try to count it.
     */
    // TimeDifference diff;
    // diff.startTime();
    /* init initializes practically everything including
     *  graphics
     *  sound
     *  input
     *  network
     *  configuration
     *  ...
     */
    if (! Global::init(conditions)){
        Global::debug(0) << "Could not initialize system" << endl;
        return -1;
    } else {
        // Configuration::setFullscreen((gfx == Global::FULLSCREEN ? true : false));
    }

    if (! Global::dataCheck()){
        Global::debug(0) << "Could not find data" << std::endl;
        return -1;
    }

    // diff.endTime();
    // Global::debug(0) << diff.printTime("Init took") << endl;

    // Graphics::Bitmap screen(Graphics::getScreenBuffer());
    Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, Graphics::getScreenBuffer());

    try{
        setupPaintownMod();
    } catch (const LoadException & fail){
        Global::debug(0) << "Could not load a paintown mod. Aborting. " << fail.getTrace() << std::endl;
        return -1;
    }
    
    InputManager input;
    Music music(music_on);

    setUpTouch(input.getTouch());

    /* If there are no actions then start the Paintown menu */
    if (actions.size() == 0){
        actions.push_back(Util::ReferenceCount<Argument::Action>(new DefaultGame()));
    }

    Util::Parameter<Util::ReferenceCount<Graphics::ShaderManager> > defaultShaderManager(Graphics::shaderManager, Util::ReferenceCount<Graphics::ShaderManager>(new Graphics::ShaderManager()));

    Util::Parameter<Util::ReferenceCount<Path::RelativePath> > defaultFont(Font::defaultFont, Util::ReferenceCount<Path::RelativePath>(new Path::RelativePath("fonts/LiberationSans-Regular.ttf")));

    Util::Parameter<Util::ReferenceCount<Menu::FontInfo> > defaultMenuFont(Menu::menuFontParameter, Util::ReferenceCount<Menu::FontInfo>(new Menu::RelativeFontInfo(*defaultFont.current(), Configuration::getMenuFontWidth(), Configuration::getMenuFontHeight())));

    Graphics::LocalTextureCache defaultTextureCache;

    startMain(actions, allow_quit);

    Configuration::saveConfiguration();

    }

    System::shutdown();
    DebugLog << "Bye!" << endl;
    Global::closeLog();

    return 0;
}
