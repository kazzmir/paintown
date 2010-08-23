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

namespace Ast{

class Walker;

class Section: public Element {
public: 
    Section(const std::string * name):
    name(name){
        if (name == 0){
            std::cerr << "[" << __FILE__ << ": " << __LINE__ << "] Cannot create a section with an empty name" << std::endl;
            exit(-1);
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
    
    virtual Element * copy() const {
        Section * out = new Section(new std::string(getName()));
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
