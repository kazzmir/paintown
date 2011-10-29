#ifndef _paintown_c0b3d0dc8f1134d72afa82e1d3e9d3f0
#define _paintown_c0b3d0dc8f1134d72afa82e1d3e9d3f0

#include "Value.h"

namespace Ast{

class Range: public Value {
public:
    enum RangeType{
        AllInclusive,
        AllExclusive,
        LeftInclusiveRightExclusive,
        LeftExclusiveRightInclusive,
    };

    Range(int line, int column, RangeType type, const Value * low, const Value * high):
    Value(line, column),
    type(type),
    low(low),
    high(high){
    }
    
    virtual void walk(Walker & walker) const {
        walker.onRange(*this);
    }

    virtual std::string toString(char left, char right) const {
        std::ostringstream out;
        out << left << low->toString() << ", " << high->toString() << right;
        return out.str();
    }

    virtual RangeType getRangeType() const {
        return type;
    }
    
    virtual std::string toString() const {
        switch (type){
            case AllInclusive : return toString('[', ']');
            case AllExclusive : return toString('(', ')');
            case LeftInclusiveRightExclusive : return toString('[', ')');
            case LeftExclusiveRightInclusive : return toString('(', ']');
            default : return "";
        }
    }

    virtual const Value * getLow() const {
        return low;
    }

    virtual const Value * getHigh() const {
        return high;
    }

    using Element::operator==;
    virtual bool operator==(const Value & him) const {
        return him == *this;
    }

    virtual bool operator==(const Range & him) const {
        return getRangeType() == him.getRangeType() &&
               *getLow() == *him.getLow() &&
               *getHigh() == *him.getHigh();
    }
    
    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_RANGE << getLine() << getColumn() << getRangeType() << getLow()->serialize() << getHigh()->serialize();
        return token;
    }

    static Range * deserialize(const Token * token){
        int type;
        int line, column;
        const Token * low;
        const Token * high;
        token->view() >> line >> column >> type >> low >> high;
        return new Range(line, column, RangeType(type), Value::deserialize(low), Value::deserialize(high));
    }

    class RangeView: public ViewImplementation {
    public:
        RangeView(const Range * owner):
        owner(owner){
        }

        const Range * owner;

        virtual std::string getType() const {
            return owner->getType();
        }
        
        virtual const Value * self() const {
            return owner;
        }
        
        virtual std::string toString() const {
            return owner->toString();
        }
    };

    virtual View view() const {
        return View(Util::ReferenceCount<ViewImplementation>(new RangeView(this)));
    }

    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        low->mark(marks);
        high->mark(marks);
    }

    virtual Element * copy() const {
        return new Range(getLine(), getColumn(), type, (Value*) low->copy(), (Value*) high->copy());
    }
    
    virtual std::string getType() const {
        return "range";
    }
    
    virtual ~Range(){
        delete low;
        delete high;
    }

protected:
    RangeType type;
    const Value * low;
    const Value * high;
};

}

#endif
