#pragma once
struct Mix_Chunk;

struct SoundData{
    SoundData():
    sample(NULL){
    }
    Mix_Chunk * sample;
};
