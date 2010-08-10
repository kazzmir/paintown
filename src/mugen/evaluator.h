#ifndef _paintown_4e928a11be0e15dd1511a045d162c6c1
#define _paintown_4e928a11be0e15dd1511a045d162c6c1

#include <string>
#include <vector>

namespace Ast{
    class Value;
}

class MugenStage;

namespace Mugen{

namespace Compiler{
    class Value;
}

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
private:
    explicit RuntimeValue(Compiler::Value * value);
public:
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

    bool operator==(const RuntimeValue & other) const;

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

    double toNumber() const;
    bool toBool() const;

    const char * canonicalName() const {
        switch (type){
            case Invalid : return "invalid";
            case Bool : return "bool";
            case String : return "string";
            case Double : return "double";
            case ListOfString : return "list of string";
            case RangeType : return "range";
            default : return "???";
        }
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
    Environment(){
    }

    virtual const Character & getCharacter() const = 0;
    virtual const MugenStage & getStage() const = 0;
    virtual const std::vector<std::string> getCommands() const = 0;

    virtual ~Environment(){
    }
};

class EmptyEnvironment: public Environment {
public:
    EmptyEnvironment(){
    }

    virtual ~EmptyEnvironment(){
    }

    virtual const Character & getCharacter() const;
    virtual const MugenStage & getStage() const;
    virtual const std::vector<std::string> getCommands() const;
};

class FullEnvironment: public Environment {
public:
    FullEnvironment(const MugenStage & stage, const Character & character, const std::vector<std::string> commands):
    stage(stage),
    character(character),
    commands(commands){
    }

    FullEnvironment(const MugenStage & stage, const Character & character):
    stage(stage),
    character(character){
    }

    FullEnvironment(const FullEnvironment & copy):
    stage(copy.stage),
    character(copy.character),
    commands(copy.commands){
    }

    virtual ~FullEnvironment(){
    }

    virtual inline const Character & getCharacter() const {
        return character;
    }
    
    virtual inline const MugenStage & getStage() const {
	return stage;
    }

    virtual inline const std::vector<std::string> getCommands() const {
        return commands;
    }

protected:
    const MugenStage & stage;
    const Character & character;
    std::vector<std::string> commands;
};
    
RuntimeValue evaluate(const Ast::Value * value, const Environment & environment);

double toNumber(const RuntimeValue & value);
bool toBool(const RuntimeValue & value);

}

#endif
