#ifndef _SECTION_H_
#define _SECTION_H_

#include <stdio.h>
#include <map>
#include <list>
#include <string>
#include <iostream>
#include "Variable.h"
#include "Value.h"

namespace Ast{

class Section{
public: 
    typedef std::list<Value*> ValueList;

    Section(){
    }

    Section(const std::string & name):
    stringData(name){
    }

    std::map<Variable *, std::list<Value *> > &getKeyValueMap() { return keyValueMap; }

    std::list<ValueList*> & getActions(){
        return actions;
    }

        std::list<Value *> &get(Variable *variable) {
            return keyValueMap[variable];
        }

    void setName(char *n){
        stringData = n;
    }

    void setName(const std::string & n){
        stringData = n;
    }

    const std::string & getName(){
        return stringData;
    }

    void debugExplain() {
        // printf("[%s]\n", stringData.c_str());
        std::cout << "[" << stringData << "]" << std::endl;

        std::map<Variable *, std::list<Value *> >::iterator iter;

        for (iter = keyValueMap.begin(); 
                iter != keyValueMap.end();
                iter++) {
            iter->first->debugExplain();
            printf(" = ");

            std::list<Value *>::iterator iter2;
            bool first = true;
            for (iter2 = iter->second.begin();
                    iter2 != iter->second.end();
                    iter2++) {
                if (!first) {
                    printf(", ");
                } else {
                    first = false;
                }
                if ((*iter2) != NULL) {
                    (*iter2)->debugExplain();
                }
            }
            printf("\n");
        }

        for (std::list<ValueList*>::iterator it = actions.begin(); it != actions.end(); it++){
            bool first = false;
            for (ValueList::iterator val = (*it)->begin(); val != (*it)->end(); val++){
                (*val)->debugExplain();
                printf(", ");
                /*
                if (!first){
                    printf(", ");
                } else {
                    first = false;
                }
                */
            }
            printf("\n");
        }

    }

private:
    std::map<Variable *, std::list<Value *> > keyValueMap;
    std::list<ValueList*> actions;
    std::string stringData;
};

}

#endif
