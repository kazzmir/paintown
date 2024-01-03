#pragma once
struct Mix_Chunk;

struct SoundData{
    SoundData():
    sample(NULL),
    channel(-1){
    }
    Mix_Chunk * sample;
    int channel;
};
