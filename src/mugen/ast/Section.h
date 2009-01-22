#ifndef _SECTION_H_
#define _SECTION_H_

#include <stdio.h>
#include <map>
#include <list>
#include "Variable.h"
#include "Value.h"

class Section {
public: 
	Section() {
	}

	std::map<Variable *, std::list<Value *> > &getKeyValueMap() { return keyValueMap; }

	std::list<Value *> &get(Variable *variable) {
		return keyValueMap[variable];
	}

	void setName(char *n) { stringData = n; }
	const char *getName() { return stringData; }

	void debugExplain() {
		printf("[%s]\n", stringData);

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
	}

private:
	std::map<Variable *, std::list<Value *> > keyValueMap;
	char *stringData;
};

#endif
