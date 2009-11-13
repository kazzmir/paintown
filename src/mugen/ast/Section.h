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

namespace Ast{

class Walker;

class Section{
public: 
    Section(const std::string * name):
    name(name){
        if (name == 0){
            std::cerr << "[" << __FILE__ << ": " << __LINE__ << "] Cannot create a section with an empty name" << std::endl;
            exit(-1);
        }
    }

    virtual void walk(Walker & walker){
        walker.onSection(*this);
        for (std::list<Attribute*>::iterator it = attributes.begin(); it != attributes.end(); it++){
            Attribute * attribute = *it;
            attribute->walk(walker);
        }

        for (std::list<Value*>::iterator it = values.begin(); it != values.end(); it++){
            Value * value = *it;
            value->walk(walker);
        }
    }

    std::string getName() const {
        return *name;
    }

    void addAttribute(Attribute * attribute){
        attributes.push_back(attribute);
    }

    const std::list<Attribute *> & getAttributes() const {
        return attributes;
    }

    void addValue(Value * value){
        values.push_back(value);
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
};

}

#endif
