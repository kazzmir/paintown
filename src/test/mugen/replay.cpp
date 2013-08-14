#include <string>
#include <fstream>
#include "util/init.h"
#include "util/debug.h"
#include "util/timedifference.h"
#include "util/funcs.h"
#include "util/sound/music.h"
#include "mugen/character.h"
#include "mugen/config.h"
#include "mugen/behavior.h"
#include "mugen/stage.h"
#include "mugen/constraint.h"
#include "mugen/world.h"
#include "mugen/parse-cache.h"
#include "mugen/util.h"
#include "mugen/game.h"
#include "util/file-system.h"

using namespace std;

static const char * REPLAY_FILE = "src/test/mugen/replay.txt";

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

class PlayBehavior: public Mugen::Behavior {
public:
    PlayBehavior(const std::string & path){
        load(path);
    }

    void load(const std::string & path){
        ifstream file(path.c_str());
        while (file.good()){
            char line[256];
            file.getline(line, sizeof(line) - 1);
            line[255] = 0;
            inputs.push_back(parse(line));
        }
        file.close();
    }
    
    /* a,b,c */
    Mugen::Input parse(const std::string & line){
        Mugen::Input out;
        
        vector<string> keys = Util::splitString(line, ',');
        for (vector<string>::iterator it = keys.begin(); it != keys.end(); it++){
            string key = *it;
            if (key == "F"){
                out.pressed.forward = true;
            }
            if (key == "B"){
                out.pressed.back = true;
            }
            if (key == "U"){
                out.pressed.up = true;
            }
            if (key == "D"){
                out.pressed.down = true;
            }
            if (key == "a"){
                out.pressed.a = true;
            }
            if (key == "b"){
                out.pressed.b = true;
            }
            if (key == "c"){
                out.pressed.c = true;
            }
            if (key == "x"){
                out.pressed.x = true;
            }
            if (key == "y"){
                out.pressed.y = true;
            }
            if (key == "z"){
                out.pressed.z = true;
            }
            if (key == "s"){
                out.pressed.start = true;
            }
            if (key == "~F"){
                out.released.forward = true;
            }
            if (key == "~B"){
                out.released.back = true;
            }
            if (key == "~U"){
                out.released.up = true;
            }
            if (key == "~D"){
                out.released.down = true;
            }
            if (key == "~a"){
                out.released.a = true;
            }
            if (key == "~b"){
                out.released.b = true;
            }
            if (key == "~c"){
                out.released.c = true;
            }
            if (key == "~x"){
                out.released.x = true;
            }
            if (key == "~y"){
                out.released.y = true;
            }
            if (key == "~z"){
                out.released.z = true;
            }
            if (key == "~s"){
                out.released.start = true;
            }
        }

        return out;
    }

    void flip(){
    }

    vector<Mugen::Input> inputs;

    vector<string> currentCommands(const Mugen::Stage & stage, Mugen::Character * owner, const vector<Mugen::Command2*> & commands, bool reversed){
        vector<string> out;

        Mugen::Input input = inputs[stage.getTicks()];

        for (vector<Mugen::Command2*>::const_iterator it = commands.begin(); it != commands.end(); it++){
            Mugen::Command2 * command = *it;
            if (command->handle(input, stage.getTicks())){
                Global::debug(1) << "command: " << command->getName() << endl;
                out.push_back(command->getName());
            }
        }

        return out;
    }
};

class RecordHumanBehavior: public Mugen::HumanBehavior {
public:
    RecordHumanBehavior(const InputMap<Mugen::Keys> & right, const InputMap<Mugen::Keys> & left):
    HumanBehavior(right, left),
    out(REPLAY_FILE){
    }

    std::ofstream out;

    ~RecordHumanBehavior(){
        out.close();
    }

    void writeInput(const Mugen::Input & input){
        vector<string> inputs;
        if (input.pressed.forward){
            inputs.push_back("F");
        }
        if (input.pressed.back){
            inputs.push_back("B");
        }
        if (input.pressed.up){
            inputs.push_back("U");
        }
        if (input.pressed.down){
            inputs.push_back("D");
        }
        if (input.pressed.a){
            inputs.push_back("a");
        }
        if (input.pressed.b){
            inputs.push_back("b");
        }
        if (input.pressed.c){
            inputs.push_back("c");
        }
        if (input.pressed.x){
            inputs.push_back("x");
        }
        if (input.pressed.y){
            inputs.push_back("y");
        }
        if (input.pressed.z){
            inputs.push_back("z");
        }
        if (input.pressed.start){
            inputs.push_back("s");
        }
        if (input.released.forward){
            inputs.push_back("~F");
        }
        if (input.released.back){
            inputs.push_back("~B");
        }
        if (input.released.up){
            inputs.push_back("~U");
        }
        if (input.released.down){
            inputs.push_back("~D");
        }
        if (input.released.a){
            inputs.push_back("~a");
        }
        if (input.released.b){
            inputs.push_back("~b");
        }
        if (input.released.c){
            inputs.push_back("~c");
        }
        if (input.released.x){
            inputs.push_back("~x");
        }
        if (input.released.y){
            inputs.push_back("~y");
        }
        if (input.released.z){
            inputs.push_back("~z");
        }
        if (input.released.start){
            inputs.push_back("~s");
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

    vector<string> currentCommands(const Mugen::Stage & stage, Mugen::Character * owner, const vector<Mugen::Command2*> & commands, bool reversed){
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
    stage->setMatchWins(1);
    Mugen::Game::runMatch(stage.raw(), "", Mugen::RunMatchOptions());
}

int play(){
    /*
    Global::init(Global::WINDOWED);
    Global::setDebug(0);
    Music music(false);
    Util::Parameter<Util::ReferenceCount<Path::RelativePath> > defaultFont(Font::defaultFont, Util::ReferenceCount<Path::RelativePath>(new Path::RelativePath("fonts/arial.ttf")));
    Util::Parameter<Util::ReferenceCount<Graphics::ShaderManager> > defaultShaderManager(Graphics::shaderManager, Util::ReferenceCount<Graphics::ShaderManager>(new Graphics::ShaderManager()));
    Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, Graphics::getScreenBuffer());
    */

    string kfm = "mugen/chars/kfm/kfm.def";
    Game game(kfm, kfm, "mugen/stages/kfm.def");
    srand(0);
    Mugen::Random randomState(*Mugen::Random::getState());
    game.load();

    vector<PaintownUtil::ReferenceCount<Mugen::World> > worlds;

    {
        PlayBehavior human(REPLAY_FILE);
        game.player1->setBehavior(&human);
        PaintownUtil::ReferenceCount<Mugen::Stage> stage = game.stage;
        stage->reset();
        stage->setMatchWins(1);
        while (!stage->isMatchOver()){
            worlds.push_back(stage->snapshotState());
            stage->logic();
        }
    }
    
    Mugen::Random::setState(randomState);
    {
        int count = 200;
        game.load();
        PlayBehavior human(REPLAY_FILE);
        game.player1->setBehavior(&human);
        PaintownUtil::ReferenceCount<Mugen::Stage> stage = game.stage;
        stage->reset();
        stage->setMatchWins(1);
        while (!stage->isMatchOver()){
            PaintownUtil::ReferenceCount<Mugen::World> newWorld = stage->snapshotState();
            PaintownUtil::ReferenceCount<Mugen::World> oldWorld = worlds[stage->getTicks()];
            if (*newWorld != *oldWorld){
                Global::debug(0) << "Worlds are not the same at tick " << stage->getTicks() << std::endl;
                Global::debug(0) << "Old World: " << oldWorld->serialize()->toString() << std::endl;
                Global::debug(0) << "New World: " << newWorld->serialize()->toString() << std::endl;
                return 1;
            }
            stage->logic();

            /* Go back 100 ticks and rerun the simulation */
            if (stage->getTicks() == count){
                int before = count - 100;
                Global::debug(0) << "Rewinding by 100 ticks to " << before << std::endl;
                stage->updateState(*worlds[before]);
                count += 100;
            }
        }
    }

    return 0;

    // Mugen::Game::runMatch(stage.raw(), "", Mugen::RunMatchOptions());
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
        return play();
    }

    return 0;
}
