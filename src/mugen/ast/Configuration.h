#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include <vector>
#include <stdio.h>
#include "Section.h"

namespace Ast{

class Configuration {
public: // constructors and destructors
	Configuration() {
	}

	std::vector<Section *> &getSections() { return sections; }

	void debugExplain() {
		for (std::vector<Section *>::iterator iter = sections.begin();
			iter != sections.end();
			iter++) {
			(*iter)->debugExplain();
		}
	}	

private: // data

	std::vector<Section *> sections;
};

}

#endif
