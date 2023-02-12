#include "behavior.h"
#include <vector>
#include <string>
#include <math.h>
#include "r-tech1/input/input-map.h"
#include "r-tech1/input/input-source.h"
#include "r-tech1/debug.h"
#include "r-tech1/funcs.h"
// #include "command.h"
#include "constraint.h"
#include "random.h"
#include "character.h"
#include "stage.h"

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

vector<string> DummyBehavior::currentCommands(const Mugen::Stage & stage, Character * owner, const std::vector<Command2*> & commands, bool reversed){
    vector<string> out;
    return out;
}

DummyBehavior::~DummyBehavior(){
}

static void debugit(const string & what, const InputMap<Mugen::Keys> & right){
    Global::debug(0) << "Debug input map " << what << std::endl;
    const std::map<Keyboard::KeyType, PaintownUtil::ReferenceCount<KeyState<Mugen::Keys> > > & keys = right.getKeyStates();
    for (std::map<Keyboard::KeyType, PaintownUtil::ReferenceCount<KeyState<Mugen::Keys> > >::const_iterator it = keys.begin(); it != keys.end(); it++){
        Keyboard::KeyType key = it->first;
        const PaintownUtil::ReferenceCount<KeyState<Mugen::Keys> > & state = it->second;
        if (state != NULL){
            Global::debug(0) << "Mapped " << key << " to " << state->out << std::endl;
        }
    } 
}

HumanBehavior::HumanBehavior(const InputMap<Mugen::Keys> & right, const InputMap<Mugen::Keys> & left):
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
    bool forward = input.pressed.forward;
    input.pressed.forward = input.pressed.back;
    input.pressed.back = forward;
}

Mugen::Input HumanBehavior::updateInput(InputMap<Keys> & keys, Mugen::Input old){
    /* Reset released events */
    old.released = Input::Key();

    vector<InputMap<Keys>::InputEvent> eventsHold = InputManager::getEvents(keys, InputSource(true));
    for (vector<InputMap<Keys>::InputEvent>::iterator it = eventsHold.begin(); it != eventsHold.end(); it++){
        InputMap<Keys>::InputEvent event = *it;

        /* Release event if the key was pressed but now its not */
#define DoKey(Name, accessor) case Mugen::Name: \
                                old.released.accessor = old.pressed.accessor && !event.enabled; \
                                old.pressed.accessor = event.enabled; break;

        switch (event.out){
            DoKey(Right, forward)
            DoKey(Left, back)
            DoKey(Up, up)
            DoKey(Down, down)
            DoKey(A, a)
            DoKey(B, b)
            DoKey(C, c)
            DoKey(X, x)
            DoKey(Y, y)
            DoKey(Z, z)
            DoKey(Start, start)
            default: break;
            /* the keys map already sets up left/right to be forward/backward */

            /*
            case Mugen::Right: {
                old.released.forward = old.pressed.forward && !event.enabled;
                old.pressed.forward = event.enabled;
                break;
            }
            case Mugen::Left: old.pressed.back = event.enabled; break;
            case Mugen::Up: old.pressed.up = event.enabled; break;
            case Mugen::Down: old.pressed.down = event.enabled; break;
            case Mugen::A: old.pressed.a = event.enabled; break;
            case Mugen::B: old.pressed.b = event.enabled; break;
            case Mugen::C: old.pressed.c = event.enabled; break;
            case Mugen::X: old.pressed.x = event.enabled; break;
            case Mugen::Y: old.pressed.y = event.enabled; break;
            case Mugen::Z: old.pressed.z = event.enabled; break;
            case Mugen::Start: old.pressed.start = event.enabled; break;
            default: break;
            */
        }
    }

#undef DoKey

    return old;
}

vector<string> HumanBehavior::currentCommands(const Mugen::Stage & stage, Character * owner, const vector<Command2*> & commands, bool reversed){
    vector<string> out;
    
    // InputMap<Mugen::Keys>::Output output = InputManager::getMap(getInput(reversed));
    input = updateInput(getInput(reversed), input);

    for (vector<Command2*>::const_iterator it = commands.begin(); it != commands.end(); it++){
        Command2 * command = *it;
        if (command->handle(input, stage.getTicks())){
            Global::debug(1) << "command: " << command->getName() << endl;
            out.push_back(command->getName());
        }
    }

    return out;
}
    
const Mugen::Input & HumanBehavior::getInput() const {
    return input;
}

HumanBehavior::~HumanBehavior(){
}

void HumanBehavior::updateKeys(const InputMap<Keys> & right, const InputMap<Keys> & left){
    this->right = right;
    this->left = left;
}

RandomAIBehavior::RandomAIBehavior(){
}
    
void RandomAIBehavior::flip(){
}

static string randomCommand(const vector<Command2*> & commands){
    if (commands.size() == 0){
        return "";
    }

    int choice = Mugen::random(commands.size());
    return commands[choice]->getName();
}

vector<string> RandomAIBehavior::currentCommands(const Mugen::Stage & stage, Character * owner, const vector<Command2*> & commands, bool reversed){
    vector<string> out;
    if (Mugen::random(100) > 90){
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
    /*
    moves["holdfwd"].points -= 10;
    moves["holdfwd"].minimumDistance = 999999;
    moves["holdfwd"].maximumDistance = 0;
    moves["holdback"].points -= 10;
    moves["holdback"].minimumDistance = 999999;
    moves["holdback"].maximumDistance = 0;
    */

    /* this is just to give the AI a starting move that does nothing, this is
     * basically a noop. eventualy the AI will learn other moves to do.
     *
     * the name is something that the user can't write for a command (hopefully) 
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
string LearningAIBehavior::selectBestCommand(int distance, const vector<Command2*> & commands){
    Move * currentMove = NULL;
    string what = "";
    double points = 0;

    for (vector<Command2*>::const_iterator it = commands.begin(); it != commands.end(); it++){
        string name = (*it)->getName();

        /* Skip movement keys */
        if (name == "holdfwd" ||
            name == "holdback" ||
            name == "holddown" ||
            name == "holdup"){
            continue;
        }

        Move & move = moves[name];
        double morePoints = move.points + Mugen::random(10);
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
    int what = Mugen::random(100);
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

vector<string> LearningAIBehavior::currentCommands(const Mugen::Stage & stage, Character * owner, const vector<Command2*> & commands, bool reversed){

    vector<string> out;

    /* maybe attack */
    if ((int) Mugen::random(200) < difficulty * 2){
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
        if (dontMove > 40 && Mugen::random(10) > 8){
            direction = randomDirection();
            dontMove = 0;
        }

        /* make the AI jump sometimes */
        if (Mugen::random(100) == 0){
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

/* crummy regex parsing */
static vector<string> parseCommands(string stuff){
    vector<string> out;

    while (PaintownUtil::matchRegex(stuff, PaintownUtil::Regex("\\s*(\\w+)"))){
        string get;
        get = PaintownUtil::captureRegex(stuff, PaintownUtil::Regex("\\s*(\\w+)"),0);
        out.push_back(get);
        stuff = PaintownUtil::captureRegex(stuff, PaintownUtil::Regex("\\s*\\w+,?\\s*(.*)"), 0);
    }

    return out;
}

ScriptedBehavior::ScriptedBehavior(const Filesystem::AbsolutePath & path){
    if (!Storage::instance().exists(path)){
        ostringstream out;
        out << "Could not load script file '" << path.path() << "'";
        throw MugenException(out.str(), __FILE__, __LINE__);
    }

    ifstream file(path.path().c_str());
    while (file.good() && !file.eof()){

        char line[1024];
        file.getline(line, sizeof(line) - 1);
        line[sizeof(line) - 1] = '\0';
        string whole(line);
        
        if (PaintownUtil::matchRegex(whole, PaintownUtil::Regex("\\d+\\s*$"))){
            int ticks = atoi(PaintownUtil::captureRegex(whole, PaintownUtil::Regex("(\\d+)"), 0).c_str());
            actions.push_back(Action(ticks, vector<string>()));
        } else if (PaintownUtil::matchRegex(whole, PaintownUtil::Regex("\\d+\\s+"))){
            int ticks = atoi(PaintownUtil::captureRegex(whole, PaintownUtil::Regex("(\\d+)"), 0).c_str());
            string command = PaintownUtil::captureRegex(whole, PaintownUtil::Regex("\\d+\\s+(.*)"), 0);
            vector<string> commands = parseCommands(command);
            actions.push_back(Action(ticks, commands));
            // Global::debug(0) << "Added action " << PaintownUtil::join(commands, ", ") << " for " << ticks << std::endl;
            // Global::debug(1) << "Added no action for " << ticks << std::endl;
        } else {
            string command = PaintownUtil::captureRegex(whole, PaintownUtil::Regex("(\\w+)\\s*"), 0);
            vector<string> commands = parseCommands(command);
            if (commands.size() > 0){
                actions.push_back(Action(1, commands));
            }
            // Global::debug(1) << "Added action " << command << " for 1" << std::endl;
        }
    }

    currentAction = actions.begin();
}

std::vector<std::string> ScriptedBehavior::currentCommands(const Stage & stage, Character * owner, const std::vector<Command2*> & commands, bool reversed){
    vector<string> out;

    if (currentAction != actions.end()){
        Action & action = *currentAction;

        action.ticks -= 1;
        out = action.commands;

        if (action.ticks == 0){
            currentAction++;

            // Global::debug(0) << "Next action " << PaintownUtil::join(action.commands, ", ") << std::endl;
        }
    }

    return out;
}
    
void ScriptedBehavior::flip(){
}
    
ScriptedBehavior::~ScriptedBehavior(){
}

}
