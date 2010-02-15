#include "behavior.h"
#include <vector>
#include <string>
#include <math.h>
#include "input/input-map.h"
#include "command.h"
#include "character.h"
#include "stage.h"
#include "util/funcs.h"

using namespace std;

namespace Mugen{

namespace PaintownUtil = ::Util;

Behavior::Behavior(){
}

Behavior::~Behavior(){
}
    
void Behavior::hit(Character * enemy){
}

HumanBehavior::HumanBehavior(InputMap<Mugen::Keys> right, InputMap<Mugen::Keys> left):
right(right),
left(left){
}
    
InputMap<Keys> & HumanBehavior::getInput(bool facingRight){
    if (facingRight){
        return right;
    }
    return left;
}

vector<string> HumanBehavior::currentCommands(const MugenStage & stage, Character * owner, const vector<Command*> & commands, bool reversed){
    vector<string> out;
    
    InputMap<Mugen::Keys>::Output output = InputManager::getMap(getInput(reversed));

    for (vector<Command*>::const_iterator it = commands.begin(); it != commands.end(); it++){
        Command * command = *it;
        if (command->handle(output)){
            Global::debug(2) << "command: " << command->getName() << endl;
            out.push_back(command->getName());
        }
    }

    return out;

}

HumanBehavior::~HumanBehavior(){
}

RandomAIBehavior::RandomAIBehavior(){
}

static string randomCommand(const vector<Command*> & commands){
    if (commands.size() == 0){
        return "";
    }

    int choice = PaintownUtil::rnd(commands.size());
    return commands[choice]->getName();
}

vector<string> RandomAIBehavior::currentCommands(const MugenStage & stage, Character * owner, const vector<Command*> & commands, bool reversed){
    vector<string> out;
    if (PaintownUtil::rnd(100) > 90){
        out.push_back(randomCommand(commands));
    }
    return out;
}

RandomAIBehavior::~RandomAIBehavior(){
}

LearningAIBehavior::LearningAIBehavior(){
    /* make walking more likely to begin with */
    moves["holdfwd"].points -= 10;
    moves["holdfwd"].minimumDistance = 999999;
    moves["holdfwd"].maximumDistance = 0;
    moves["holdback"].points -= 10;
    moves["holdback"].minimumDistance = 999999;
    moves["holdback"].maximumDistance = 0;
    moves["not-possible+#$*(@#$"].points = 5;

    direction = Forward;
}

string LearningAIBehavior::selectBestCommand(int distance, const vector<Command*> & commands){
    Move * currentMove = NULL;
    string what = "";
    double points = 0;

    for (vector<Command*>::const_iterator it = commands.begin(); it != commands.end(); it++){
        string name = (*it)->getName();
        Move & move = moves[name];
        double morePoints = move.points + PaintownUtil::rnd(10);
        if (move.minimumDistance != -1){
            if (distance < move.maximumDistance + 10 && distance > move.minimumDistance - 10){
                morePoints += 2;
            }
        }
        morePoints -= log((double) move.attempts);

        if (currentMove == NULL){
            currentMove = &move;
            what = name;
            points = morePoints;
        } else if (morePoints > points){
            currentMove = &move;
            what = name;
            points = morePoints;
        }
    }

    return what;
}

vector<string> LearningAIBehavior::currentCommands(const MugenStage & stage, Character * owner, const vector<Command*> & commands, bool reversed){
        
    vector<string> out;

    if (PaintownUtil::rnd(10) > 8){
        const Character * enemy = stage.getEnemy(owner);
        int xDistance = (int) fabs(owner->getX() - enemy->getX());
        string command = selectBestCommand(xDistance, commands);
        moves[command].attempts += 1;
        out.push_back(command);
        lastCommand = command;
        lastDistance = xDistance;
    } else if (direction == Forward){
        out.push_back("holdfwd");
        if (PaintownUtil::rnd(10) > 8){
            direction = Backward;
        }
    } else {
        out.push_back("holdback");
        if (PaintownUtil::rnd(10) > 8){
            direction = Forward;
        }
    }

    return out;
}
    
void LearningAIBehavior::hit(Character * enemy){
    Move & move = moves[lastCommand];
    move.points += 1;
    if (move.maximumDistance == -1 || lastDistance > move.maximumDistance){
        move.maximumDistance = lastDistance;
    }

    if (move.minimumDistance == -1 || lastDistance < move.minimumDistance){
        move.minimumDistance = lastDistance;
    }
}

LearningAIBehavior::~LearningAIBehavior(){
}

}
