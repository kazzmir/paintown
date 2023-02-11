#ifndef _rtech1_music_renderer_h
#define _rtech1_music_renderer_h

#ifdef USE_SDL
/* for Uint8 */
#include <SDL.h>
#include "sdl/mixer/SDL_mixer.h"
#include "audio.h"
#endif

#ifdef USE_ALLEGRO
struct AUDIOSTREAM;
#endif

#ifdef USE_ALLEGRO5
struct ALLEGRO_AUDIO_STREAM;
struct ALLEGRO_EVENT_QUEUE;
#endif

namespace Util{

class MusicPlayer;
/* implemented by some backend: allegro4/sdl/allergo5 */
class MusicRenderer{
public:
    MusicRenderer();
    MusicRenderer(int frequency, int channels);
    virtual ~MusicRenderer();
    void poll(MusicPlayer & player);
    void play(MusicPlayer & player);
    void pause();

protected:
    void create(int frequency, int channels);
#ifdef USE_SDL
    static void mixer(void * arg, Uint8 * stream, int length);
    void fill(MusicPlayer * player);
    void read(MusicPlayer * player, Uint8 * stream, int bytes);
    AudioConverter convert;
    // SDL_AudioCVT convert;
    Uint8 * data;
    int position;
    int converted;
#endif

#ifdef USE_ALLEGRO
    AUDIOSTREAM * stream;
#endif

#ifdef USE_ALLEGRO5
    ALLEGRO_AUDIO_STREAM * stream;
    ALLEGRO_EVENT_QUEUE * queue;
#endif
};

}

#endif
