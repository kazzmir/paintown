#ifndef _paintown_4e928a11be0e15dd1511a045d162c6c1
#define _paintown_4e928a11be0e15dd1511a045d162c6c1

#include <string>
#include <vector>

namespace Ast{
    class Value;
}

namespace Mugen{

class Character;

struct Range{
    Range():
    low(0), high(0){
    }

    Range(int low, int high):
    low(low), high(high){
    }

    int low, high;
};

struct RuntimeValue{
    enum Type{
        Invalid,
        Bool,
        String,
        Double,
        ListOfString,
        RangeType,
    };

    RuntimeValue():
    type(Invalid){
    }

    RuntimeValue(bool b):
    type(Bool),
    bool_value(b){
    }

    RuntimeValue(double d):
    type(Double),
    double_value(d){
    }

    RuntimeValue(int i):
    type(Double),
    double_value(i){
    }

    RuntimeValue(const std::string & str):
    type(String),
    string_value(str){
    }

    RuntimeValue(const std::vector<std::string> & strings):
    type(ListOfString),
    strings_value(strings){
    }

    RuntimeValue(int low, int high):
    type(RangeType),
    range(low, high){
    }

    inline bool isBool() const {
        return type == Bool;
    }
    
    inline bool isDouble() const {
        return type == Double;
    }
    
    inline bool isString() const {
        return type == String;
    }

    inline bool isRange() const {
        return type == RangeType;
    }

    inline bool getBoolValue() const {
        return bool_value;
    }
    
    inline std::string getStringValue() const {
        return string_value;
    }

    inline double getDoubleValue() const {
        return double_value;
    }

    inline int getRangeLow() const {
        return range.low;
    }
    
    inline int getRangeHigh() const {
        return range.high;
    }

    Type type;
    std::string string_value;
    bool bool_value;
    double double_value;
    std::vector<std::string> strings_value;
    Range range;
};

class Environment{
public:
    Environment(const Character & character, const std::vector<std::string> commands):
    character(character),
    commands(commands){
    }

    Environment(const Character & character):
    character(character){
    }

    Environment(const Environment & copy):
    character(copy.character),
    commands(copy.commands){
    }

    virtual inline const Character & getCharacter() const {
        return character;
    }

    virtual inline const std::vector<std::string> getCommands() const {
        return commands;
    }

protected:
    const Character & character;
    std::vector<std::string> commands;
};
    
RuntimeValue evaluate(const Ast::Value * value, const Environment & environment);

double toNumber(const RuntimeValue & value);
bool toBool(const RuntimeValue & value);

}

#endif
