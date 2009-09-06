#ifndef _paintown_section_h_
#define _paintown_section_h_

#include <stdio.h>
#include <map>
#include <list>
#include <string>
#include <iostream>
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

    void addAttribute(Attribute * attribute){
        attributes.push_back(attribute);
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

    ~Section(){
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
