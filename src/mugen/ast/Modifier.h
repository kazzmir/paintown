#ifndef _MODIFIER_H_
#define _MODIFIER_H_

class Modifier {
public:
	Modifier(const char m, const int c):
		modifierChar(m),
		count(c) {}

	const char getModifierChar() { return modifierChar; }
	const int getCount() { return count; }

private:
	char modifierChar;
	int count;
};

#endif
