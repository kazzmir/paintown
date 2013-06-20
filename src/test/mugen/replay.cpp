#include <string>
#include "util/init.h"
#include "util/debug.h"
#include "util/timedifference.h"
#include "util/sound/music.h"
#include "mugen/character.h"
#include "mugen/config.h"
#include "mugen/behavior.h"
#include "mugen/stage.h"
#include "mugen/world.h"
#include "mugen/parse-cache.h"
#include "mugen/util.h"
#include "mugen/game.h"
#include "util/file-system.h"

using namespace std;

class Game{
public:
    Game(const string & playerPath1, const string & playerPath2, const string & stagePath):
    playerPath1(playerPath1),
    playerPath2(playerPath2),
    stagePath(stagePath){
    }

    string playerPath1;
    string playerPath2;
    string stagePath;

    void load(){
        Mugen::ParseCache cache;
        Global::debug(0) << "Loading player 1 " << playerPath1 << endl;
        player1 = PaintownUtil::ReferenceCount<Mugen::Character>(new Mugen::Character(Storage::instance().find(Filesystem::RelativePath(playerPath1)), Mugen::Stage::Player1Side));
        player1->load();
        player1Behavior = PaintownUtil::ReferenceCount<Mugen::Behavior>(new Mugen::LearningAIBehavior(Mugen::Data::getInstance().getDifficulty()));
        player1->setBehavior(player1Behavior.raw());

        Global::debug(0) << "Loading player 2 " << playerPath2 << endl;
        player2 = PaintownUtil::ReferenceCount<Mugen::Character>(new Mugen::Character(Storage::instance().find(Filesystem::RelativePath(playerPath2)), Mugen::Stage::Player2Side));
        player2->load();
        player2Behavior = PaintownUtil::ReferenceCount<Mugen::Behavior>(new Mugen::LearningAIBehavior(Mugen::Data::getInstance().getDifficulty()));
        player2->setBehavior(player2Behavior.raw());

        Global::debug(0) << "Loading stage" << std::endl;
        stage = PaintownUtil::ReferenceCount<Mugen::Stage>(new Mugen::Stage(Storage::instance().find(Filesystem::RelativePath(stagePath))));
        stage->addPlayer1(player1.raw());
        stage->addPlayer2(player2.raw());
        stage->load();
    }

    PaintownUtil::ReferenceCount<Mugen::Character> player1;
    PaintownUtil::ReferenceCount<Mugen::Character> player2;
    PaintownUtil::ReferenceCount<Mugen::Stage> stage;
    PaintownUtil::ReferenceCount<Mugen::Behavior> player1Behavior;
    PaintownUtil::ReferenceCount<Mugen::Behavior> player2Behavior;
};

int run(string path1 = "mugen/chars/kfm/kfm.def", string path2 = "mugen/chars/kfm/kfm.def"){
    Game game(path1, path2, "mugen/stages/kfm.def");
    Mugen::Random randomState(*Mugen::Random::getState());
    game.load();

    vector<PaintownUtil::ReferenceCount<Mugen::World> > worlds;

    Global::debug(0) << "Run match" << std::endl;
    /* Don't need the real timer here because we just invoke the logic portion of stage
     * as fast as possible.
     */
    TimeDifference diff;
    diff.startTime();
    PaintownUtil::ReferenceCount<Mugen::Stage> stage = game.stage;
    while (!stage->isMatchOver()){
        worlds.push_back(stage->snapshotState());
        stage->logic();
    }
    diff.endTime();
    Global::debug(0, "test") << diff.printTime("Took") << endl;

    /* Reset the state */
    Mugen::Random::setState(randomState);
    game.load();

    Global::debug(0) << "Rerun match and check states" << std::endl;
    /* Now check that all previous states match what comes out of the new game */
    stage = game.stage;
    {
        int tick = 0;
        diff.startTime();
        int last = 0;
        int count = 0;
        for (vector<PaintownUtil::ReferenceCount<Mugen::World> >::iterator it = worlds.begin(); it != worlds.end(); it++){
            PaintownUtil::ReferenceCount<Mugen::World> oldWorld = *it;
            PaintownUtil::ReferenceCount<Mugen::World> newWorld = stage->snapshotState();
            if (*oldWorld != *newWorld){
                Global::debug(0) << "Worlds do not match at tick " << tick << std::endl;
                Global::debug(0) << "Old World: " << oldWorld->serialize()->toString() << std::endl;
                Global::debug(0) << "New World: " << newWorld->serialize()->toString() << std::endl;
                return 1;
            }
            stage->logic();
            tick += 1;
            count += 1;

            int next = count * 100 / worlds.size();
            if (next > last){
                last = next;
                std::ostream & out = Global::debug(0);
                out << next << "%" << "\r";
                out.flush();
            }
        }
        diff.endTime();
        Global::debug(0, "test") << diff.printTime("Took") << endl;
    }
    
    Mugen::Random::setState(randomState);
    game.load();

    Global::debug(0) << "Set every 50th state." << std::endl;
    /* Now check that all previous states match what comes out of the new game */
    stage = game.stage;
    {
        int tick = 0;
        diff.startTime();
        int last = 0;
        int count = 0;
        for (vector<PaintownUtil::ReferenceCount<Mugen::World> >::iterator it = worlds.begin(); it != worlds.end(); it++){
            PaintownUtil::ReferenceCount<Mugen::World> oldWorld = *it;
            if (tick > 0 && tick % 50 == 0){
                stage->updateState(*oldWorld);
            }
            PaintownUtil::ReferenceCount<Mugen::World> newWorld = stage->snapshotState();
            if (*oldWorld != *newWorld){
                Global::debug(0) << "Worlds do not match at tick " << tick << std::endl;
                Global::debug(0) << "Old World: " << oldWorld->serialize()->toString() << std::endl;
                Global::debug(0) << "New World: " << newWorld->serialize()->toString() << std::endl;
                return 1;
            }
            stage->logic();
            tick += 1;
            count += 1;

            int next = count * 100 / worlds.size();
            if (next > last){
                last = next;
                std::ostream & out = Global::debug(0);
                out << next << "%" << "\r";
                out.flush();
            }
        }
        diff.endTime();
        Global::debug(0, "test") << diff.printTime("Took") << endl;
    }

    return 0;
}

class RecordHumanBehavior: public Mugen::HumanBehavior {
public:
    RecordHumanBehavior(const InputMap<Mugen::Keys> & right, const InputMap<Mugen::Keys> & left):
    HumanBehavior(right, left){
    }

    void writeInput(const Mugen::Input & input){
        std::ostream & out = Global::debug(0);
        vector<string> inputs;
        if (input.forward){
            inputs.push_back("F");
        }
        if (input.back){
            inputs.push_back("B");
        }
        if (input.up){
            inputs.push_back("U");
        }
        if (input.down){
            inputs.push_back("D");
        }
        if (input.a){
            inputs.push_back("a");
        }
        if (input.b){
            inputs.push_back("b");
        }
        if (input.c){
            inputs.push_back("c");
        }
        if (input.x){
            inputs.push_back("x");
        }
        if (input.y){
            inputs.push_back("y");
        }
        if (input.z){
            inputs.push_back("z");
        }
        if (input.start){
            inputs.push_back("s");
        }
        bool first = true;
        for (vector<string>::iterator it = inputs.begin(); it != inputs.end(); it++){
            if (!first){
                out << ",";
            }
            out << *it;
            first = false;
        }
        out << "\n";
    }

    vector<string> currentCommands(const Mugen::Stage & stage, Mugen::Character * owner, const vector<Mugen::Command*> & commands, bool reversed){
        vector<string> out = Mugen::HumanBehavior::currentCommands(stage, owner, commands, reversed);
        writeInput(getInput());
        return out;
    }
};

void record(){
    Music music(false);
    Util::Parameter<Util::ReferenceCount<Path::RelativePath> > defaultFont(Font::defaultFont, Util::ReferenceCount<Path::RelativePath>(new Path::RelativePath("fonts/arial.ttf")));
    Util::Parameter<Util::ReferenceCount<Graphics::ShaderManager> > defaultShaderManager(Graphics::shaderManager, Util::ReferenceCount<Graphics::ShaderManager>(new Graphics::ShaderManager()));
    Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, Graphics::getScreenBuffer());

    string kfm = "mugen/chars/kfm/kfm.def";
    Game game(kfm, kfm, "mugen/stages/kfm.def");
    Mugen::Random randomState(*Mugen::Random::getState());
    game.load();

    RecordHumanBehavior human(Mugen::getPlayer1Keys(), Mugen::getPlayer1InputLeft());
    game.player1->setBehavior(&human);
    PaintownUtil::ReferenceCount<Mugen::Stage> stage = game.stage;
    stage->reset();
    Mugen::Game::runMatch(stage.raw(), "", Mugen::RunMatchOptions());
}

int main(int argc, char ** argv){
    InputManager manager;
    if (argc >= 2 && string("record") == argv[1]){
        Global::init(Global::WINDOWED);
        Global::setDebug(0);
        srand(0);

        try{
            record();
        } catch (QuitGameException & quit){
        }
    } else {
    }

    return 0;
}
