#include "behavior.h"
#include <vector>
#include <string>
#include <math.h>
#include "util/input/input-map.h"
#include "util/input/input-source.h"
#include "command.h"
#include "util/debug.h"
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
    
void Behavior::hit(Object * enemy){
}

DummyBehavior::DummyBehavior(){
}
    
void DummyBehavior::flip(){
}

vector<string> DummyBehavior::currentCommands(const Mugen::Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed){
    vector<string> out;
    return out;
}

DummyBehavior::~DummyBehavior(){
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

/* turned around so we have to swap forward/backward */
void HumanBehavior::flip(){
    bool forward = input.forward;
    input.forward = input.back;
    input.back = forward;
}

Mugen::Input HumanBehavior::updateInput(InputMap<Keys> & keys, Mugen::Input old){
    vector<InputMap<Keys>::InputEvent> eventsHold = InputManager::getEvents(keys, InputSource());
    for (vector<InputMap<Keys>::InputEvent>::iterator it = eventsHold.begin(); it != eventsHold.end(); it++){
        InputMap<Keys>::InputEvent event = *it;

        switch (event.out){
            /* the keys map already sets up left/right to be forward/backward */
            case Mugen::Right: old.forward = event.enabled; break;
            case Mugen::Left: old.back = event.enabled; break;
            case Mugen::Up: old.up = event.enabled; break;
            case Mugen::Down: old.down = event.enabled; break;
            case Mugen::A: old.a = event.enabled; break;
            case Mugen::B: old.b = event.enabled; break;
            case Mugen::C: old.c = event.enabled; break;
            case Mugen::X: old.x = event.enabled; break;
            case Mugen::Y: old.y = event.enabled; break;
            case Mugen::Z: old.z = event.enabled; break;
            case Mugen::Start: old.start = event.enabled; break;
            default: break;
        }
    }

    return old;
}

vector<string> HumanBehavior::currentCommands(const Mugen::Stage & stage, Character * owner, const vector<Command*> & commands, bool reversed){
    vector<string> out;
    
    // InputMap<Mugen::Keys>::Output output = InputManager::getMap(getInput(reversed));
    input = updateInput(getInput(reversed), input);

    for (vector<Command*>::const_iterator it = commands.begin(); it != commands.end(); it++){
        Command * command = *it;
        if (command->handle(input)){
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
    
void RandomAIBehavior::flip(){
}

static string randomCommand(const vector<Command*> & commands){
    if (commands.size() == 0){
        return "";
    }

    int choice = PaintownUtil::rnd(commands.size());
    return commands[choice]->getName();
}

vector<string> RandomAIBehavior::currentCommands(const Mugen::Stage & stage, Character * owner, const vector<Command*> & commands, bool reversed){
    vector<string> out;
    if (PaintownUtil::rnd(100) > 90){
        out.push_back(randomCommand(commands));
    }
    return out;
}

RandomAIBehavior::~RandomAIBehavior(){
}

LearningAIBehavior::LearningAIBehavior(int difficulty):
direction(Forward),
difficulty(difficulty),
dontMove(0){
    /* make walking more likely to begin with */
    moves["holdfwd"].points -= 10;
    moves["holdfwd"].minimumDistance = 999999;
    moves["holdfwd"].maximumDistance = 0;
    moves["holdback"].points -= 10;
    moves["holdback"].minimumDistance = 999999;
    moves["holdback"].maximumDistance = 0;

    /* this is just to give the AI a starting move that does nothing, this is
     * basically a noop. eventualy the AI will learn other moves to do.
     */
    moves["not-possible+#$*(@#$"].points = 5;
}

/* the command with the most points will be chosen. points are tallied by
 * adding all the quantities associated with that move thus far. right now they are:
 *  - starting points is the number of times the move hit successfully
 *  - add points if the move is within range, where range is determined by the last
 *     successful hit for that command
 *  - add a random number of points to shake things up
 *  - subtract points if the move is out of range
 *  - subtract points based on the number of times the move has been tried
 *  - subtract points if the move has been done recently
 */
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
            } else if (distance > move.maximumDistance + 10){
                morePoints -= 3;
            }
        }

        morePoints -= log((double) move.attempts * 3);
        morePoints -= move.consecutive * 2;

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

    for (map<string, Move>::iterator it = moves.begin(); it != moves.end(); it++){
        Move & move = (*it).second;
        if (currentMove != &move){
            move.consecutive /= 2;
        }
    }
        
    currentMove->attempts += 1;
    currentMove->consecutive += 1;

    return what;
}
    
void LearningAIBehavior::flip(){
}

static LearningAIBehavior::Direction randomDirection(){
    int what = PaintownUtil::rnd(100);
    if (what > 70){
        return LearningAIBehavior::Forward;
    } else if (what > 40){
        return LearningAIBehavior::Backward;
    } else if (what > 20){
        return LearningAIBehavior::Stopped;
    } else {
        return LearningAIBehavior::Crouch;
    }
}

vector<string> LearningAIBehavior::currentCommands(const Mugen::Stage & stage, Character * owner, const vector<Command*> & commands, bool reversed){

    vector<string> out;

    /* maybe attack */
    if (PaintownUtil::rnd(200) < difficulty * 2){
        const Character * enemy = stage.getEnemy(owner);
        int xDistance = (int) fabs(owner->getX() - enemy->getX());
        string command = selectBestCommand(xDistance, commands);
        out.push_back(command);
        lastCommand = command;
        lastDistance = xDistance;
    } else {
        /* otherwise move around */
        dontMove += 1;
        if (direction == Forward){
            out.push_back("holdfwd");
        } else if (direction == Backward){
            out.push_back("holdback");
        } else if (direction == Crouch){
            out.push_back("holddown");
        } else if (direction == Stopped){
        }
            
        /* after keeping a direction for 40 ticks, maybe change directions */
        if (dontMove > 40 && PaintownUtil::rnd(10) > 8){
            direction = randomDirection();
            dontMove = 0;
        }

        /* make the AI jump sometimes */
        if (PaintownUtil::rnd(100) == 0){
            out.push_back("holdup");
        }
    }

    return out;
}

/* hit succeeded, reinforce learning behavior for that move */
void LearningAIBehavior::hit(Object * enemy){
    Move & move = moves[lastCommand];
    move.points += 1;
    if (move.points > 5){
        move.points = 5;
    }

    if (move.maximumDistance == -1 || lastDistance > move.maximumDistance){
        move.maximumDistance = lastDistance;
    }

    if (move.minimumDistance == -1 || lastDistance < move.minimumDistance){
        move.minimumDistance = lastDistance;
    }
}

LearningAIBehavior::~LearningAIBehavior(){
}

ScriptedBehavior::ScriptedBehavior(const Filesystem::AbsolutePath & path){
    ifstream file(path.path().c_str());
    while (file.good() && !file.eof()){

        char line[1024];
        file.getline(line, sizeof(line) - 1);
        line[sizeof(line) - 1] = '\0';
        string whole(line);
        
        if (PaintownUtil::matchRegex(whole, "\\d+\\s+\\w+")){
            int ticks = atoi(PaintownUtil::captureRegex(whole, "(\\d+)", 0).c_str());
            string command = PaintownUtil::captureRegex(whole, "\\d+\\s+(\\w+)", 0);
            actions.push_back(Action(ticks, command));
            Global::debug(1) << "Added action " << command << " for " << ticks << std::endl;
        } else if (PaintownUtil::matchRegex(whole, "\\d+")){
            int ticks = atoi(PaintownUtil::captureRegex(whole, "(\\d+)", 0).c_str());
            actions.push_back(Action(ticks, ""));
            Global::debug(1) << "Added no action for " << ticks << std::endl;
        } else if (PaintownUtil::matchRegex(whole, "\\w+\\s*")){
            string command = PaintownUtil::captureRegex(whole, "(\\w+)\\s*", 0);
            actions.push_back(Action(1, command));
            Global::debug(1) << "Added action " << command << " for 1" << std::endl;
        }
    }

    currentAction = actions.begin();
}

std::vector<std::string> ScriptedBehavior::currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed){
    vector<string> out;

    if (currentAction != actions.end()){
        Action & action = *currentAction;

        action.ticks -= 1;
        out.push_back(action.command);

        if (action.ticks == 0){
            currentAction++;
        }
    }

    return out;
}
    
void ScriptedBehavior::flip(){
}
    
ScriptedBehavior::~ScriptedBehavior(){
}

}
