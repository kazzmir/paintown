#include "ast/all.h"
#include "command.h"

using namespace std;

namespace Mugen{

Command::Command(string name, Ast::KeyList * keys, int maxTime, int bufferTime):
name(name),
keys(keys),
maxTime(maxTime),
bufferTime(bufferTime),
ticks(-1),
holdKey(-1),
current(keys->getKeys().begin()),
holder(0),
successTime(0),
needRelease(0){
}

bool Command::handle(InputMap<Mugen::Keys>::Output keys){
    class KeyWalker: public Ast::Walker{
    public:
        KeyWalker(InputMap<Mugen::Keys>::Output & keys, const InputMap<Mugen::Keys>::Output & oldKeys, int & holdKey, const Ast::Key *& holder, const Ast::Key *& needRelease):
        ok(false),
        fail(false),
        holdKey(holdKey),
        keys(keys),
        oldKeys(keys),
        holder(holder),
        needRelease(needRelease){
        }
        
        bool ok;
        bool fail;
        int & holdKey;
        InputMap<Mugen::Keys>::Output & keys;
        const InputMap<Mugen::Keys>::Output & oldKeys;
        const Ast::Key *& holder;
        const Ast::Key *& needRelease;

        virtual void onKeySingle(const Ast::KeySingle & key){
            if (key == "a"){
                ok = keys[Mugen::A];
            } else if (key == "b"){
                ok = keys[Mugen::B];
            } else if (key == "c"){
                ok = keys[Mugen::C];
            } else if (key == "x"){
                ok = keys[Mugen::X];
            } else if (key == "y"){
                ok = keys[Mugen::Y];
            } else if (key == "z"){
                ok = keys[Mugen::Z];
            } else if (key == "B"){
                ok = keys[Mugen::Left];
            } else if (key == "DB"){
                ok = keys[Mugen::Left] && keys[Mugen::Down];
            } else if (key == "D"){
                ok = keys[Mugen::Down];
            } else if (key == "DF"){
                ok = keys[Mugen::Right] && keys[Mugen::Down];
            } else if (key == "F"){
                ok = keys[Mugen::Right];
            } else if (key == "UF"){
                ok = keys[Mugen::Right] && keys[Mugen::Up];
            } else if (key == "U"){
                ok = keys[Mugen::Up];
            } else if (key == "UB"){
                ok = keys[Mugen::Left] && keys[Mugen::Up];
            } else if (key == "start"){
                ok = keys[Mugen::Start];
            }
            if (ok){
                needRelease = &key;
            }
        }

        bool sameKeys(const InputMap<Mugen::Keys>::Output & map1, const InputMap<Mugen::Keys>::Output & map2 ){
            return map1 == map2;
        }

        virtual void onKeyModifier(const Ast::KeyModifier & key){
            needRelease = NULL;
            switch (key.getModifierType()){
                case Ast::KeyModifier::MustBeHeldDown : {
                    key.getKey()->walk(*this);
                    if (ok){
                        holder = key.getKey();
                    }
                    break;
                }
                case Ast::KeyModifier::Release : {
                    if (key.getExtra() > 0){
                        if (holdKey > 0){
                            int fake = -1;
                            const Ast::Key * fakeKey;
                            KeyWalker walker(keys, oldKeys, fake, holder, fakeKey);
                            key.getKey()->walk(walker);
                            if (walker.ok){
                                holdKey -= 1;
                            } else {
                                fail = true;
                            }
                        } else if (holdKey == 0){
                            int fake = -1;
                            const Ast::Key * fakeKey;
                            KeyWalker walker(keys, oldKeys, fake, holder, fakeKey);
                            key.getKey()->walk(walker);
                            /* if ok is true then the key is still being pressed */
                            if (!walker.ok){
                                ok = true;
                            }
                        } else if (holdKey == -1){
                            holdKey = key.getExtra();
                        }
                    } else {
                        int fake = -1;
                        KeyWalker walker(keys, oldKeys, fake, holder, needRelease);
                        key.getKey()->walk(walker);
                        if (!walker.ok){
                            ok = true;
                        }
                    }
                    break;
                }
                case Ast::KeyModifier::Direction : {
                    key.getKey()->walk(*this);
                    break;
                }
                case Ast::KeyModifier::Only : {
                    key.getKey()->walk(*this);
                    if (!ok){
                        if (!sameKeys(keys, oldKeys)){
                            fail = true;
                        }
                    }
                    break;
                }
            }
        }

        virtual void onKeyCombined(const Ast::KeyCombined & key){
            int fake = -1;
            KeyWalker left(keys, oldKeys, fake, holder, needRelease);
            KeyWalker right(keys, oldKeys, fake, holder, needRelease);
            key.getKey1()->walk(left);
            key.getKey2()->walk(right);
            ok = left.ok && right.ok;
            if (ok){
                needRelease = &key;
            }
        }
    };
    
    if (successTime > 0){
        successTime -= 1;
        Global::debug(1) << "Pressed " << name << endl;
        return true;
    }

    bool use = true;
    if (needRelease != NULL && *needRelease == *(*current)){
        const Ast::Key * fake;
        KeyWalker walker(keys, oldKeys, holdKey, holder, fake);
        needRelease->walk(walker);
        Global::debug(1) << "Waiting for key " << needRelease->toString() << " to be released: " << walker.ok << endl;

        if (walker.ok){
            /* if the key is still held down then don't continue */
            use = false;
        } else {
            /* otherwise the key is released so reset the release key */
            needRelease = NULL;
        }
    } else {
        needRelease = NULL;
    }

    bool fail = false;
    bool ok = false;

    if (use){
        KeyWalker walker(keys, oldKeys, holdKey, holder, needRelease);
        (*current)->walk(walker);

        ok = walker.ok;
        fail = walker.fail;
        /*
        if (name == "a"){
            Global::debug(0) << "Tried move " << (*current)->toString() << " was " << ok << endl;
        }
        */
        if (holder != 0){
            holder->walk(walker);
            ok &= walker.ok;
            fail |= walker.fail;
        }
    }

    oldKeys = keys;
    if (ticks > 0){
        ticks += 1;
        if (ticks > maxTime){
            fail = true;
            /*
            if (name == "a"){
                Global::debug(0) << name << " ran out of time" << endl;
            }
            */
        }
    }

    // Global::debug(0) << "Command " << name << " at ticks " << ticks << endl;

    if (fail){
        current = this->keys->getKeys().begin();
        ticks = -1;
        // needRelease = NULL;
        holdKey = -1;
        holder = 0;
    } else if (ok){
        current++;
        if (ticks == -1){
            ticks = 1;
        }
        holdKey = -1;
        if (current == this->keys->getKeys().end()){
            /* success! */
            current = this->keys->getKeys().begin();
            ticks = -1;
            // needRelease = NULL;
            holder = 0;
            successTime = bufferTime - 1;
            Global::debug(1) << "Pressed " << name << endl;
            return true;
        }
    }

    return false;
}

Command::~Command(){
    delete keys;
}

}
