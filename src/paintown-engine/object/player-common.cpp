#include "character.h"
#include "player-common.h"
#include "util/load_exception.h"
#include "alliance.h"

namespace Paintown{

PlayerCommon::PlayerCommon(const Character& chr):
Character(chr),
score(0){
}

PlayerCommon::PlayerCommon(const char * filename):
Character(filename, ALLIANCE_PLAYER),
score(0){
}

PlayerCommon::PlayerCommon(const Filesystem::AbsolutePath & str):
Character(str, ALLIANCE_PLAYER),
score(0){
}

PlayerCommon::PlayerCommon(const PlayerCommon & pl):
Character(pl),
score(0){
}

PlayerCommon::~PlayerCommon(){
}

void PlayerCommon::increaseScore(int much){
    score += much;
}

}
