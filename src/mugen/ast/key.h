#ifndef _paintown_497caab0341b35e92742cc75b287ff0b
#define _paintown_497caab0341b35e92742cc75b287ff0b

#include "ast.h"
#include <map>
#include <vector>

namespace Ast{

class Key: public Element {
public:
    Key(){
    }

    virtual ~Key(){
    }
};

class KeySingle: public Key {
public:
    KeySingle(const char * name):
        Key(){
        }
    
    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
    }

    virtual ~KeySingle(){
    }
};

class KeyModifier: public Key {
public:
    enum ModifierType{
        /* slash (/) - means the key must be held down
         * egs. command = /D       ;hold the down direction
         * command = /DB, a   ;hold down-back while you press a
         */
        MustBeHeldDown,

        /*  tilde (~) - to detect key releases
         *  egs. command = ~a       ;release the a button
         *  command = ~D, F, a ;release down, press fwd, then a
         *  If you want to detect "charge moves", you can specify
         *  the time the key must be held down for (in game-ticks)
         *  egs. command = ~30a     ;hold a for at least 30 ticks, then release
         */
        Release,

        /*  dollar ($) - Direction-only: detect as 4-way
         *  egs. command = $D       ;will detect if D, DB or DF is held
         *  command = $B       ;will detect if B, DB or UB is held
         */
        Direction,

        /* greater-than (>) - means there must be no other keys pressed or released
         * between the previous and the current symbol.
         * egs. command = a, >~a   ;press a and release it without having hit
         * or released any other keys in between
         */
        Only,
    };

    KeyModifier(ModifierType type, const Key * key, int extra = 0):
        Key(),
        type(type),
        key(key),
        extra(0){
        }

    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        key->mark(marks);
    }

    virtual ~KeyModifier(){
        delete key;
    }

protected:
    const ModifierType type;
    const Key * key;
    const int extra;
};

class KeyCombined: public Key {
public:
    KeyCombined(const Key * key1, const Key * key2):
    Key(),
    key1(key1),
    key2(key2){
    }

    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        key1->mark(marks);
        key2->mark(marks);
    }

    virtual ~KeyCombined(){
        delete key1;
        delete key2;
    }

protected:
    const Key * key1;
    const Key * key2;
};

class KeyList: public Key {
public:
    KeyList(const std::vector<Key*> & keys):
        Key(),
        keys(keys){
        }

    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        for (std::vector<Key*>::const_iterator it = keys.begin(); it != keys.end(); it++){
            Key * key = (Key *) *it;
            key->mark(marks);
        }
    }

    virtual ~KeyList(){
        for (std::vector<Key*>::const_iterator it = keys.begin(); it != keys.end(); it++){
            delete (*it);
        }
    }

protected:
    const std::vector<Key*> keys;
};

}

#endif
