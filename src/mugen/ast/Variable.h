#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include <stdio.h>

class Variable {
public:
	Variable(const char *n):
		name(n),
		numberIndex(0),
		numberIndexFlag(false),
		stringIndex(0),
		stringIndexFlag(false) {
	}

	Variable(const char *n, const int index):
		name(n),
		numberIndex(index),
		numberIndexFlag(true),
		stringIndex(0),
		stringIndexFlag(false) {
	}

	Variable (const char *n, const char *index):
		name(n),
		numberIndex(0),
		numberIndexFlag(false),
		stringIndex(index),
		stringIndexFlag(true) {
	}

	const char *getName() { return name; }
	const int getNumberIndex() { return numberIndex; }
	const bool hasNumberIndex() { return numberIndexFlag; }
	const char *getStringIndex() { return stringIndex; }
	const bool hasStringIndex() { return stringIndexFlag; }

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

#endif
