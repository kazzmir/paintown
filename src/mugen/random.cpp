#include "random.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

namespace Mugen{

Random::Random(){
    init(time(NULL));
}

Random::Random(uint32_t seed){
    init(seed);
}

Random::Random(const Random & copy){
    memcpy(state, copy.state, sizeof(state));
    index = copy.index;
}

Random & Random::operator=(const Random & copy){
    memcpy(state, copy.state, sizeof(state));
    index = copy.index;
    return *this;
}

void Random::init(uint32_t seed){
    for (int i = 0; i < 16; i++){
        /* By using rand_r we don't mess with any global state */
        uint64_t low = rand_r(&seed);
        uint64_t high = rand_r(&seed);
        state[i] = (high << 32) | low;
    }
}

uint64_t Random::next(){
    uint64_t a, b, c, d;
    a = state[index];
    c = state[(index+13)&15];
    b = a^c^(a<<16)^(c<<15);
    c = state[(index+9)&15];
    c ^= (c>>11);
    a = state[index] = b^c;
    d = a^((a<<5)&0xDA442D20UL);
    index = (index + 15)&15;
    a = state[index];
    state[index] = a^b^d^(a<<2)^(b<<18)^(c<<28);
    return state[index];
}

PaintownUtil::ReferenceCount<Random> Random::current;
PaintownUtil::ReferenceCount<Random> Random::getState(){
    if (current == NULL){
        current = PaintownUtil::ReferenceCount<Random>(new Random());
    }

    return current;
}
    
void Random::setState(const Random & what){
    current = PaintownUtil::ReferenceCount<Random>(new Random(what));
}

uint32_t Random::random(){
    return Random::getState()->next();
}

uint32_t random(){
    return Random::random();
}

uint32_t random(int q){
    if (q <= 0){
        return 0;
    }
    return Mugen::random() % q;
}

uint32_t random(int low, int high){
    return Mugen::random(high - low) + low;
}

}
