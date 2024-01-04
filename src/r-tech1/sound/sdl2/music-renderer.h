#pragma once

/*
#include <SDL2/SDL.h>
#include "dumb.h"

struct SDL_STREAM {
    // Library contexts
    DUH_SIGRENDERER *renderer;
    DUH *src;
    SDL_AudioDeviceID dev;
    sample_t **sig_samples;
    long sig_samples_size;

    // Runtime vars
    float delta;
    int spos;   // Samples read
    int ssize;  // Total samples available
    int sbytes; // bytes per sample
    bool ended;

    // Config switches
    int bits;
    int freq;
    int quality;
    int n_channels;
    bool no_progress;
    float volume;
    const char *input;
};

*/