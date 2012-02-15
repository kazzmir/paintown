#include "match.h"

#include "util/file-system.h"

using namespace Mugen;
using namespace Match;

int Round::framesPerCount = 0;
    
Round::Round(int number):
roundNumber(number),
time(0),
ticker(0),
winner(NotAwarded),
state(NotStarted){
}

Round::~Round(){
}
    
void Round::act(){
}

void Round::setTime(){
}

int Round::getTime() const{
    return time;
}

void Round::setWinner(const Side player){
}

const Side & Round::getWinner(){
    return winner;
}
    
Manager::Manager(const Filesystem::AbsolutePath & file){
    //Round::setFramesPerCount();
}

Manager::~Manager(){
}

void Manager::act(){
}