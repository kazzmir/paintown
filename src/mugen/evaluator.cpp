#include "evaluator.h"
#include "mugen_exception.h"
#include "character.h"
#include "ast/all.h"
#include <math.h>

using namespace std;

namespace Mugen{

double toNumber(const RuntimeValue & value){
    if (value.isDouble()){
        return value.getDoubleValue();
    }
    throw MugenException("Not a number");
}

bool toBool(const RuntimeValue & value){
    if (value.isBool()){
        return value.getBoolValue();
    }
    throw MugenException("Not a bool");
}

/* a meta-circular evaluator! */
class Evaluator: public Ast::Walker {
public:
    Evaluator(const Environment & environment):
        environment(environment){
        }

    const Environment & environment;
    RuntimeValue result;

    /* value1 == value2 */
    RuntimeValue same(const RuntimeValue & value1, const RuntimeValue & value2){
        switch (value1.type){
            case RuntimeValue::ListOfString : {
                switch (value2.type){
                    case RuntimeValue::String : {
                        const vector<string> & strings = value1.strings_value;
                        for (vector<string>::const_iterator it = strings.begin(); it != strings.end(); it++){
                            const string & check = *it;
                            if (check == value2.string_value){
                                return RuntimeValue(true);
                            }
                        }
                    }
                }
            }
            case RuntimeValue::String : {
                switch (value2.type){
                    case RuntimeValue::ListOfString : {
                        return same(value2, value1);
                    }
                }
            }
            case RuntimeValue::Double : {
                switch (value2.type){
                    case RuntimeValue::Double : {
                        double epsilon = 0.0000001;
                        return RuntimeValue(fabs(value1.getDoubleValue() - value2.getDoubleValue()) < epsilon);
                    }
                }
            }
        }

        return RuntimeValue(false);
    }
   
    RuntimeValue evaluate(const Ast::Value * value){
        return Mugen::evaluate(value, environment);
    }

    RuntimeValue evalIdentifier(const Ast::Identifier & identifier){
        if (identifier == "command"){
            return RuntimeValue(environment.getCommands());
        }

        if (identifier == "anim"){
            return RuntimeValue(environment.getCharacter().getAnimation());
        }

        if (identifier == "animtime"){
            /* FIXME! */
            return RuntimeValue(0);
        }

        if (identifier == "time"){
            /* FIXME! */
            return RuntimeValue(0);
        }

        if (identifier == "velocity.walk.back.x"){
            return RuntimeValue(environment.getCharacter().getWalkBackX());
        }

        if (identifier == "velocity.walk.fwd.x"){
            return RuntimeValue(environment.getCharacter().getWalkForwardX());
        }

        ostringstream out;
        out << "Unknown identifier '" << identifier.toString() << "'";
        throw MugenException(out.str());
    }

    virtual void onIdenfitier(const Ast::Identifier & identifier){
        result = evalIdentifier(identifier);
    }

    RuntimeValue evalKeyword(const Ast::Keyword & keyword){
        if (keyword == "vel x"){
            return RuntimeValue(environment.getCharacter().getXVelocity());
        }
        if (keyword == "vel y"){
            return RuntimeValue(environment.getCharacter().getYVelocity());
        }
        return RuntimeValue();
    }

    virtual void onKeyword(const Ast::Keyword & keyword){
        result = evalKeyword(keyword);
    }

    RuntimeValue evalString(const Ast::String & string_value){
        string out;
        string_value >> out;
        return RuntimeValue(out);
    }

    virtual void onString(const Ast::String & string){
        result = evalString(string);
    }

    RuntimeValue evalFunction(const Ast::Function & function){
        if (function == "const"){
            return evaluate(function.getArg1());
        }

        if (function == "abs"){
            return RuntimeValue(fabs(toNumber(evaluate(function.getArg1()))));
        }

        return RuntimeValue();
    }

    virtual void onFunction(const Ast::Function & string){
        result = evalFunction(string);
    }

    RuntimeValue evalNumber(const Ast::Number & number){
        double x;
        number >> x;
        return RuntimeValue(x);
    }

    virtual void onNumber(const Ast::Number & number){
        result = evalNumber(number);
    }

    virtual RuntimeValue evalExpressionInfix(const Ast::ExpressionInfix & expression){
        Global::debug(1) << "Evaluate expression " << expression.toString() << endl;
        using namespace Ast;
        switch (expression.getExpressionType()){
            case ExpressionInfix::Or : {
                return RuntimeValue(toBool(evaluate(expression.getLeft())) ||
                                    toBool(evaluate(expression.getRight())));
            }
            case ExpressionInfix::XOr : {
                return RuntimeValue(toBool(evaluate(expression.getLeft())) ^
                                    toBool(evaluate(expression.getRight())));
            }
            case ExpressionInfix::And : {
                return RuntimeValue(toBool(evaluate(expression.getLeft())) &&
                                    toBool(evaluate(expression.getRight())));
            }
            case ExpressionInfix::BitwiseOr : {
                return RuntimeValue((int)toNumber(evaluate(expression.getLeft())) |
                                    (int)toNumber(evaluate(expression.getRight())));
            }
            case ExpressionInfix::BitwiseXOr : {
                return RuntimeValue((int)toNumber(evaluate(expression.getLeft())) ^
                                    (int)toNumber(evaluate(expression.getRight())));
            }
            case ExpressionInfix::BitwiseAnd : {
                return RuntimeValue((int)toNumber(evaluate(expression.getLeft())) &
                                    (int)toNumber(evaluate(expression.getRight())));
            }
            case ExpressionInfix::Assignment : {
                break;
            }
            case ExpressionInfix::Equals : {
                return same(evaluate(expression.getLeft()), evaluate(expression.getRight()));
                break;
            }
            case ExpressionInfix::Unequals : {
                return RuntimeValue(!toBool(same(evaluate(expression.getLeft()), evaluate(expression.getRight()))));
            }
            case ExpressionInfix::GreaterThanEquals : {
                return RuntimeValue(toNumber(evaluate(expression.getLeft())) >= toNumber(evaluate(expression.getRight())));
            }
            case ExpressionInfix::GreaterThan : {
                return RuntimeValue(toNumber(evaluate(expression.getLeft())) > toNumber(evaluate(expression.getRight())));
            }
            case ExpressionInfix::LessThanEquals : {
                return RuntimeValue(toNumber(evaluate(expression.getLeft())) <= toNumber(evaluate(expression.getRight())));
            }
            case ExpressionInfix::LessThan : {
                return RuntimeValue(toNumber(evaluate(expression.getLeft())) < toNumber(evaluate(expression.getRight())));
            }
            case ExpressionInfix::Add : {
                return RuntimeValue(toNumber(evaluate(expression.getLeft())) + toNumber(evaluate(expression.getRight())));
            }
            case ExpressionInfix::Subtract : {
                return RuntimeValue(toNumber(evaluate(expression.getLeft())) - toNumber(evaluate(expression.getRight())));
            }
            case ExpressionInfix::Multiply : {
                return RuntimeValue(toNumber(evaluate(expression.getLeft())) * toNumber(evaluate(expression.getRight())));
            }
            case ExpressionInfix::Divide : {
                /* FIXME: catch divide by 0 */
                return RuntimeValue(toNumber(evaluate(expression.getLeft())) / toNumber(evaluate(expression.getRight())));
            }
            case ExpressionInfix::Modulo : {
                return RuntimeValue((int)toNumber(evaluate(expression.getLeft())) % (int)toNumber(evaluate(expression.getRight())));
            }
            case ExpressionInfix::Power : {
                return RuntimeValue(pow(toNumber(evaluate(expression.getLeft())), toNumber(evaluate(expression.getRight()))));
            }
        }

        throw MugenException("Unknown expression");
    }

    virtual void onExpressionInfix(const Ast::ExpressionInfix & expression){
        result = evalExpressionInfix(expression);
    }

    RuntimeValue evalExpressionUnary(const Ast::ExpressionUnary & expression){
        switch (expression.getExpressionType()){
            case Ast::ExpressionUnary::Not : {
                return RuntimeValue(!toBool(evaluate(expression.getExpression())));
            }
            case Ast::ExpressionUnary::Minus : {
                return RuntimeValue(-toNumber(evaluate(expression.getExpression())));
            }
            case Ast::ExpressionUnary::Negation : {
                return RuntimeValue(~(int)toNumber(evaluate(expression.getExpression())));
            }
        }
        throw MugenException("Unknown expression");
    }
    
    virtual void onExpressionUnary(const Ast::ExpressionUnary & expression){
        result = evalExpressionUnary(expression);
    }
};

RuntimeValue evaluate(const Ast::Value * value, const Environment & environment){
    try{
        Evaluator eval(environment);
        value->walk(eval);
        return eval.result;
    } catch (const MugenException & e){
        ostringstream out;
        out << "Error while evaluating expression " << value->toString() << ": " << e.getReason();
        throw MugenException(out.str());
    }
}

}
