#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include <stdio.h>

namespace Ast{

class Variable {
public:
	Variable(const char *n):
		name(n),
		numberIndex(0),
		stringIndex(0),
		numberIndexFlag(false),
		stringIndexFlag(false) {
	}

	Variable(const char *n, const int index):
		name(n),
		numberIndex(index),
		stringIndex(0),
		numberIndexFlag(true),
		stringIndexFlag(false) {
	}

	Variable (const char *n, const char *index):
		name(n),
		numberIndex(0),
		stringIndex(index),
		numberIndexFlag(false),
		stringIndexFlag(true) {
	}

	const char *getName() { return name; }
	int getNumberIndex() { return numberIndex; }
	bool hasNumberIndex() { return numberIndexFlag; }
	const char *getStringIndex() { return stringIndex; }
	bool hasStringIndex() { return stringIndexFlag; }

	void debugExplain() {
		printf("%s", name);
		if (numberIndexFlag)
			printf("(%d)", numberIndex);
		if (stringIndexFlag)
			printf("(%s)", stringIndex);
	}
private:
	const char *name;
	int numberIndex;
	const char *stringIndex;
	bool numberIndexFlag;
	bool stringIndexFlag;
};

}

#endif
