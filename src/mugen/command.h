#ifndef _paintown_mugen_command
#define _paintown_mugen_command

#include "input/input-map.h"
#include "util.h"

namespace Ast{
    class Key;
    class KeyList;
}

namespace Mugen{

class CompiledKey{
public:
    CompiledKey();

    virtual bool pressed(InputMap<Mugen::Keys>::Output & keys, const InputMap<Mugen::Keys>::Output & oldKeys, int & holdKey, const CompiledKey *& holder, const CompiledKey*& needRelease) = 0;

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

    virtual bool handle(InputMap<Mugen::Keys>::Output keys);

    virtual ~Command();

protected:

    bool interpret(const Ast::Key * key, InputMap<Mugen::Keys>::Output & keys, const InputMap<Mugen::Keys>::Output & oldKeys, int & holdKey, const Ast::Key *& holder, const Ast::Key *& needRelease);

protected:
    std::string name;
    Ast::KeyList * keys;
    std::vector<CompiledKey*> compiledKeys;
    const int maxTime;
    const int bufferTime;
    int ticks;
    int holdKey;
    std::vector<Ast::Key*>::const_iterator current;
    InputMap<Mugen::Keys>::Output oldKeys;
    const Ast::Key * holder;
    int successTime;
    const Ast::Key * needRelease;
};

}

#endif
