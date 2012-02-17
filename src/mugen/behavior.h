#ifndef _paintown_mugen_behavior_h
#define _paintown_mugen_behavior_h

#include <vector>
#include <string>
#include "util.h"
#include "util/input/input-map.h"
#include "command.h"

namespace Mugen{

class Object;
class Character;
class Command;
class Stage;

/* handles input and tells the character what commands to invoke */
class Behavior{
public:
    Behavior();

    virtual std::vector<std::string> currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed) = 0;

    /* called when the player changes direction. useful for updating
     * the input mapping.
     */
    virtual void flip() = 0;

    /* hit someone */
    virtual void hit(Object * enemy);

    virtual ~Behavior();
};

class HumanBehavior: public Behavior {
public:
    HumanBehavior(const InputMap<Keys> & left, const InputMap<Keys> & right);

    virtual std::vector<std::string> currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed);
    
    virtual void flip();

    virtual ~HumanBehavior();

protected:
    InputMap<Keys> & getInput(bool facing);
    Mugen::Input updateInput(InputMap<Keys> & keys, Mugen::Input old);

protected:
    InputMap<Keys> right;
    InputMap<Keys> left;
    Mugen::Input input;
};

/* dummy does absolutely nothing */
class DummyBehavior: public Behavior {
public:
    DummyBehavior();

    virtual std::vector<std::string> currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed);
    virtual void flip();

    virtual ~DummyBehavior();
};

/* This behavior chooses commands completely at random */
class RandomAIBehavior: public Behavior {
public:
    RandomAIBehavior();

    virtual std::vector<std::string> currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed);
    virtual void flip();

    virtual ~RandomAIBehavior();
};

class ScriptedBehavior: public Behavior {
public:
    ScriptedBehavior(const Filesystem::AbsolutePath & path);

    struct Action{
        Action(int ticks, const std::vector<std::string> & commands):
            ticks(ticks),
            commands(commands){
            }

        int ticks;
        std::vector<std::string> commands;
    };

    virtual std::vector<std::string> currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed);
    virtual void flip();

    virtual ~ScriptedBehavior();

protected:
    std::vector<Action> actions;
    std::vector<Action>::iterator currentAction;
};

/* This behavior will attempt to learn which moves do damage and how likely
 * they are to hit.
 */
class LearningAIBehavior: public Behavior {
public:
    /* 1 is easy, 10 is hard */
    LearningAIBehavior(int difficult);

    virtual std::vector<std::string> currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed);
    virtual void flip();
    
    virtual void hit(Object * enemy);

    virtual ~LearningAIBehavior();

    /* stores information about the player when the move was executed */
    struct Move{
        Move():
        points(0),
        attempts(0),
        consecutive(0),
        minimumDistance(-1),
        maximumDistance(-1){
        }

        /* times the move succeeded */
        int points;

        /* times the move was tried */
        int attempts;

        /* how many consecutive times this move was tried */
        double consecutive;

        /* minimum known distance the move succeeded */
        int minimumDistance;

        /* maximum known distance the move succeeded */
        int maximumDistance;
    };
    
    enum Direction{
        Forward, Backward, Crouch, Stopped
    };

protected:
    std::string selectBestCommand(int distance, const std::vector<Command*> & commands);

    std::map<std::string, Move> moves;

    std::string lastCommand;
    int lastDistance;
    Direction direction;
    int difficulty;
    int dontMove;
};

}

#endif
