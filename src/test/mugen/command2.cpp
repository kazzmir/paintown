#include "mugen/command.h"
#include "mugen/ast/key.h"
#include "util/debug.h"
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Command{
public:
    Command(const Ast::KeyList * keys):
    keys(keys){
    }

    const Ast::KeyList * keys;

    bool process(vector<Mugen::Input> & inputs, Ast::Key * key){
        class Walker: public Ast::Walker {
        public:
            Walker(vector<Mugen::Input> & inputs):
            inputs(inputs),
            ok(false){
            }

            vector<Mugen::Input> & inputs;
            bool ok;
                        
            virtual void onKeySingle(const Ast::KeySingle & key){
                if (inputs.size() > 0){
                    const Mugen::Input & input = inputs[0];
                    if (key == "a" && input.pressed.a){
                        ok = true;
                    }
                }
            }
        };

        Walker walker(inputs);
        key->walk(walker);

        return walker.ok;
    }

    bool process(vector<Mugen::Input> inputs){
        const vector<Ast::Key*> & key_vector = keys->getKeys();
        for (vector<Ast::Key*>::const_iterator it = key_vector.begin(); it != key_vector.end(); it++){
            Ast::Key * key = *it;
            if (!process(inputs, key)){
                return false;
            }
        }

        return true;
    }
};

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

vector<Mugen::Input> readInput(const string & input){
    vector<string> lines = Util::splitString(input, ';');
    vector<Mugen::Input> out;
    
    for (vector<string>::iterator it = lines.begin(); it != lines.end(); it++){
        out.push_back(convert(*it));
    }

    return out;
}

static bool test1(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    Command a(list);

    vector<Mugen::Input> inputs = readInput("a");
    return a.process(inputs);
}

static bool test2(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    Command a(list);

    vector<Mugen::Input> inputs = readInput("b");
    return !a.process(inputs);
}

static bool run(bool (*test)(), const string & name){
    bool out = test();
    Global::debug(0) << "Test " << name << " " << out << std::endl;
    return out;
}

int main(int argc, char ** argv){
    return !(run(test1, "test1") &&
            run(test2, "test2") &&
            1);
}
