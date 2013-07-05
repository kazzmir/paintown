#ifndef _paintown_mugen_command
#define _paintown_mugen_command

#include "util/input/input-map.h"
#include "util.h"

namespace Ast{
    class Key;
    class KeyList;
}

namespace Mugen{

struct Input{
    Input(){
    }

    bool operator==(const Input & him) const {
        return pressed == him.pressed && released == him.released;
    }

    bool operator!=(const Input & him) const {
        return !(*this == him);
    }

    struct Key{
        bool a, b, c;
        bool x, y, z;
        bool back, forward;
        bool up, down;
        bool start;

        Key():
            a(false), b(false), c(false),
            x(false), y(false), z(false),
            back(false), forward(false),
            up(false), down(false),
            start(false){
            }

        bool operator==(const Key & him) const {
            return a == him.a &&
                   b == him.b &&
                   c == him.c &&
                   x == him.x &&
                   y == him.y &&
                   z == him.z &&
                   back == him.back &&
                   forward == him.forward &&
                   up == him.up &&
                   down == him.down &&
                   start == him.start;
        }
    };

    Key pressed, released;
};

class CompiledKeySingle;
class CompiledKey{
public:
    CompiledKey();

    virtual bool pressed(const Input & keys, const Input & oldKeys, int & holdKey, const CompiledKey *& holder, const CompiledKey*& needRelease) const = 0;

    virtual std::string toString() const = 0;

    virtual bool same(const CompiledKey & key) const {
        return false;
    }

    virtual bool same(const CompiledKeySingle & key) const {
        return false;
    }

    virtual bool operator==(const CompiledKey & key) const {
        return key.same(*this);
    }

    virtual ~CompiledKey();
};

/* A finite state automata that can map a single integer to a state.
 * This is convenient for serialization (for network mode).
 */
class KeyDFA{
public:
    enum Constraint{
        Pressed,
        Released
    };

    KeyDFA(int id):
    emit(false),
    id(id),
    next(NULL){
    }
    
    virtual bool transition(const Input & input, bool & emit) = 0;
    virtual void setEmit(){
        emit = true;
    }

    virtual bool getEmit() const {
        return emit;
    }

    virtual void setNextState(KeyDFA * next){
        this->next = next;
    }

    const int getId() const {
        return id;
    }

    virtual ~KeyDFA(){
    }

protected:
    /* If `emit' is true then when this state transitions it will act
     * like an accept state and emit a value.
     */
    bool emit;
    const int id;
    KeyDFA * next;
};

/* key command */
class Command{
public:
    Command(std::string name, Ast::KeyList * keys, int maxTime, int bufferTime);

    class Exception: public MugenException {
    public:
        Exception();
        virtual ~Exception() throw ();
    };

    virtual inline std::string getName() const {
        return name;
    }

    virtual bool handle(Input keys);
    virtual bool handle2(Input keys);

    virtual ~Command();

protected:

    bool interpret(const Ast::Key * key, const Input & keys, const Input & oldKeys, int & holdKey, const Ast::Key *& holder, const Ast::Key *& needRelease);

protected:
    std::string name;
    Ast::KeyList * keys;
    std::vector<CompiledKey*> compiledKeys;
    const int maxTime;
    const int bufferTime;
    int ticks;
    int holdKey;
    std::vector<CompiledKey*>::const_iterator current;
    Input oldKeys;
    const CompiledKey * holder;
    std::vector<KeyDFA*> states;
    unsigned int currentState;
    int successTime;
    const CompiledKey * needRelease;
};

}

#endif
