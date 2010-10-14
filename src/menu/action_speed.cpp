#include "action_speed.h"
#include "configuration.h"
#include "util/token.h"

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

