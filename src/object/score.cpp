#include "score.h"

Score::Score():
score(0){
}

Score::Score(const Score& copy):
score(copy.score){
}

Score::~Score(){
}
    
void Score::increaseScore(int much){
    score += much;
}
