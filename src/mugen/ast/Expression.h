#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

class Value;

class Expression {
public:
	Expression(Value *lhs, char * o, Value *rhs):
		left(lhs),
		right(rhs),
		op(o){
	}

	Value *getLeft() { return left; }
	void setLeft(Value *v) { left = v; }

	Value *getRight() { return right; }
	void setRight(Value *v) { right = v; }

	char * getOperation() { return op; }	
	void setOperation(char * o) { op = o; }

private:
	Value *left;
	Value *right;
	char * op;
};

#endif
