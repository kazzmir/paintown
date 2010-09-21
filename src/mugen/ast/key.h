#ifndef _paintown_497caab0341b35e92742cc75b287ff0b
#define _paintown_497caab0341b35e92742cc75b287ff0b

#include "ast.h"
#include "walker.h"
#include <map>
#include <vector>
#include "util/token.h"

namespace Ast{

class KeySingle;

class Key: public Value {
public:
    Key(){
    }

    virtual std::string getType() const {
        return "key";
    }

    using Element::operator==;
    virtual bool operator==(const Value & him) const {
        return him == *this;
    }

    /*
    virtual bool same(const Key & key) const {
        return false;
    }

    virtual bool same(const KeySingle & key) const {
        return false;
    }
    */

    /*
    using Element::operator==;
    virtual bool operator==(const Key & key) const {
        return false;
    }
    */

    virtual ~Key(){
    }
};

class KeySingle: public Key {
public:
    explicit KeySingle(const char * name):
        Key(),
        name(name){
        }
    
    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
    }

    using Element::operator==;
    virtual bool operator==(const std::string & that) const {
        return name == that;
    }

    virtual bool operator==(const KeySingle & key) const {
        return name == key.name;
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_KEY_SINGLE << name;
        return token;
    }

    static KeySingle * deserialize(Token * token){
        std::string what;
        *token >> what;
        return new KeySingle(what.c_str());
    }

    virtual bool operator==(const Key & key) const {
        return key == *this;
    }
    
    virtual void walk(Walker & walker) const {
        walker.onKeySingle(*this);
    }
    
    virtual std::string toString() const {
        return name;
    }
    
    virtual Element * copy() const {
        return new KeySingle(name.c_str());
    }

    virtual ~KeySingle(){
    }

protected:
    std::string name;
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
        extra(extra){
        }
    
    virtual void walk(Walker & walker) const {
        walker.onKeyModifier(*this);
    }

    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        key->mark(marks);
    }

    virtual ModifierType getModifierType() const {
        return type;
    }

    virtual inline int getExtra() const {
        return extra;
    }

    virtual const Key * getKey() const {
        return key;
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_KEY_MODIFIER << getModifierType() << getExtra() << getKey()->serialize();
        return token;
    }

    static KeyModifier * deserialize(Token * token){
        int type = 0;
        int extra = 0;
        Token * next;
        *token >> type >> extra >> next;
        return new KeyModifier(ModifierType(type), (Key*) Value::deserialize(next), extra);
    }

    using Element::operator==;
    virtual bool operator==(const Key & key) const {
        return key == *this;
        // return key.same(*this);
    }
    
    virtual bool operator==(const KeyModifier & him) const {
        return getModifierType() == him.getModifierType() &&
               getExtra() == him.getExtra() &&
               *getKey() == *him.getKey();
    }

    virtual std::string toString() const {
        std::ostringstream out;
        out << modifierName(type);
        if (type == Release && extra != 0){
            out << extra;
        }
        out << key->toString();
        return out.str();
    }

    const char * modifierName(ModifierType type) const {
        switch (type){
            case MustBeHeldDown : return "/";
            case Release : return "~";
            case Direction : return "$";
            case Only : return ">";
            default : return "?";
        }
    }

    virtual Element * copy() const {
        return new KeyModifier(type, (Key*) key->copy(), extra);
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
    
    virtual void walk(Walker & walker) const {
        walker.onKeyCombined(*this);
    }

    virtual const Key * getKey1() const {
        return key1;
    }
    
    virtual const Key * getKey2() const {
        return key2;
    }

    using Element::operator==;
    virtual bool operator==(const Key & key) const {
        return key == *this;
        // return key.same(*this);
    }

    virtual bool operator==(const KeyCombined & him) const {
        return *getKey1() == *him.getKey1() &&
               *getKey2() == *him.getKey2();
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_KEY_COMBINED << key1->serialize() << key2->serialize();
        return token;
    }

    static KeyCombined * deserialize(Token * token){
        Token * left;
        Token * right;
        *token >> left >> right;
        return new KeyCombined((Key*) Value::deserialize(left),
                               (Key*) Value::deserialize(right));
    }
    
    virtual std::string toString() const {
        std::ostringstream out;
        out << key1->toString() << "+" << key2->toString();
        return out.str();
    }

    virtual Element * copy() const {
        return new KeyCombined((Key*) key1->copy(), (Key*) key2->copy());
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

    using Element::operator==;
    virtual bool operator==(const Key & key) const {
        return key == *this;
        // return key.same(*this);
    }

    virtual bool operator==(const KeyList & him) const {
        std::vector<Key*>::const_iterator my_it = keys.begin();
        std::vector<Key*>::const_iterator him_it = him.keys.begin();
        while (true){
            if (my_it == keys.end() || him_it == him.keys.end()){
                break;
            }

            if (**my_it != **him_it){
                return false;
            }

            my_it++;
            him_it++;
        }

        return my_it == keys.end() && him_it == him.keys.end();
    }

    virtual Element * copy() const {
        std::vector<Key*> copied;
        for (std::vector<Key*>::const_iterator it = keys.begin(); it != keys.end(); it++){
            copied.push_back((Key*) (*it)->copy());
        }
        return new KeyList(copied);
    }

    virtual const std::vector<Key*> & getKeys() const {
        return keys;
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_KEY_LIST;
        for (std::vector<Key*>::const_iterator it = keys.begin(); it != keys.end(); it++){
            Key * key = *it;
            *token << key->serialize();
        }
        return token;
    }

    static KeyList * deserialize(Token * token){
        std::vector<Key*> keys;
        try{
            while (true){
                Token * next;
                *token >> next;
                keys.push_back((Key*) Value::deserialize(next));
            }
        } catch (const TokenException & e){
        }
        return new KeyList(keys);
    }
    
    virtual std::string toString() const {
        std::ostringstream out;

        for (std::vector<Key*>::const_iterator it = keys.begin(); it != keys.end(); it++){
            if (it != keys.begin()){
                out << ", ";
            }
            out << (*it)->toString();
        }

        return out.str();
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
