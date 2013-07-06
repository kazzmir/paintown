#include "mugen/command.h"
#include "mugen/ast/key.h"
#include "util/debug.h"
#include <vector>
#include <string>
#include <sstream>

using std::vector;
using std::string;
using std::ostringstream;

void parseKey(Mugen::Input & input, const string & key){
    if (key == "a"){
        input.pressed.a = true;
    } else if (key == "b"){
        input.pressed.b = true;
    } else if (key == "c"){
        input.pressed.c = true;
    } else if (key == "x"){
        input.pressed.x = true;
    } else if (key == "y"){
        input.pressed.y = true;
    } else if (key == "z"){
        input.pressed.z = true;
    } else if (key == "U"){
        input.pressed.up = true;
    } else if (key == "D"){
        input.pressed.down = true;
    } else if (key == "F"){
        input.pressed.forward = true;
    } else if (key == "B"){
        input.pressed.back = true;
    } else if (key == "~a"){
        input.released.a = true;
    } else if (key == "~b"){
        input.released.b = true;
    } else if (key == "~c"){
        input.released.c = true;
    } else if (key == "~x"){
        input.released.x = true;
    } else if (key == "~y"){
        input.released.y = true;
    } else if (key == "~z"){
        input.released.z = true;
    } else if (key == "~U"){
        input.released.up = true;
    } else if (key == "~D"){
        input.released.down = true;
    } else if (key == "~F"){
        input.released.forward = true;
    } else if (key == "~B"){
        input.released.back = true;
    } else {
        Global::debug(0) << "Unknown key " << key << std::endl;
    }
}

Mugen::Input convert(string raw){
    size_t semicolon = raw.find(';');
    if (semicolon != -1){
        raw.erase(semicolon, 1);
    }
    vector<string> keys = Util::splitString(raw, ',');
    Mugen::Input input;
    
    for (vector<string>::iterator it = keys.begin(); it != keys.end(); it++){
        parseKey(input, Util::trim(*it));
    }

    return input;
}

vector<Mugen::Input> loadScript(const string & input){
    vector<string> lines = Util::splitString(input, ';');
    vector<Mugen::Input> out;
    
    for (vector<string>::iterator it = lines.begin(); it != lines.end(); it++){
        out.push_back(convert(*it));
    }

    return out;
}

int testKeys(Mugen::Command & command, const vector<Mugen::Input> & inputs){
    /* Make sure we emit something */
    bool emitted = false;
    for (vector<Mugen::Input>::const_iterator it = inputs.begin(); it != inputs.end(); it++){
        const Mugen::Input & input = *it;
        bool last = false;
        if (&input == &inputs.back()){
            last = true;
        }

        if (!last){
            /* If its not the last input then the command should not fire, so if it does then its an error */
            if (command.handle(input)){
                return 1;
            }
        } else {
            /* And similarly if its the last input the command should fire, otherwise error */
            if (!command.handle(input)){
                return 1;
            } else {
                emitted = true;
            }
        }
    }

    return emitted ? 0 : 1;
}

int test1(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);
    Mugen::Command command("test", list, 100, 1);
    
    std::ostringstream script;
    script << "a;";

    return testKeys(command, loadScript(script.str()));
}

int test2(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);
    Mugen::Command command("test", list, 100, 1);

    std::ostringstream script;
    script << "a;";
    script << "b;";

    return testKeys(command, loadScript(script.str()));
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
    
    std::ostringstream script;
    script << "a;a;a;a;a;";
    script << "a;a;a;a;a;";
    script << "b;";

    return testKeys(command, loadScript(script.str()));

    /*
    Mugen::Input input;
    input.pressed.a = true;
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
    */
}

/* ~a, ~a, ~a */
int test5(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeyModifier(0, 0, Ast::KeyModifier::Release, new Ast::KeySingle(0, 0, "a"), 0));
    keys.push_back(new Ast::KeyModifier(0, 0, Ast::KeyModifier::Release, new Ast::KeySingle(0, 0, "a"), 0));
    keys.push_back(new Ast::KeyModifier(0, 0, Ast::KeyModifier::Release, new Ast::KeySingle(0, 0, "a"), 0));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);
    Mugen::Command command("test", list, 100, 1);

    std::ostringstream script;
    script << "a;~a;";
    script << ";";
    script << "a;~a;";
    script << ";";
    script << "a;~a;";

    return testKeys(command, loadScript(script.str()));

    /*
    Mugen::Input input;
    input.pressed.a = true;
    if (command.handle(input)){
        return 1;
    }

    input = Mugen::Input();
    input.released.a = true;
    if (command.handle(input)){
        return 1;
    }

    input = Mugen::Input();
    if (command.handle(input)){
        return 1;
    }

    input.pressed.a = true;
    if (command.handle(input)){
        return 1;
    }

    input.pressed.a = false;
    input.released.a = true;
    if (command.handle(input)){
        return 1;
    }

    input = Mugen::Input();
    if (command.handle(input)){
        return 1;
    }

    input.pressed.a = true;
    if (command.handle(input)){
        return 1;
    }

    input.pressed.a = false;
    input.released.a = true;
    if (!command.handle(input)){
        return 1;
    }

    return 0;
    */
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
        runTest(test5, "Test5") ||
        /* having false here lets us copy/paste a runTest line easily */
        false
        ){
        return 1;
    }
}
