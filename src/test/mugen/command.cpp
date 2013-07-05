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
    input.pressed.a = true;
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
    input.pressed.a = true;
    if (command.handle(input)){
        return 1;
    }
    input = Mugen::Input();
    input.pressed.b = true;
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
    input.pressed.a = true;
    if (command.handle(input)){
        return 1;
    }
    input = Mugen::Input();
    input.pressed.a = true;
    input.pressed.b = true;
    if (command.handle(input)){
        return 1;
    }

    return 0;
}

int test4(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);
    Mugen::Command command("test", list, 100, 1);
    
    Mugen::Input input;
    input.pressed.a = true;
    /* allow a to be held down for a few ticks */
    for (int i = 0; i < 10; i++){
        if (command.handle(input)){
            return 1;
        }
    }
    input = Mugen::Input();
    input.pressed.b = true;
    if (!command.handle(input)){
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
        runTest(test3, "Test3") ||
        runTest(test4, "Test4") ||
        /* having false here lets us copy/paste a runTest line easily */
        false
        ){
        return 1;
    }
}
