#include "character.h"
#include "player-common.h"
#include "util/load_exception.h"
#include "globals.h"

PlayerCommon::PlayerCommon(const Character& chr) throw (LoadException):
Character(chr),
score(0){
}

PlayerCommon::PlayerCommon(const char * filename) throw (LoadException):
Character(filename, ALLIANCE_PLAYER),
score(0){
}

PlayerCommon::PlayerCommon(const std::string & str) throw (LoadException):
Character(str, ALLIANCE_PLAYER),
score(0){
}

PlayerCommon::PlayerCommon(const PlayerCommon & pl) throw (LoadException):
Character(pl),
score(0){
}

PlayerCommon::~PlayerCommon(){
}

void PlayerCommon::increaseScore(int much){
    score += much;
}
