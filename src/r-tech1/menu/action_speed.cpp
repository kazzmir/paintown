#include "r-tech1/menu/action_speed.h"
#include "r-tech1/configuration.h"
#include "r-tech1/token.h"

ActionSpeed::ActionSpeed(const Token *token):
speed(0.1){
    token->view() >> speed;
}

ActionSpeed::~ActionSpeed(){
}

void ActionSpeed::act(){
    if (speed < 0.1){
        speed = 0.1;
    }
    Configuration::setGameSpeed(speed);
}

