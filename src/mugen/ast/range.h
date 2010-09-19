#ifndef _paintown_c0b3d0dc8f1134d72afa82e1d3e9d3f0
#define _paintown_c0b3d0dc8f1134d72afa82e1d3e9d3f0

namespace Ast{

class Range: public Value {
public:
    enum RangeType{
        AllInclusive,
        AllExclusive,
        LeftInclusiveRightExclusive,
        LeftExclusiveRightInclusive,
    };

    Range(RangeType type, const Value * low, const Value * high):
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
    
    Token * serialize() const {
        Token * token = new Token();
        *token << "range" << getRangeType() << low->serialize() << high->serialize();
        return token;
    }

    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        low->mark(marks);
        high->mark(marks);
    }

    virtual Element * copy() const {
        return new Range(type, (Value*) low->copy(), (Value*) high->copy());
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
