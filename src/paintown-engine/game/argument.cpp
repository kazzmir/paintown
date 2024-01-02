#include "argument.h"
#include "game.h"
#include "mod.h"
#include "r-tech1/regex.h"
#include "r-tech1/configuration.h"
#include "r-tech1/debug.h"
#include "r-tech1/file-system.h"
#include "r-tech1/input/input-source.h"
#include "../level/utils.h"
#include "../object/player.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

namespace Paintown{

class ArcadeArgument: public Argument::Parameter {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("paintown:arcade");
        return out;
    }

    string description() const {
        return " player=<name>: Start an arcade game in the Paintown engine";
    }

    struct Data{
        Filesystem::AbsolutePath player;
    };

    class Run: public Argument::Action {
    public:
        Run(const Data & data):
        data(data){
        }

        Data data;

        void act(){
            vector<Level::LevelInfo> levels = Mod::getCurrentMod()->getLevels();
            if (levels.size() == 0){
                Global::debug(0) << "No levels" << std::endl;
                return;
            }
            int remap = 0;

            /*
            PlayerFuture future(data.player, Configuration::getInvincible(), Configuration::getLives(), remap, Util::ReferenceCount<InputSource>(new InputSource(InputSource(false).addKeyboard(0).addJoystick(0))));
            vector<Util::Future<Object *> *> players;
            players.push_back(&future);
            */

            Util::ReferenceCount<InputSource> source = Util::ReferenceCount<InputSource>(new InputSource(InputSource(false).addKeyboard(0).addJoystick(0)));

            Player * player = new Player(data.player, source);
            player->setInvincible(Configuration::getInvincible());
            player->setMap(remap);
            player->setObjectId(-1);
            player->setLives(Configuration::getLives());
            vector<Util::ReferenceCount<Object>> players;
            players.push_back(Util::ReferenceCount<Object>(player));

            Game::realGame(players, levels[0]);
        }
    };

    static vector<string> split(string str, char splitter){
        vector<string> strings;
        size_t next = str.find(splitter);
        while (next != string::npos){
            strings.push_back(str.substr(0, next));
            str = str.substr(next+1);
            next = str.find(splitter);
        }
        if (str != ""){
            strings.push_back(str);
        }

        return strings;
    }

    Filesystem::AbsolutePath findPlayer(const string & name){
        Filesystem::RelativePath path("players/" + name + "/" + name + ".txt");
        return Storage::instance().find(path);
    }

    Data parse(string input){
        Data out;
        vector<string> args = split(input, '=');

        if (args.size() == 2 && args[0] == "player"){
            out.player = findPlayer(args[1]);
        }

        return out;
    }

    /* given the rest of the arguments, try to read player=x from the next argument */
    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        current++;
        if (current != end){
            try{
                Data data(parse(*current));
                actions.push_back(::Util::ReferenceCount<Argument::Action>(new Run(data)));
            } catch (const Filesystem::NotFound & fail){
                Global::debug(0) << fail.getTrace() << std::endl;
            }
        }
        return current;
    }
};

std::vector< ::Util::ReferenceCount<Argument::Parameter> > arguments(){
    vector< ::Util::ReferenceCount<Argument::Parameter> > all;
    all.push_back(::Util::ReferenceCount<Argument::Parameter>(new ArcadeArgument()));
    return all;
}

}
