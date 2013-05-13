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
    Input():
        a(false), b(false), c(false),
        x(false), y(false), z(false),
        back(false), forward(false),
        up(false), down(false),
        start(false){
        }

    bool operator==(const Input & him) const {
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

    bool operator!=(const Input & him) const {
        return !(*this == him);
    }

    bool a, b, c;
    bool x, y, z;
    bool back, forward;
    bool up, down;
    bool start;
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
    int successTime;
    const CompiledKey * needRelease;
};

}

#endif
