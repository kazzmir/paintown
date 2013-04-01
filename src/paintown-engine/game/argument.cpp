#include "argument.h"
#include "game.h"
#include "mod.h"
#include "util/regex.h"
#include "util/configuration.h"
#include "util/debug.h"
#include "paintown-engine/level/utils.h"
#include "paintown-engine/object/player.h"
#include "util/file-system.h"
#include "util/input/input-source.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

namespace Paintown{

class ArcadeArgument: public Argument {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("paintown:arcade");
        return out;
    }

    string description() const {
        return " : Start an arcade game in the Paintown engine";
    }

    struct Data{
        Filesystem::AbsolutePath player;
    };

    class Run: public ArgumentAction {
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
            PlayerFuture future(data.player, Configuration::getInvincible(), Configuration::getLives(), remap, Util::ReferenceCount<InputSource>(new InputSource(InputSource(false).addKeyboard(0).addJoystick(0))));
            vector<Util::Future<Object *> *> players;
            players.push_back(&future);
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
        vector<string> args = split(input, ',');

        for (vector<string>::iterator it = args.begin(); it != args.end(); it++){
            string arg = *it;
            if (Util::matchRegex(arg, Util::Regex("player=.*"))){
                string name = Util::captureRegex(arg, Util::Regex("player=(.*)"), 0);
                out.player = findPlayer(name);
            }
        }

        return out;
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, ActionRefs & actions){
        current++;
        if (current != end){
            try{
                Data data(parse(*current));
                actions.push_back(::Util::ReferenceCount<ArgumentAction>(new Run(data)));
            } catch (const Filesystem::NotFound & fail){
                Global::debug(0) << fail.getTrace() << std::endl;
            }
        }
        return current;
    }
};

std::vector< ::Util::ReferenceCount<Argument> > arguments(){
    vector< ::Util::ReferenceCount<Argument> > all;
    all.push_back(::Util::ReferenceCount<Argument>(new ArcadeArgument()));
    return all;
}

}
