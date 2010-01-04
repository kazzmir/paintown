#ifndef _paintown_4e928a11be0e15dd1511a045d162c6c1
#define _paintown_4e928a11be0e15dd1511a045d162c6c1

#include <string>
#include <vector>

namespace Ast{
    class Value;
}

namespace Mugen{

class Character;

struct RuntimeValue{
    enum Type{
        Invalid,
        Bool,
        String,
        Double,
        ListOfString,
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

    inline bool isBool() const {
        return type == Bool;
    }
    
    inline bool isDouble() const {
        return type == Double;
    }

    inline bool getBoolValue() const {
        return bool_value;
    }
    
    inline double getDoubleValue() const {
        return double_value;
    }

    Type type;
    std::string string_value;
    bool bool_value;
    double double_value;
    std::vector<std::string> strings_value;
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
