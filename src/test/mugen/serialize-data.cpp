#include <exception>
#include <sstream>
#include "util/debug.h"
#include "util/token.h"
#include "mugen/common.h"
#include "mugen/compiler.h"
#include "mugen/serialize.h"

using namespace std;

class Fail: public exception {
public:
    Fail(const string & name):
    name(name){
    }

    virtual ~Fail() throw () {
    }

    string name;

    const string & getName() const {
        return name;
    }
};

static void testAttackTypeAttribute(){
    Mugen::AttackType::Attribute in = Mugen::AttackType::SpecialAttack;
    Token * save = Mugen::serialize(in);
    Mugen::AttackType::Attribute out = Mugen::deserializeAttackTypeAttribute(save);
    /*
    Global::debug(0) << "In " << in << std::endl;
    Global::debug(0) << "Token " << save->toString() << std::endl;
    Global::debug(0) << "Out " << out << std::endl;
    */

    delete save;
    if (in != out){
        throw Fail("testAttackTypeAttribute");
    }
}

static void testAttackTypeAnimation(){
    Mugen::AttackType::Animation in = Mugen::AttackType::Hard;
    Token * save = Mugen::serialize(in);
    Mugen::AttackType::Animation out = Mugen::deserializeAttackTypeAnimation(save);

    delete save;
    if (in != out){
        throw Fail("testAttackTypeAnimation");
    }
}

static void testAttackTypeGround(){
    Mugen::AttackType::Ground in = Mugen::AttackType::Low;
    Token * save = Mugen::serialize(in);
    Mugen::AttackType::Ground out = Mugen::deserializeAttackTypeGround(save);

    delete save;
    if (in != out){
        throw Fail("testAttackTypeGround");
    }
}

static void testTransType(){
    Mugen::TransType in = Mugen::Sub;
    Token * save = Mugen::serialize(in);
    Mugen::TransType out = Mugen::deserializeTransType(save);

    delete save;
    if (in != out){
        throw Fail("testTransType");
    }
}

static void testCharacterId(){
    Mugen::CharacterId in(2384);
    Token * save = Mugen::serialize(in);
    Mugen::CharacterId out = Mugen::deserializeCharacterId(save);

    delete save;
    if (in != out){
        throw Fail("testCharacterId");
    }
}

static void testRuntimeValue(){
    try{
        Mugen::RuntimeValue t1(true);
        Token * t1_token = Mugen::serialize(t1);

        Mugen::RuntimeValue t2;
        Mugen::deserialize_RuntimeValue(t2, t1_token);

        delete t1_token;
        if (t1 != t2){
            throw Fail("testRuntimeValue");
        }
    } catch (const TokenException & fail){
        ostringstream out;
        out << "testRuntimeValue: " << fail.getTrace();
        throw Fail(out.str());
    }
}

int main(int argc, char ** argv){
    try{
        testAttackTypeAttribute();
        testAttackTypeAnimation();
        testAttackTypeGround();
        testTransType();
        testCharacterId();
        testRuntimeValue();

        /*
Token * serialize(const std::vector<CharacterId> &);
Token * serialize(const std::string &);
Token * serialize(const Physics::Type);
Token * serialize(const Facing);
Token * serialize(int);
Token * serialize(const Graphics::Color &);
*/
    } catch (const Fail & fail){
        Global::debug(0) << fail.getName() << " failed" << std::endl;
        return 1;
    }

    return 0;
}
