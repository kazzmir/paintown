#include <string>
#include "util/init.h"
#include "util/debug.h"
#include "util/timedifference.h"
#include "mugen/character.h"
#include "mugen/config.h"
#include "mugen/behavior.h"
#include "mugen/stage.h"
#include "mugen/parse-cache.h"
#include "util/file-system.h"

using namespace std;

void run(string path1 = "mugen/chars/kfm/kfm.def", string path2 = "mugen/chars/kfm/kfm.def"){
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

    Global::debug(0) << "Run match" << std::endl;
    /* Don't need the real timer here because we just invoke the logic portion of stage
     * as fast as possible.
     */
    vector<string> commands;
    TimeDifference diff;
    diff.startTime();
    for (int i = 0; i < 20000; i++){
        kfm1.testStates(stage, commands, -1);
        kfm1.testStates(stage, commands, -2);
    }
    diff.endTime();
    Global::debug(0, "test") << diff.printTime("Success! Took") << endl;
}

int main(int argc, char ** argv){
    InputManager manager;
    Global::InitConditions conditions;
    conditions.graphics = Global::InitConditions::Disabled;
    Global::init(conditions);
    if (argc == 1){
        run();
    } else if (argc == 2){
        run(argv[1]);
    } else if (argc > 2){
        run(argv[1], argv[2]);
    }
}
