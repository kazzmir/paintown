#ifdef USE_ALLEGRO
#include <allegro.h>
#endif

#ifdef USE_ALLEGRO5
#include <allegro5/allegro_audio.h>
#endif

#if 0

#ifdef USE_ALLEGRO
#include "../libs/dumb/include/aldumb.h"

#ifdef _WIN32
/* what do we need winalleg for?
 * reason: ...
 */
#include <winalleg.h>
#endif

#endif

#ifdef USE_SDL
#include "sdl/mixer/SDL_mixer.h"
#endif

#endif

#ifdef USE_SDL2
#include <SDL2/SDL_audio.h>
#endif

#include "music-renderer.h"
#include "music-player.h"
#include "music-exception.h"
#include "sound.h"
#include <sstream>
#include "audio.h"

namespace Util{

#ifdef USE_ALLEGRO5
const int DUMB_SAMPLES = 1024;
MusicRenderer::MusicRenderer(){
    create(Sound::Info.frequency, 2);
}
    
MusicRenderer::MusicRenderer(int frequency, int channels){
    create(frequency, channels);
}

void MusicRenderer::create(int frequency, int channels){
    ALLEGRO_CHANNEL_CONF configuration = ALLEGRO_CHANNEL_CONF_2;
    switch (channels){
        case 1: configuration = ALLEGRO_CHANNEL_CONF_1; break;
        case 2: configuration = ALLEGRO_CHANNEL_CONF_2; break;
        case 3: configuration = ALLEGRO_CHANNEL_CONF_3; break;
        case 4: configuration = ALLEGRO_CHANNEL_CONF_4; break;
        case 5: configuration = ALLEGRO_CHANNEL_CONF_5_1; break;
        case 6: configuration = ALLEGRO_CHANNEL_CONF_6_1; break;
        case 7: configuration = ALLEGRO_CHANNEL_CONF_7_1; break;
        default: configuration = ALLEGRO_CHANNEL_CONF_2; break;
    }
    stream = al_create_audio_stream(4, DUMB_SAMPLES, frequency, ALLEGRO_AUDIO_DEPTH_INT16,  configuration);
    if (!stream){
        throw MusicException(__FILE__, __LINE__, "Could not create allegro5 audio stream");
    }
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_audio_stream_event_source(stream));
}

void MusicRenderer::play(MusicPlayer & player){
    al_attach_audio_stream_to_mixer(stream, al_get_default_mixer());
}

void MusicRenderer::pause(){
    al_detach_audio_stream(stream);
}

MusicRenderer::~MusicRenderer(){
    al_destroy_audio_stream(stream);
    al_destroy_event_queue(queue);
}

void MusicRenderer::poll(MusicPlayer & player){
    ALLEGRO_EVENT event;
    while (al_get_next_event(queue, &event)){
        if (event.type == ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT) {
            ALLEGRO_AUDIO_STREAM * stream = (ALLEGRO_AUDIO_STREAM *) event.any.source;
            void * data = al_get_audio_stream_fragment(stream);
            if (data != NULL){
                player.render(data, al_get_audio_stream_length(stream));
                al_set_audio_stream_fragment(stream, data);
            }
        }
    }
}
#elif USE_SDL
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
#if SDL_VERSION_ATLEAST(1, 3, 0)
        case AUDIO_F32MSB: return Float32;
        case AUDIO_F32LSB: return Float32;
#endif
    }

    std::ostringstream out;
    out << "Don't know how to deal with SDL format " << sdlFormat << std::endl;
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
    int size = convert.convertedLength(BUFFER_SIZE);
    data = new Uint8[size < BUFFER_SIZE ? BUFFER_SIZE : size];
    position = 0;
    converted = 0;
}

/*
static int sampleSize(){
    int size = 1;
    switch (Sound::Info.format){
        case AUDIO_U8:
        case AUDIO_S8: size = 1; break;
        case AUDIO_U16LSB:
        case AUDIO_S16LSB:
        case AUDIO_U16MSB:
        case AUDIO_S16MSB: size = 2; break;
#if SDL_VERSION_ATLEAST(1, 3, 0)
        case AUDIO_S32LSB:
        case AUDIO_S32MSB:
        case AUDIO_F32LSB:
        case AUDIO_F32MSB: size = 2; break;
#endif
        default: size = 2; break;
    }
    return size * Sound::Info.channels;
}
*/

void MusicRenderer::fill(MusicPlayer * player){
    position = 0;
    /* read samples in dual-channel, 16-bit, signed form */
    // TimeDifference time;
    // time.startTime();
    player->render(data, BUFFER_SIZE / 4);
    // time.endTime();
    // Global::debug(0) << time.printTime("Render time") << std::endl;

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
    // Mix_HookMusic(mixer, &player);
}

void MusicRenderer::pause(){
    /* FIXME */
    // Mix_HookMusic(NULL, NULL);
}

void MusicRenderer::poll(MusicPlayer & player){
}

MusicRenderer::~MusicRenderer(){
    /* FIXME */
    // Mix_HookMusic(NULL, NULL);
    delete[] data;
}
#elif USE_SDL2

// Moved to sdl2/music-renderer.cpp

#elif USE_ALLEGRO
int BUFFER_SIZE = 1 << 11;
static int ALLEGRO_MONO = 0;
static int ALLEGRO_STEREO = 1;
MusicRenderer::MusicRenderer(){
    create(Sound::Info.frequency, 2);
}

MusicRenderer::MusicRenderer(int frequency, int channels){
    create(frequency, channels);
}

void MusicRenderer::create(int frequency, int channels){
    int configuration = ALLEGRO_STEREO;
    if (channels == 1){
        configuration = ALLEGRO_MONO;
    }
    stream = play_audio_stream(BUFFER_SIZE, 16, configuration, frequency, 255, 128);
    if (!stream){
        throw MusicException(__FILE__, __LINE__, "Could not create Allegro stream");
    }
    if (stream->len != BUFFER_SIZE){
        throw MusicException(__FILE__, __LINE__, "Buffer size mismatch");
    }
    voice_set_priority(stream->voice, 255);
}

void MusicRenderer::play(MusicPlayer & player){
    voice_start(stream->voice);
}

void MusicRenderer::pause(){
    voice_stop(stream->voice);
}

void MusicRenderer::poll(MusicPlayer & player){
    short * buffer = (short*) get_audio_stream_buffer(stream);
    if (buffer){
        player.render(buffer, BUFFER_SIZE);

        /* allegro wants unsigned data but gme produces signed so to convert
         * signed samples to unsigned samples we have to raise each value
         * by half the maximum value of a short (0xffff+1)/2 = 0x8000
         */
        for (int i = 0; i < BUFFER_SIZE * 2; i++){
            buffer[i] += 0x8000;
        }

        free_audio_stream_buffer(stream);
    }
}

MusicRenderer::~MusicRenderer(){
    stop_audio_stream(stream);
}

#endif /* USE_ALLEGRO */

}
