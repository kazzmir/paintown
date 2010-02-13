#ifndef _paintown_mugen_command
#define _paintown_mugen_command

#include "input/input-map.h"
#include "util.h"

namespace Ast{
    class Key;
    class KeyList;
}

namespace Mugen{

/* key command */
class Command{
public:
    Command(std::string name, Ast::KeyList * keys, int maxTime, int bufferTime);
#if 0
    enum Keys{
        Down,
        Up,
        Forward,
        Back,
        X,
        Y,
        Z,
        A,
        B,
        C,
    };
#endif 
    virtual inline std::string getName() const {
        return name;
    }

    virtual bool handle(InputMap<Mugen::Keys>::Output keys);

    virtual ~Command();

protected:
    std::string name;
    Ast::KeyList * keys;
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
