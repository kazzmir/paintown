#ifndef _MODIFIER_H_
#define _MODIFIER_H_

namespace Ast{

class Modifier {
public:
	Modifier(const char m, const int c):
		modifierChar(m),
		count(c) {}

	char getModifierChar() { return modifierChar; }
	int getCount() { return count; }

private:
	char modifierChar;
	int count;
};

}

#endif
