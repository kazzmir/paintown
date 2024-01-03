#ifdef USE_SDL2

#include "../music-renderer.h"
#include "../music-player.h"
#include "../music-exception.h"
#include "../sound.h"
#include <sstream>
#include "../audio.h"
#include "music-renderer.h"

namespace Util {

#ifdef PS3
static const int BUFFER_SIZE = 1024 * 4;
#else
static const int BUFFER_SIZE = 1024 * 16;
#endif

// static const int BUFFER_SIZE = 65536 * 2;
Encoding formatType(int sdlFormat){
    switch (sdlFormat){
        case AUDIO_U8: return Unsigned8;
        case AUDIO_S16SYS: return Signed16;
    }

    std::ostringstream out;
    out << "Don't know how to deal with SDL format " << sdlFormat << " ; bitsize: " << SDL_AUDIO_BITSIZE(sdlFormat) << std::endl;
    throw MusicException(__FILE__, __LINE__, out.str());

    /*
    if (bigEndian()){
        switch (Sound::Info.format){
            case AUDIO_S16MSB: return Signed16;
        }
        return Signed16;
    } else {
        switch (Sound::Info.format){
            case AUDIO_S16LSB: return Signed16;
        }
        return Signed16;
    }
    */
}

// Borrowed from dumbplay.c 
/*
void stream_audio(void *userdata, Uint8 *stream, int len) {
    SDL_STREAM *streamer = (SDL_STREAM *)userdata;

    // Read samples from libdumb save them to the SDL buffer. Note that we are
    // reading SAMPLES, not bytes!
    int r_samples = len / streamer->sbytes;
    int got =
        duh_render_int(streamer->renderer, &streamer->sig_samples,
                       &streamer->sig_samples_size, streamer->bits, 0,
                       streamer->volume, streamer->delta, r_samples, stream);
    if (got == 0) {
        streamer->ended = true;
    }

    // Get current position from libdumb for the playback display. If we get
    // position that is 0, it probably means that the song ended and
    // duh_sigrenderer_get_position points to the start of the file.
    streamer->spos = duh_sigrenderer_get_position(streamer->renderer);
    if (streamer->spos == 0) {
        streamer->spos = streamer->ssize;
    }
}
*/

MusicRenderer::MusicRenderer():
convert(formatType(AUDIO_S16SYS), Sound::Info.channels, Sound::Info.frequency,
        formatType(Sound::Info.format), Sound::Info.channels, Sound::Info.frequency){
    create(Sound::Info.frequency, Sound::Info.channels);
}

MusicRenderer::MusicRenderer(int frequency, int channels):
convert(formatType(AUDIO_S16SYS), channels, frequency,
        formatType(Sound::Info.format), Sound::Info.channels, Sound::Info.frequency){
    create(frequency, channels);
}

void MusicRenderer::create(int frequency, int channels){
    
    // Global::debug(1) << "Convert between " << format << ", " << channels << ", " << frequency << " to " << Sound::Info.format << ", " << Sound::Info.channels << ", " << Sound::Info.frequency << std::endl;
    /*
    SDL_BuildAudioCVT(&convert, format, channels, frequency,
                                Sound::Info.format, Sound::Info.channels,
                                Sound::Info.frequency);
                                */

    // FIXME
    
    int size = convert.convertedLength(BUFFER_SIZE);
    data = new Uint8[size < BUFFER_SIZE ? BUFFER_SIZE : size];
    position = 0;
    converted = 0;
    
}

void MusicRenderer::fill(MusicPlayer * player){
    position = 0;
    /* read samples in dual-channel, 16-bit, signed form */
    // TimeDifference time;
    // time.startTime();
    player->render(data, BUFFER_SIZE / 4);
    // time.endTime();
    // Global::debug(0) << time.printTime("Render time") << std::endl;

    // FIXME
    converted = convert.convert(data, BUFFER_SIZE);
}

void MusicRenderer::read(MusicPlayer * player, Uint8 * stream, int bytes){
    // Global::debug(0) << "Need " << bytes << " bytes. Have " << (converted - position) << std::endl;
    while (bytes > 0){
        int length = bytes;
        if (length + position >= converted){
            length = converted - position;
        }
        /*
        if (length % 4 != 0){
            length -= length % 4;
            if (length == 0){
                fill(player);
                continue;
            }
        }
        */

        // Global::debug(0) << "Copy " << length << " bytes" << std::endl;
        /* data contains samples in the same format as the output */
        memcpy(stream, data + position, length);
        stream += length;
        position += length;
        bytes -= length;
        if (position >= converted){
            fill(player);
        }
    }
}

void MusicRenderer::mixer(void * arg, Uint8 * stream, int bytes){
    MusicPlayer * player = (MusicPlayer*) arg;

    player->getRenderer()->read(player, stream, bytes);
}

void MusicRenderer::play(MusicPlayer & player){
    /* FIXME */
    Mix_HookMusic(mixer, &player);
}

void MusicRenderer::pause(){
    /* FIXME */
    Mix_HookMusic(NULL, NULL);
}

void MusicRenderer::poll(MusicPlayer & player){
}

MusicRenderer::~MusicRenderer(){
    /* FIXME */
    Mix_HookMusic(NULL, NULL);
    delete[] data;
}

}
#endif