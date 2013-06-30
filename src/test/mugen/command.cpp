#include "mugen/command.h"
#include "mugen/ast/key.h"
#include "util/debug.h"
#include <vector>

int test1(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);
    Mugen::Command command("test", list, 100, 1);
    
    Mugen::Input input;
    input.a = true;
    if (!command.handle(input)){
        return 1;
    }

    return 0;
}

int test2(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);
    Mugen::Command command("test", list, 100, 1);
    
    Mugen::Input input;
    input.a = true;
    if (command.handle(input)){
        return 1;
    }
    input = Mugen::Input();
    input.b = true;
    if (!command.handle(input)){
        return 1;
    }

    return 0;
}

/* test that 'a, b' makes 'a' be let go before b can fire */
int test3(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);
    Mugen::Command command("test", list, 100, 1);
    
    Mugen::Input input;
    input.a = true;
    if (command.handle(input)){
        return 1;
    }
    input = Mugen::Input();
    input.a = true;
    input.b = true;
    if (command.handle(input)){
        return 1;
    }

    return 0;
}

int runTest(int (*test)(), const std::string & name){
    if (test()){
        Global::debug(0) << name << " failed" << std::endl;
        return 1;
    }
    return 0;
}

int main(int argc, char ** argv){
    if (runTest(test1, "Test1") ||
        runTest(test2, "Test2") ||
        runTest(test3, "Test3")
        ){
        return 1;
    }
}
