#include "input.h"
#include "configuration.h"
#include "paintown-engine/object/object.h"
#include <vector>

using namespace std;

static Input::PaintownInput convertKey(const Configuration & configuration, const int facing, const int key){
    if (key == configuration.getRight()){
        switch (facing){
            case Paintown::Object::FACING_LEFT : return Input::Back;
            case Paintown::Object::FACING_RIGHT : return Input::Forward;
        }
    } else if (key == configuration.getLeft()){
        switch (facing){
            case Paintown::Object::FACING_LEFT : return Input::Forward;
            case Paintown::Object::FACING_RIGHT : return Input::Back;
        }
    } else if (key == configuration.getJump()){
        return Input::Jump;
    } else if (key == configuration.getUp()){
        return Input::Up;
    } else if (key == configuration.getDown()){
        return Input::Down;
    } else if (key == configuration.getAttack1()){
        return Input::Attack1;
    } else if (key == configuration.getAttack2()){
        return Input::Attack2;
    } else if (key == configuration.getAttack3()){
        return Input::Attack3;
    }

    return Input::Unknown;
}

vector<Input::PaintownInput> Input::convertKeyboard(const Configuration & configuration, int facing, const vector<int> & keys){
    vector<Input::PaintownInput> all;

    for (vector<int>::const_iterator it = keys.begin(); it != keys.end(); it++){
        Input::PaintownInput in = convertKey(configuration, facing, *it);
        if (in != Unknown){
            all.push_back(in);
        }
        
    }

    return all;
}
