#include "input.h"
#include "configuration.h"
#include "object/object.h"
#include <vector>

using namespace std;

static PaintownInput convertKey(const Configuration & configuration, const int facing, const int key){
    if (key == configuration.getRight()){
        switch (facing){
            case Object::FACING_LEFT : return Back;
            case Object::FACING_RIGHT : return Forward;
        }
    } else if (key == configuration.getLeft()){
        switch (facing){
            case Object::FACING_LEFT : return Forward;
            case Object::FACING_RIGHT : return Back;
        }
    } else if (key == configuration.getJump()){
        return Jump;
    } else if (key == configuration.getUp()){
        return Up;
    } else if (key == configuration.getDown()){
        return Down;
    } else if (key == configuration.getAttack1()){
        return Attack1;
    } else if (key == configuration.getAttack2()){
        return Attack2;
    } else if (key == configuration.getAttack3()){
        return Attack3;
    }

    return Unknown;
}

vector<PaintownInput> Input::convertKeyboard(const Configuration & configuration, int facing, const vector<int> & keys){
    vector<PaintownInput> all;

    for (vector<int>::const_iterator it = keys.begin(); it != keys.end(); it++){
        PaintownInput in = convertKey(configuration, facing, *it);
        if (in != Unknown){
            all.push_back(in);
        }
        
    }

    return all;
}
