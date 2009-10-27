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
#include "Value.h"

namespace Ast{

class Section{
public: 
    Section(const std::string * name):
    name(name){
        if (name == 0){
            std::cerr << "[" << __FILE__ << ": " << __LINE__ << "] Cannot create a section with an empty name" << std::endl;
            exit(-1);
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
