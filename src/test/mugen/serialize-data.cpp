#include <exception>
#include "util/debug.h"
#include "util/token.h"
#include "mugen/common.h"
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

int main(int argc, char ** argv){
    try{
        testAttackTypeAttribute();
    } catch (const Fail & fail){
        Global::debug(0) << fail.getName() << " failed" << std::endl;
    }

    return 0;
}
