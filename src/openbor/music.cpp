#include <string>
#include <iostream>
#include <fstream>
#include <string.h>
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_mixer.h>

using namespace std;

/* IMA ADPCM references:
 * http://www.cs.columbia.edu/~hgs/audio/dvi/IMA_ADPCM.pdf
 * http://wiki.multimedia.cx/index.php?title=IMA_ADPCM
 *
 * IMA decoding code from SDL (src/audio/SDL_wave.c)
 * The only difference between this code and the SDL code is that
 * this code does not assume the IMA data is split up into blocks.
 */
struct IMA_ADPCM_decodestate {
    Sint32 sample;
    Sint8 index;
};

static Sint32 IMA_ADPCM_nibble(struct IMA_ADPCM_decodestate *state, Uint8 nybble){
    const Sint32 max_audioval = ((1<<(16-1))-1);
    const Sint32 min_audioval = -(1<<(16-1));
    static const int index_table[16] = {
        -1, -1, -1, -1, 2,  4,  6,  8,
        -1, -1, -1, -1, 2,  4,  6,  8
    };
    static const Sint32 step_table[89] = {
        7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31,
        34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130,
        143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408,
        449, 494, 544, 598, 658, 724, 796, 876, 963, 1060, 1166, 1282,
        1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327,
        3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630,
        9493, 10442, 11487, 12635, 13899, 15289, 16818, 18500, 20350,
        22385, 24623, 27086, 29794, 32767
    };
    Sint32 delta, step;

    /* Compute difference and new sample value */
    step = step_table[state->index];
    delta = step >> 3;
    if ( nybble & 0x04 ) delta += step;
    if ( nybble & 0x02 ) delta += (step >> 1);
    if ( nybble & 0x01 ) delta += (step >> 2);
    if ( nybble & 0x08 ) delta = -delta;
    state->sample += delta;

    /* Update index value */
    state->index += index_table[nybble];
    if ( state->index > 88 ) {
        state->index = 88;
    } else if ( state->index < 0 ) {
        state->index = 0;
    }

    /* Clamp output sample */
    if ( state->sample > max_audioval ) {
        state->sample = max_audioval;
    } else if ( state->sample < min_audioval ) {
        state->sample = min_audioval;
    }

    return state->sample;
}

static void Fill_IMA_ADPCM_block(unsigned char * decoded, const unsigned char * encoded, int channel, int channels, struct IMA_ADPCM_decodestate *state){

    // decoded += (channel * 2);
    for (int i = 0; i < 4; i++){
        Sint8 nybble = (*encoded)&0x0F;
        Sint32 new_sample = IMA_ADPCM_nibble(state, nybble);
        decoded[0] = new_sample&0xFF;
        new_sample >>= 8;
        decoded[1] = new_sample&0xFF;
        decoded += 2 * channels;

        nybble = (*encoded)>>4;
        new_sample = IMA_ADPCM_nibble(state, nybble);
        decoded[0] = new_sample&0xFF;
        new_sample >>= 8;
        decoded[1] = new_sample&0xFF;
        decoded += 2 * channels;

        ++encoded;
    }
}

void do_ima_adpcm_decode(unsigned char * input, unsigned char * output, int samples, int channels){
    struct IMA_ADPCM_decodestate state[2];
    state[0].sample = 0;
    state[0].index = 0;
    state[1].sample = 0;
    state[1].index = 0;
    output += 2;
    while (samples > 0){
        for (int c = 0; c < channels; c++){
            Fill_IMA_ADPCM_block(output, input, c, channels, &state[c]);
            input += 4;
            samples -= 8;
        }
        output += (channels * 8 * 2);
    }
}

unsigned char * ima_adpcm_to_wav(string file, int * length){
    ifstream input(file.c_str(), ios::in | ios::binary);
    input.seekg(0, ios::end);
    int size = (unsigned int) input.tellg() - 160;
    unsigned char * data = new unsigned char[size];
    memset(data, 0, size);
    /* most bor data files start at 160
     * TODO: Get the frequency and channels
     */ 
    input.seekg(160);
    input.read((char*) data, size);
    int samples = size * 2;
    unsigned char * output = new unsigned char[size * 4];
    memset(output, 0, size*4);
    do_ima_adpcm_decode(data, output, samples, 1);
    *length = size * 4;
    delete[] data;
    input.close();

    ofstream out("raw.pcm", ios::out | ios::binary);
    out.write((char*) output, size*4);
    out.close();

    return output;
}

struct Player{
    Uint8 * data;
    int position;
};

void playit(void * data, Uint8 * stream, int length){
    Player * player = (Player *) data;
    memcpy(stream, player->data + player->position, length);
    player->position += length;

    /* For stereo from mono
    for (int i = 0; i < length / 2; i++){
        Sint16 sample = *(Sint16*)(player->data + player->position + i * 2);
        Sint16 * sint = (Sint16*) stream;
        sint[i*2] = sample;
        sint[i*2+1] = sample;
    }
    // memcpy(stream, player->data + player->position, length);
    player->position += length / 2;
    */
}

Uint8 * convert(Uint8 * data, int length){
    SDL_AudioCVT converter;
    SDL_AudioSpec adpcm;
    adpcm.format = AUDIO_S16;
    adpcm.channels = 1;
    adpcm.freq = 22050;
    SDL_AudioSpec to;
    to.format = AUDIO_S16;
    to.channels = 2;
    to.freq = 22050;
    
    int build = SDL_BuildAudioCVT(&converter, adpcm.format, adpcm.channels, adpcm.freq, to.format, to.channels, to.freq);
    if (build <= 0){
        cout << "Could not build converter: " << build << " " << SDL_GetError() << endl;
        exit(0);
    }
    converter.buf = new unsigned char[length * converter.len_mult];
    memcpy(converter.buf, data, length);
    converter.len = length;
    SDL_ConvertAudio(&converter);
    return converter.buf;
}

void play(string file){
    SDL_AudioSpec wavSpec;
    Uint32 wav_length = 0;
    Uint8 * wav_buffer = 0;
    int length = 0;
    cout << "Loading ima file" << endl;
    unsigned char * data = ima_adpcm_to_wav(file, &length);
    cout << "Loaded" << endl;
    /*
    SDL_RWops * ops = SDL_RWFromConstMem(data, MAX_SIZE);
    SDL_AudioSpec * out = SDL_LoadWAV_RW(ops, 0, &wavSpec, &wav_buffer, &wav_length);
    if (out == 0){
        cout << "Could not load audio: " << SDL_GetError() << endl;
    }

    // if (Mix_OpenAudio(out->freq, out->format, out->channels, 4096) != 0){
    // if (Mix_OpenAudio(out->freq, out->format, 2, 4096) != 0){
    if (Mix_OpenAudio(22050, AUDIO_S16, 2, 4096) != 0){
        cout << "Could not open audio!" << endl;
    }
    SDL_FreeRW(ops);
    ops = SDL_RWFromConstMem(data, MAX_SIZE);

    Mix_Chunk * sound = Mix_LoadWAV_RW(ops, 0);
    cout << "Loaded rw file: " << sound << Mix_GetError() << endl;
    SDL_FreeRW(ops);
    */

    Player * player = new Player;
    player->data = (Uint8*) convert(data, length);
    player->position = 0;
    
    if (Mix_OpenAudio(22050, AUDIO_S16, 2, 4096) != 0){
        cout << "Could not open audio!" << endl;
    }

    Mix_HookMusic(playit, player);

    /*
    SDL_AudioSpec xout;
    Player * player = new Player();
    player->data = wav_buffer;
    player->position = 0;
    wavSpec.userdata = player;
    wavSpec.callback = playit;
    SDL_OpenAudio(&wavSpec, &xout);
    SDL_PauseAudio(0);
    cout << "Stereo: " << (int) xout.channels << endl;
    cout << "Format: " << (int) xout.format << endl;
    cout << "Frequency: " << (int) xout.freq << endl;
    */

    /*
    int channel = Mix_PlayChannel(-1, sound, 0);
    if (channel == -1){
        cout << "Could not play!" << endl;
    }
    */
}

int main(int argc, char ** argv){
    SDL_Init(SDL_INIT_AUDIO);
    if (argc > 1){
        play(argv[1]);
    }
    SDL_Delay(100000000);
}
