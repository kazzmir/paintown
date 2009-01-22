#ifndef _VALUE_H_
#define _VALUE_H_

#include <list>
#include <stdio.h>
#include "Expression.h"
#include "Modifier.h"

enum ValueSpecies {
	SimpleNumber,
	SimpleString,
	SimpleIdentifier,
	ModifiedIdentifier,
	SimpleExpression,
	ParameterizedIdentifier,
	Range
};

class Value {
public:

	Value(const double &n):
		number(n),
		species(SimpleNumber) {
	}

	Value(const double &lo, const double &hi):
		number(lo),
		higherNumber(hi),
		species(Range) {
	}

	Value(const char *str):
		stringData(str),
		species(SimpleString) {
	}

	Value() {
	}

public:
	const double getNumber() const { return number; }
	void setNumber(const double &n) { number = n; }

	const double getLowNumber() const { return number; }
	void setLowNumber(const double &n) { number = n; }

	const double getHighNumber() const { return number; }
	void setHighNumber(const double &n) { higherNumber = n; }

	const char *getString() { return stringData; }
	const char *getIdentifier() { return stringData; }
	void setString(const char *str) { stringData = str; }
	void setIdentifier(const char *str) { stringData = str; }

	std::list<Modifier *> &getModifiers() { return modifiers; }

	Expression *getExpression() { return expression; }
	void setExpression(Expression *expr) { expression = expr; }

	std::list<Value *> &getParameters() { return parameters; }

	const ValueSpecies getSpecies() { return species; }
	void setSpecies(const ValueSpecies s) { species = s; }

	void debugExplain() {
		switch (species) {
			case SimpleNumber: printf("%g", number);
				break;
			case SimpleString: printf("%s", stringData);
				break;
			case Range: printf("<%g - %g>", number, higherNumber);
				break;
                        case ParameterizedIdentifier: {
                            break;
                        }
                        case ModifiedIdentifier: {
                            break;
                        }
                        case SimpleExpression: {
                            break;
                        }
                        case SimpleIdentifier: {
                            break;
                        }
		}
	}
private:
	double number;
	const char *stringData;
	std::list<Modifier *> modifiers;
	Expression *expression;
	std::list<Value *> parameters;
	double higherNumber; // in case of a range
	ValueSpecies species;
};

#endif
