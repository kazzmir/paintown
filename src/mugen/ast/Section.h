#ifndef _paintown_section_h_
#define _paintown_section_h_

#include <stdio.h>
#include <map>
#include <list>
#include <string>
#include <iostream>
#include <string>
#include <stdlib.h>
#include "attribute.h"
#include "attribute-simple.h"
#include "Value.h"
#include "exception.h"
#include "ast.h"
#include "util/token.h"

namespace Ast{

class Walker;

class Section: public Element {
public: 
    static std::string SERIAL_SECTION_ATTRIBUTE;
    static std::string SERIAL_SECTION_VALUE;

    Section(const std::string * name):
    Element(-1, -1),
    name(name){
        if (name == 0){
            throw Exception("Cannot create a section with an empty name");
        }
    }

    Section(const std::string * name, int line, int column):
    Element(line, column),
    name(name){
        if (name == 0){
            throw Exception("Cannot create a section with an empty name");
        }
    }

    enum WalkList{
        WalkAttribute,
        WalkValue,
    };

    virtual void walk(Walker & walker){
        walker.onSection(*this);
        std::list<Attribute*>::iterator attribute_it = attributes.begin();
        std::list<Value*>::iterator value_it = values.begin();

        /* walk values in the order they came in. hopefully the number of values in the
         * walkList are the same number of items in the attributes+values lists.
         * I mean this should be the case since the only operations done to those lists
         * is adding elements. No one ever removes them.
         */
        for (std::list<WalkList>::iterator it = walkList.begin(); it != walkList.end(); it++){
            WalkList what = *it;
            switch (what){
                case WalkAttribute : {
                    Attribute * attribute = *attribute_it;
                    attribute->walk(walker);
                    attribute_it++;
                    break;
                }
                case WalkValue : {
                    Value * value = *value_it;
                    value->walk(walker);
                    value_it++;
                    break;
                }
            }
        }

        /*
        for (std::list<Attribute*>::iterator it = attributes.begin(); it != attributes.end(); it++){
            Attribute * attribute = *it;
            attribute->walk(walker);
        }

        for (std::list<Value*>::iterator it = values.begin(); it != values.end(); it++){
            Value * value = *it;
            value->walk(walker);
        }
        */
        walker.onSectionFinished(*this);
    }

    std::string getName() const {
        return *name;
    }

    void addAttribute(Attribute * attribute){
        attributes.push_back(attribute);
        walkList.push_back(WalkAttribute);
    }

    template <class Thing> static bool checkEquality(const std::list<Thing*> & my_list, const std::list<Thing*> & him_list){
        typename std::list<Thing*>::const_iterator my_it = my_list.begin();
        typename std::list<Thing*>::const_iterator him_it = him_list.begin();
        while (true){
            if (my_it == my_list.end() || him_it == him_list.end()){
                break;
            }
            Thing * mine = *my_it;
            Thing * him = *him_it;
            if (*mine != *him){
                return false;
            }

            my_it++;
            him_it++;
        }

        return my_it == my_list.end() && him_it == him_list.end();
    }

    using Element::operator==;
    virtual bool operator==(const Section & him) const {
        if (this == &him){
            return true;
        }

        return checkEquality(attributes, him.attributes) &&
               checkEquality(values, him.values);
    }
    
    virtual Element * copy() const {
        Section * out = new Section(new std::string(getName()), getLine(), getColumn());
        out->walkList = walkList;
        for (std::list<Attribute*>::const_iterator attribute_it = attributes.begin(); attribute_it != attributes.end(); attribute_it++){
            out->attributes.push_back((Attribute*) (*attribute_it)->copy());
        }

        for (std::list<Value*>::const_iterator value_it = values.begin(); value_it != values.end(); value_it++){
            out->values.push_back((Value*) (*value_it)->copy());
        }

        return out;
    }

    const std::list<Attribute *> & getAttributes() const {
        return attributes;
    }

    void addValue(Value * value){
        values.push_back(value);
        walkList.push_back(WalkValue);
    }

    virtual Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_SECTION_LIST << getName() << getLine() << getColumn();
        std::list<Attribute*>::const_iterator attribute_it = attributes.begin();
        std::list<Value*>::const_iterator value_it = values.begin();
        for (std::list<WalkList>::const_iterator it = walkList.begin(); it != walkList.end(); it++){
            WalkList what = *it;
            switch (what){
                case WalkAttribute : {
                    Attribute * attribute = *attribute_it;
                    Token * next = token->newToken();
                    *next << SERIAL_SECTION_ATTRIBUTE << attribute->serialize();
                    attribute_it++;
                    break;
                }
                case WalkValue : {
                    Value * value = *value_it;
                    Token * next = token->newToken();
                    *next << SERIAL_SECTION_VALUE << value->serialize();
                    value_it++;
                    break;
                }
            }
        }

        return token;
    }

    static Section * deserialize(const Token * token){
        const Token * next;
        std::string name;
        int line, column;
        TokenView view = token->view();
        view >> name >> line >> column;
        Section * section = new Section(new std::string(name), line, column);
        while (view.hasMore()){
            view >> next;
            if (*next == SERIAL_SECTION_ATTRIBUTE){
                const Token * attribute;
                next->view() >> attribute;
                section->addAttribute(Attribute::deserialize(attribute));
            } else if (*next == SERIAL_SECTION_VALUE){
                const Token * value;
                next->view() >> value;
                section->addValue(Value::deserialize(value));
            } else {
                throw Exception("Can't deserialize " + next->getName());
            }
        }

        return section;
    }
    
    std::string toString(){
        std::ostringstream out;

        out << "[" << *name << "]" << std::endl;
        
        std::list<Attribute*>::iterator attribute_it = attributes.begin();
        std::list<Value*>::iterator value_it = values.begin();
        for (std::list<WalkList>::iterator it = walkList.begin(); it != walkList.end(); it++){
            WalkList what = *it;
            switch (what){
                case WalkAttribute : {
                    Attribute * attribute = *attribute_it;
                    out << attribute->toString() << std::endl;
                    attribute_it++;
                    break;
                }
                case WalkValue : {
                    Value * value = *value_it;
                    out << value->toString() << std::endl;
                    value_it++;
                    break;
                }
            }
        }

        return out.str();
    }

    void debugExplain() {
        // printf("[%s]\n", stringData.c_str());
        std::cout << "[" << *name << "]" << std::endl;

        for (std::list<Attribute*>::iterator it = attributes.begin(); it != attributes.end(); it++){
            Attribute * attribute = *it;
            attribute->debugExplain();
        }
        
        for (std::list<Value*>::iterator it = values.begin(); it != values.end(); it++){
            Value * value = *it;
            value->debugExplain();
        }
    }

    virtual AttributeSimple * findAttribute(const std::string & find) const {
        for (std::list<Attribute*>::const_iterator attribute_it = getAttributes().begin(); attribute_it != getAttributes().end(); attribute_it++){
            Attribute * attribute = *attribute_it;
            if (attribute->getKind() == Attribute::Simple){
                AttributeSimple * simple = (AttributeSimple*) attribute;
                if (*simple == find){
                    return simple;
                }
            }
        }

        throw Exception("Could not find attribute " + find + " in section " + getName());
    }

    /* mark phase of garbage collection. all live pointers are marked 'true' */
    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        marks[name] = true;
        for (std::list<Attribute*>::const_iterator it = attributes.begin(); it != attributes.end(); it++){
            Attribute * attribute = *it;
            attribute->mark(marks);
        }
        for (std::list<Value*>::const_iterator it = values.begin(); it != values.end(); it++){
            Value * value = *it;
            value->mark(marks);
        }
    }

    /*
    virtual bool referenced(const void * value) const {
        if (value == this){
            return true;
        }

        if (value == name){
            return true;
        }

        for (std::list<Attribute*>::const_iterator it = attributes.begin(); it != attributes.end(); it++){
            Attribute * attribute = *it;
            if (attribute->referenced(value)){
                return true;
            }
        }
        for (std::list<Value*>::const_iterator it = values.begin(); it != values.end(); it++){
            Value * v = *it;
            if (v->referenced(value)){
                return true;
            }
        }
        return false;
    }
    */

    virtual ~Section(){
        delete name;
        for (std::list<Attribute*>::iterator it = attributes.begin(); it != attributes.end(); it++){
            delete *it;
        }
        for (std::list<Value*>::iterator it = values.begin(); it != values.end(); it++){
            delete *it;
        }
    }

private:
    const std::string * name;
    std::list<Attribute *> attributes;
    std::list<Value *> values;
    std::list<WalkList> walkList;
};

}

#endif
