#include "argument.h"
#include "config.h"
#include "util.h"
#include "menu.h"
#include "util/debug.h"
#include "game.h"

using std::vector;
using std::string;
using std::endl;

namespace Mugen{

static void splitString(const string & subject, char split, string & left, string & right){
    size_t find = subject.find(split);
    if (find != string::npos){
        left = subject.substr(0, find);
        right = subject.substr(find + 1);
    }
}

/*
static void setMugenMotif(){
    / * FIXME: parse the motif properly * /
    std::string motif;
    try {
        *Mugen::Configuration::get("motif") >> motif;
    } catch (const std::ios_base::failure & ex){
        motif.clear();
    }
    if (!motif.empty()){
        Mugen::Data::getInstance().setMotif(Filesystem::AbsolutePath(motif));
    } else {
        / * FIXME: search for a system.def file * /
        Mugen::Data::getInstance().setMotif(Storage::instance().find(Filesystem::RelativePath("mugen/data/system.def")));
        / *
        TokenReader reader;
        Token * head = reader.readToken(path.path());
        const Token * motif = head->findToken("menu/option/mugen/motif");
        if (motif != NULL){
            string path;
            motif->view() >> path;
            Mugen::Data::getInstance().setMotif(Filesystem::RelativePath(path));
        }
        * /
    }
}
*/

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
            Util::loadMotif();
            Mugen::run();
        }
    };

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        actions.push_back(::Util::ReferenceCount<ArgumentAction>(new Run()));
        return current;
    }
};

static bool parseMugenInstant(string input, string * player1, string * player2, string * stage){
    size_t comma = input.find(',');
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

/* TODO: refactor this (and the above method) */
static bool parseMugenInstant(string input, string * player1, string * player2, string * player3, string * player4, string * stage){
    size_t comma = input.find(',');
    if (comma == string::npos){
        Global::debug(0) << "Expected five arguments separated by a comma, only 1 was given: " << input << endl;
        return false;
    }
    *player1 = input.substr(0, comma);
    input.erase(0, comma + 1);
    comma = input.find(',');

    if (comma == string::npos){
        Global::debug(0) << "Expected five arguments separated by a comma, only 2 were given: " << input << endl;
        return false;
    }

    *player2 = input.substr(0, comma);
    input.erase(0, comma + 1);

    comma = input.find(',');

    if (comma == string::npos){
        Global::debug(0) << "Expected five arguments separated by a comma, only 2 were given: " << input << endl;
        return false;
    }

    *player3 = input.substr(0, comma);
    input.erase(0, comma + 1);

    comma = input.find(',');

    if (comma == string::npos){
        Global::debug(0) << "Expected five arguments separated by a comma, only 2 were given: " << input << endl;
        return false;
    }

    *player4 = input.substr(0, comma);
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
        Script,
        Team
    };

    MugenInstant():
        enabled(false),
        kind(None){
        }

    bool enabled;
    string player1;
    string player2;
    string player3;
    string player4;
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
            Util::loadMotif();
            Global::debug(0) << "Mugen training mode player1 '" << data.player1 << "' player2 '" << data.player2 << "' stage '" << data.stage << "'" << endl;
            Mugen::Game::startTraining(data.player1, data.player2, data.stage);
        }
    };

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        current++;
        if (current != end){
            data.enabled = parseMugenInstant(*current, &data.player1, &data.player2, &data.stage);
            data.kind = MugenInstant::Training;
            actions.push_back(::Util::ReferenceCount<ArgumentAction>(new Run(data)));
        } else {
            Global::debug(0) << "Expected an argument. Example: mugen:training kfm,ken,falls" << endl;
        }
        return current;
    }
};

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
            Util::loadMotif();
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
            actions.push_back(::Util::ReferenceCount<ArgumentAction>(new Run(data)));
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
            Util::loadMotif();
            Global::debug(0) << "Mugen watch mode player1 '" << data.player1 << "' player2 '" << data.player2 << "' stage '" << data.stage << "'" << endl;
            Mugen::Game::startWatch(data.player1, data.player2, data.stage);
        }
    };

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        current++;
        if (current != end){
            data.enabled = parseMugenInstant(*current, &data.player1, &data.player2, &data.stage);
            data.kind = MugenInstant::Watch;
            actions.push_back(::Util::ReferenceCount<ArgumentAction>(new Run(data)));
        } else {
            Global::debug(0) << "Expected an argument. Example: mugen:watch kfm,ken,falls" << endl;
        }

        return current;
    }
};

class MugenTeamArgument: public Argument {
public:
    MugenInstant data;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("mugen:team");
        return out;
    }

    string description() const {
        return " <player 1 name>,<player 2 name>,<player 3 name>,<player 4 name>,<stage> : Start watch game with the specified players and stage";
    }

    class Run: public ArgumentAction {
    public:

        Run(MugenInstant data):
            data(data){
            }

        MugenInstant data;

        void act(){
            Util::loadMotif();
            Global::debug(0) << "Mugen watch mode player1 '" << data.player1 << "' player2 '" << data.player2 << "' player3 '" << data.player3 << "' player 4 '" << data.player4 << "' stage '" << data.stage << "'" << endl;
            Mugen::Game::startTeam(data.player1, data.player2, data.player3, data.player4, data.stage);
        }
    };

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        current++;
        if (current != end){
            data.enabled = parseMugenInstant(*current, &data.player1, &data.player2, &data.player3, &data.player4, &data.stage);
            data.kind = MugenInstant::Team;
            actions.push_back(::Util::ReferenceCount<ArgumentAction>(new Run(data)));
        } else {
            Global::debug(0) << "Expected an argument. Example: mugen:team kfm,ken,kfm,kfm,falls" << endl;
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
            Util::loadMotif();
            Global::debug(0) << "Mugen arcade mode player1 '" << data.player1 << "' player2 '" << data.player2 << "' stage '" << data.stage << "'" << endl;
            Mugen::Game::startArcade(data.player1, data.player2, data.stage);
        }
    };

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        current++;
        if (current != end){
            data.enabled = parseMugenInstant(*current, &data.player1, &data.player2, &data.stage);
            data.kind = MugenInstant::Arcade;

            actions.push_back(::Util::ReferenceCount<ArgumentAction>(new Run(data)));
        } else {
            Global::debug(0) << "Expected an argument. Example: mugen:arcade kfm,ken,falls" << endl;
        }

        return current;
    }
};

class MugenServerArgument: public Argument {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("mugen:server");
        return out;
    }

    string description() const {
        return " [port] : Start a server on port 8473";
    }

    class Run: public ArgumentAction {
    public:
        Run(int port):
        port(port){
        }
        int port;
        void act(){
            Game::startNetworkVersus("kfm", "kfm", "kfm", true, "localhost", port);
        }
    };

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        int port = 8473;
        current++;
        if (current != end){
            port = atoi((*current).c_str());
        }
        actions.push_back(::Util::ReferenceCount<ArgumentAction>(new Run(port)));
        return current;
    }
};

class MugenClientArgument: public Argument {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("mugen:client");
        return out;
    }

    string description() const {
        return " [host] [port] : Join a server on port (defaults to 8473)";
    }

    class Run: public ArgumentAction {
    public:
        Run(const string & host, int port):
        host(host),
        port(port){
        }

        string host;
        int port;

        void act(){
            Game::startNetworkVersus("kfm", "kfm", "kfm", false, host, port);
        }
    };

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        string host = "127.0.0.1";
        int port = 8473;
        current++;
        if (current != end){
            host = *current;
            current++;
            if (current != end){
                port = atoi((*current).c_str());
            }
        }
        actions.push_back(::Util::ReferenceCount<ArgumentAction>(new Run(host, port)));
        return current;
    }
};

std::vector< ::Util::ReferenceCount<Argument> > arguments(){
    vector< ::Util::ReferenceCount<Argument> > all;
    all.push_back(::Util::ReferenceCount<Argument>(new MugenArgument()));
    all.push_back(::Util::ReferenceCount<Argument>(new MugenTrainingArgument()));
    all.push_back(::Util::ReferenceCount<Argument>(new MugenScriptArgument()));
    all.push_back(::Util::ReferenceCount<Argument>(new MugenWatchArgument()));
    all.push_back(::Util::ReferenceCount<Argument>(new MugenTeamArgument()));
    all.push_back(::Util::ReferenceCount<Argument>(new MugenArcadeArgument()));

    all.push_back(::Util::ReferenceCount<Argument>(new MugenServerArgument()));
    all.push_back(::Util::ReferenceCount<Argument>(new MugenClientArgument()));
    return all;
}

}
