#include <string>
#include "util/init.h"
#include "util/debug.h"
#include "util/timedifference.h"
#include "mugen/character.h"
#include "mugen/config.h"
#include "mugen/behavior.h"
#include "mugen/stage.h"
#include "mugen/world.h"
#include "mugen/parse-cache.h"
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
        stage->reset();
    }

    PaintownUtil::ReferenceCount<Mugen::Character> player1;
    PaintownUtil::ReferenceCount<Mugen::Character> player2;
    PaintownUtil::ReferenceCount<Mugen::Stage> stage;
    PaintownUtil::ReferenceCount<Mugen::Behavior> player1Behavior;
    PaintownUtil::ReferenceCount<Mugen::Behavior> player2Behavior;
};

int run(string path1 = "mugen/chars/kfm/kfm.def", string path2 = "mugen/chars/kfm/kfm.def"){
    /*
    Mugen::ParseCache cache;
    string stagePath = "mugen/stages/kfm.def";
    Mugen::Character kfm1(Storage::instance().find(Filesystem::RelativePath(path1)), Mugen::Stage::Player1Side);
    Mugen::Character kfm2(Storage::instance().find(Filesystem::RelativePath(path2)), Mugen::Stage::Player2Side);
    Global::debug(0) << "Loading player 1 " << path1 << endl;
    kfm1.load();
    Global::debug(0) << "Loading player 2 " << path2 << endl;
    kfm2.load();
    Mugen::LearningAIBehavior player1AIBehavior(Mugen::Data::getInstance().getDifficulty());
    Mugen::LearningAIBehavior player2AIBehavior(Mugen::Data::getInstance().getDifficulty());
    kfm1.setBehavior(&player1AIBehavior);
    kfm2.setBehavior(&player2AIBehavior);
    Mugen::Stage stage(Storage::instance().find(Filesystem::RelativePath(stagePath)));
    stage.addPlayer1(&kfm1);
    stage.addPlayer2(&kfm2);
    Global::debug(0) << "Loading stage" << std::endl;
    stage.load();
    stage.reset();
    */

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
    int tick = 0;
    diff.startTime();
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
    }
    diff.endTime();
    Global::debug(0, "test") << diff.printTime("Took") << endl;

    return 0;
}

int main(int argc, char ** argv){
    InputManager manager;
    Global::initNoGraphics();
    Global::setDebug(0);
    /* Always use the same random number sequence */
    srand(0);
    if (argc == 1){
        return run();
    } else if (argc == 2){
        return run(argv[1]);
    } else if (argc > 2){
        return run(argv[1], argv[2]);
    }

    return 0;
}
