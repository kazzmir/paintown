#include "ast/all.h"
#include "command.h"
#include "util/debug.h"

using namespace std;

namespace Mugen{

CompiledKey::CompiledKey(){
}

CompiledKey::~CompiledKey(){
}

class CompiledKeySingle: public CompiledKey {
public:
    enum Keys{
        A, B, C, X, Y, Z,
        Back, Forward, Down, Up,
        DownBack, UpBack,
        DownForward, UpForward,
        Start,
    };

    CompiledKeySingle(const Ast::KeySingle & name):
    key(convertKey(name)){
    }
    
    using CompiledKey::same;
    virtual bool same(const CompiledKeySingle & key) const {
        return this->key == key.key;
    }
    
    virtual bool operator==(const CompiledKey & key) const {
        return key.same(*this);
    }
    
    std::string toString() const{
        switch (key){
            case A : return "a";
            case B : return "b";
            case C : return "c";
            case X : return "x";
            case Y : return "y";
            case Z : return "z";
            case Back : return "B";
            case Forward : return "F";
            case Down : return "D";
            case Up : return "U";
            case DownBack : return "DB";
            case UpBack : return "UB";
            case DownForward : return "DF";
            case UpForward : return "UF";
            case Start : return "s";
        }
        return "???";
    }

    static Keys convertKey(const Ast::KeySingle & name){
        if (name == "a"){
            return A;
        } else if (name == "b"){
            return B;
        } else if (name == "c"){
            return C;
        } else if (name == "x"){
            return X;
        } else if (name == "y"){
            return Y;
        } else if (name == "z"){
            return Z;
        } else if (name == "B"){
            return Back;
        } else if (name == "DB"){
            return DownBack;
        } else if (name == "D"){
            return Down;
        } else if (name == "DF"){
            return DownForward;
        } else if (name == "F"){
            return Forward;
        } else if (name == "UF"){
            return UpForward;
        } else if (name == "U"){
            return Up;
        } else if (name == "UB"){
            return UpBack;
        } else if (name == "s"){
            return Start;
        }

        /* maybe throw an exception? */
        return Start;
    }

    bool pressedKey(const Input & keys) const {
        switch (key){
            case A: return keys.a;
            case B: return keys.b;
            case C: return keys.c;
            case X: return keys.x;
            case Y: return keys.y;
            case Z: return keys.z;
            case Back: return keys.back;
            case Forward: return keys.forward;
            case Down: return keys.down;
            case Up: return keys.up;
            case DownBack: return keys.down && keys.back;
            case UpBack: return keys.up && keys.back;
            case DownForward: return keys.down && keys.forward;
            case UpForward: return keys.up && keys.forward;
            case Start: return keys.start;
            default: return false;
        }
    }
    
    bool pressed(const Input & keys, const Input & oldKeys, int & holdKey, const CompiledKey *& holder, const CompiledKey*& needRelease) const {
        if (pressedKey(keys)){
            needRelease = this;
            return true;
        }

        return false;
    }

protected:
    Keys key;
};

class CompiledKeyCombined: public CompiledKey {
public:
    CompiledKeyCombined(const CompiledKey * key1, const CompiledKey * key2):
    key1(key1),
    key2(key2){
    }
    
    virtual std::string toString() const {
        ostringstream out;
        out << key1->toString() << "+" << key2->toString();
        return out.str();
    }
    
    virtual bool operator==(const CompiledKey & key) const {
        return key.same(*this);
    }

    bool pressed(const Input & keys, const Input & oldKeys, int & holdKey, const CompiledKey *& holder, const CompiledKey*& needRelease) const {
        int fake = -1;
        bool ok = key1->pressed(keys, oldKeys, fake, holder, needRelease) &&
                  key2->pressed(keys, oldKeys, fake, holder, needRelease);
        if (ok){
            needRelease = this;
        }
        return ok;
    }

    virtual ~CompiledKeyCombined(){
        delete key1;
        delete key2;
    }

protected:
    const CompiledKey * key1;
    const CompiledKey * key2;
};

class CompiledKeyMustBeHeldDown: public CompiledKey {
public:
    CompiledKeyMustBeHeldDown(const Ast::KeyModifier & ast, CompiledKey * key):
    key(key){
    }
    
    virtual std::string toString() const {
        ostringstream out;
        out << "/" << key->toString();
        return out.str();
    }
    
    bool pressed(const Input & keys, const Input & oldKeys, int & holdKey, const CompiledKey *& holder, const CompiledKey*& needRelease) const {
        if (key->pressed(keys, oldKeys, holdKey, holder, needRelease)){
            needRelease = NULL;
            holder = key;
            return true;
        }
        return false;
    }
    
    virtual bool operator==(const CompiledKey & key) const {
        return key.same(*this);
    }

    virtual ~CompiledKeyMustBeHeldDown(){
        delete key;
    }

protected:
    CompiledKey * key;
};

class CompiledKeyRelease: public CompiledKey {
public:
    CompiledKeyRelease(const Ast::KeyModifier & ast, CompiledKey * key):
    key(key),
    time(0){
        time = ast.getExtra();
        /*
        if (time < 1){
            time = 1;
        }
        */
    }
    
    virtual bool operator==(const CompiledKey & key) const {
        return key.same(*this);
    }

    virtual std::string toString() const {
        ostringstream out;
        out << "~";
        if (time != 0){
            out << time;
        }
        out << key->toString(); 
        return out.str();
    }
    
    bool pressed(const Input & keys, const Input & oldKeys, int & holdKey, const CompiledKey *& holder, const CompiledKey*& needRelease) const {
        if (holdKey > 0){
            int fake = -1;
            const CompiledKey * fakeKey;
            if (key->pressed(keys, oldKeys, fake, holder, fakeKey)){
                holdKey -= 1;
                return false;
            } else {
                throw Command::Exception();
            }
        } else if (holdKey == 0){
            int fake = -1;
            const CompiledKey * fakeKey;
            return !key->pressed(keys, oldKeys, fake, holder, fakeKey);
        } else if (holdKey == -1){
            int fake = -1;
            const CompiledKey * fakeKey;
            if (key->pressed(keys, oldKeys, fake, holder, fakeKey)){
                holdKey = time;
            } else if (time == 0){
                return true;
            }
            return false;
        }

        /*
        if (time > 0){
            if (holdKey > 0){
                int fake = -1;
                const CompiledKey * fakeKey;
                if (key->pressed(keys, oldKeys, fake, holder, fakeKey)){
                    holdKey -= 1;
                    return false;
                } else {
                    throw Command::Exception();
                }
            } else if (holdKey == 0){
                int fake = -1;
                const CompiledKey * fakeKey;
                return !key->pressed(keys, oldKeys, fake, holder, fakeKey);
            } else if (holdKey == -1){
                holdKey = time;
                return false;
            }
        } else {
            int fake = -1;
            return !key->pressed(keys, oldKeys, fake, holder, needRelease);
        }
        */

        throw Command::Exception();
    }

    virtual ~CompiledKeyRelease(){
        delete key;
    }

protected:
    CompiledKey * key;
    int time;
};
 
/* FIXME: make direction keys work
 * This doesn't look like its used, instead direction keys are handled
 * natively by KeySingle
 */
class CompiledKeyDirection: public CompiledKey {
public:
    CompiledKeyDirection(const Ast::KeyModifier & ast, CompiledKey * key):
    key(key){
    }
    
    virtual bool operator==(const CompiledKey & key) const {
        return key.same(*this);
    }
    
    virtual std::string toString() const {
        ostringstream out;
        out << "$" << key->toString();
        return out.str();
    }

    bool pressed(const Input & keys, const Input & oldKeys, int & holdKey, const CompiledKey *& holder, const CompiledKey*& needRelease) const {
        return false;
    }

    virtual ~CompiledKeyDirection(){
        delete key;
    }

protected:
    CompiledKey * key;
};

class CompiledKeyOnly: public CompiledKey {
public:
    CompiledKeyOnly(const Ast::KeyModifier & ast, CompiledKey * key):
    key(key){
    }
        
    bool sameKeys(const Input & map1, const Input & map2 ) const {
        return map1 == map2;
    }
    
    virtual std::string toString() const {
        ostringstream out;
        out << ">" << key->toString();
        return out.str();
    }
    
    virtual bool operator==(const CompiledKey & key) const {
        return key.same(*this);
    }

    bool pressed(const Input & keys, const Input & oldKeys, int & holdKey, const CompiledKey *& holder, const CompiledKey*& needRelease) const {
        if (!key->pressed(keys, oldKeys, holdKey, holder, needRelease)){
            if (!sameKeys(keys, oldKeys)){
                throw Command::Exception();
            }
            return false;
        } else {
            return true;
        }
    }

    virtual ~CompiledKeyOnly(){
        delete key;
    }

protected:
    CompiledKey * key;
};

Command::Exception::Exception():
MugenException("Command", __FILE__, __LINE__){
}

Command::Exception::~Exception() throw () {
}

static CompiledKey* compile(const Ast::Key * key){
    class Walker: public Ast::Walker {
    public:
        Walker():
        key(NULL){
        }

        CompiledKey* key;

        CompiledKey* compile(const Ast::KeySingle & key){
            return new CompiledKeySingle(key);
        }
        
        CompiledKey* compile(const Ast::KeyModifier & key){
            switch (key.getModifierType()){
                case Ast::KeyModifier::MustBeHeldDown: return new CompiledKeyMustBeHeldDown(key, Mugen::compile(key.getKey()));
                case Ast::KeyModifier::Release: return new CompiledKeyRelease(key, Mugen::compile(key.getKey()));
                // case Ast::KeyModifier::Direction: return new CompiledKeyDirection(key, Mugen::compile(key.getKey()));
                case Ast::KeyModifier::Direction: return Mugen::compile(key.getKey());
                case Ast::KeyModifier::Only: return new CompiledKeyOnly(key, Mugen::compile(key.getKey()));
            }
            throw Command::Exception();
        }
        
        CompiledKey* compile(const Ast::KeyCombined & key){
            return new CompiledKeyCombined(Mugen::compile(key.getKey1()), Mugen::compile(key.getKey2()));
        }

        virtual void onKeySingle(const Ast::KeySingle & key){
            this->key = compile(key);
        }
        
        virtual void onKeyModifier(const Ast::KeyModifier & key){
            this->key = compile(key);
        }
        
        virtual void onKeyCombined(const Ast::KeyCombined & key){
            this->key = compile(key);
        }
    };

    Walker walk;
    key->walk(walk);
    return walk.key;
}

static vector<CompiledKey*> compile(Ast::KeyList * keys){
    vector<CompiledKey*> compiled;

    for (vector<Ast::Key*>::const_iterator it = keys->getKeys().begin(); it != keys->getKeys().end(); it++){
        compiled.push_back(compile(*it));
    }
    
    return compiled;
}

Command::Command(string name, Ast::KeyList * keys, int maxTime, int bufferTime):
name(name),
keys(keys),
compiledKeys(compile(keys)),
maxTime(maxTime),
bufferTime(bufferTime),
ticks(-1),
holdKey(-1),
current(compiledKeys.begin()),
holder(NULL),
successTime(0),
needRelease(NULL){
}

/* AST walking way, purely interpreted
 * Not used anymore.
 */
bool Command::interpret(const Ast::Key * key, const Input & keys, const Input & oldKeys, int & holdKey, const Ast::Key *& holder, const Ast::Key *& needRelease){
    class KeyWalker: public Ast::Walker{
    public:
        KeyWalker(const Input & keys, const Input & oldKeys, int & holdKey, const Ast::Key *& holder, const Ast::Key *& needRelease):
        ok(false),
        holdKey(holdKey),
        keys(keys),
        oldKeys(keys),
        holder(holder),
        needRelease(needRelease){
        }
        
        bool ok;
        int & holdKey;
        const Input & keys;
        const Input & oldKeys;
        const Ast::Key *& holder;
        const Ast::Key *& needRelease;

        virtual void onKeySingle(const Ast::KeySingle & key){
            if (key == "a"){
                ok = keys.a;
            } else if (key == "b"){
                ok = keys.b;
            } else if (key == "c"){
                ok = keys.c;
            } else if (key == "x"){
                ok = keys.x;
            } else if (key == "y"){
                ok = keys.y;
            } else if (key == "z"){
                ok = keys.z;
            } else if (key == "B"){
                ok = keys.back;
            } else if (key == "DB"){
                ok = keys.back && keys.down;
            } else if (key == "D"){
                ok = keys.down;
            } else if (key == "DF"){
                ok = keys.forward && keys.down;
            } else if (key == "F"){
                ok = keys.forward;
            } else if (key == "UF"){
                ok = keys.forward && keys.up;
            } else if (key == "U"){
                ok = keys.up;
            } else if (key == "UB"){
                ok = keys.back && keys.up;
            } else if (key == "start"){
                ok = keys.start;
            }
            if (ok){
                needRelease = &key;
            }
        }

        bool sameKeys(const Input & map1, const Input & map2 ){
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
                                throw Exception();
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
                            throw Exception();
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

    KeyWalker walker(keys, oldKeys, holdKey, holder, needRelease);
    key->walk(walker);

    return walker.ok;
}

bool Command::handle(Input keys){

    if (compiledKeys.size() == 0){
        return false;
    }
    
    if (successTime > 0){
        successTime -= 1;
        Global::debug(1) << "Pressed " << name << endl;
        return true;
    }

    bool use = true;

    /*
    if (needRelease != NULL){
        Global::debug(0) << "Waiting for release: " << needRelease->toString() << endl;
    }
    */

    if (needRelease != NULL && *needRelease == *(*current)){
    // if (needRelease != NULL){
        const CompiledKey * fake;
        try{
            bool ok = needRelease->pressed(keys, oldKeys, holdKey, holder, fake);

            /*
               KeyWalker walker(keys, oldKeys, holdKey, holder, fake);
               needRelease->walk(walker);
               */
            // Global::debug(1) << "Waiting for key " << needRelease->toString() << " to be released: " << ok << endl;

            if (ok){
                /* if the key is still held down then don't continue */
                use = false;
            } else {
                /* otherwise the key is released so reset the release key */
                needRelease = NULL;
            }
        } catch (const Exception & ce){
            /* what to do here/ */
        }
    } else {
        needRelease = NULL;
    }

    bool fail = false;
    bool ok = false;

    if (use){
        /*
        KeyWalker walker(keys, oldKeys, holdKey, holder, needRelease);
        (*current)->walk(walker);
        */
        try{
            ok = (*current)->pressed(keys, oldKeys, holdKey, holder, needRelease);
        } catch (const Exception & ce){
            fail = true;
        }

        /*
        if (name == "a"){
            Global::debug(0) << "Tried move " << (*current)->toString() << " was " << ok << endl;
        }
        */
        if (holder != 0){
            const CompiledKey * fake;
            try{
                ok &= holder->pressed(keys, oldKeys, holdKey, holder, fake);
            } catch (const Exception & e){
                fail = true;
            }
            /*
            holder->walk(walker);
            ok &= walker.ok;
            fail |= walker.fail;
            */
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

    /* // For debugging
    if (name == "superPulo"){
        Global::debug(0) << "Command " << name << " at ticks " << ticks << " " << (*current)->toString() << endl;
    }
    */

    if (fail){
        current = this->compiledKeys.begin();
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
        if (current == this->compiledKeys.end()){
            /* success! */
            current = this->compiledKeys.begin();
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
    for (vector<CompiledKey*>::iterator it = compiledKeys.begin(); it != compiledKeys.end(); it++){
        delete (*it);
    }
}

}
