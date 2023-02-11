#ifndef _rtech1_music_player_h
#define _rtech1_music_player_h

#include <string>
#include <vector>
#include <stdio.h>

#ifdef HAVE_MP3_MPG123
#include <mpg123.h>
#endif

#ifdef HAVE_OGG
#include <vorbis/vorbisfile.h>
#endif

#ifdef HAVE_MP3_MAD
#include <mad.h>
#endif

#include "../pointer.h"
#include "../file-system.h"

struct DUH;
struct DUH_SIGRENDERER;
struct DUMBFILE;
struct LOGG_Stream;
class Music_Emu;

namespace Util{

class MusicRenderer;

class MusicPlayer{
public:
    MusicPlayer();
    virtual void play();
    virtual void poll();
    virtual void pause();
    virtual void setVolume(double volume) = 0;
    virtual ~MusicPlayer();

    /* length is in samples not bytes.
     * this function must produce samples that are
     *  signed
     *  16-bit
     *  use the native endian of the machine
     *  dual-channel
     *
     *  Which means each sample should be 4 bytes
     *    2 bytes * 2 channels
     */
    virtual void render(void * stream, int length) = 0;

    virtual inline double getVolume() const {
        return volume;
    }

    virtual const ReferenceCount<MusicRenderer> & getRenderer() const {
        return out;
    }
    virtual void setRenderer(const ReferenceCount<MusicRenderer> & what);

protected:
    double volume;
    ReferenceCount<MusicRenderer> out;
};

/* uses the GME library, plays nintendo music files and others */
class GMEPlayer: public MusicPlayer {
public:
    GMEPlayer(std::string path);
    virtual void setVolume(double volume);
    virtual ~GMEPlayer();
    virtual void render(void * stream, int length);

protected:
    Music_Emu * emulator;
};

#ifdef HAVE_OGG
struct OggPage{
    struct Page{
        Page():
            position(0),
            max(0),
            buffer(NULL){
            }

        int position;
        int max;
        char * buffer;

        ~Page(){
            delete[] buffer;
        }
    };
    Page buffer1;
    // Page buffer2;
    // int use;
};

/* Maybe have some common sdl mixer class that this can inherit? */
class OggPlayer: public MusicPlayer {
public:
    OggPlayer(const Filesystem::AbsolutePath & path);
    virtual void setVolume(double volume);
    virtual void render(void * stream, int length);

    virtual ~OggPlayer();

    class Stream{
    public:
        Stream();
        virtual void reset() = 0;
        virtual size_t doRead(void *ptr, size_t size, size_t nmemb) = 0;
        virtual int doSeek(ogg_int64_t offset, int whence) = 0;
        virtual int doClose() = 0;
        virtual long doTell() = 0;

        virtual ov_callbacks oggCallbacks() = 0;

        static size_t read(void *ptr, size_t size, size_t nmemb, void *datasource);
        static int seek(void *datasource, ogg_int64_t offset, int whence);
        static int close(void *datasource);
        static long tell(void *datasource);
        virtual ~Stream();
    };

protected:
    Stream * createStream(const ReferenceCount<Storage::File> & path);
    void openOgg();
    void fillPage(OggPage::Page * page);
    void doRender(char * data, int bytes);
    // FILE* file;
    Filesystem::AbsolutePath path;
    OggVorbis_File ogg;
    ReferenceCount<OggPage> buffer;
    ReferenceCount<Stream> stream;

    int frequency;
    int channels;
    int bits;
    ogg_int64_t length;
};
#endif

#if defined (HAVE_MP3_MPG123) || defined (HAVE_MP3_MAD)

#ifdef HAVE_MP3_MPG123
class Mpg123Handler{
public:
    Mpg123Handler();
    virtual ~Mpg123Handler();

    virtual void read(void * data, int samples);
    virtual void setVolume(double volume);

    virtual std::string name() const = 0;

protected:
    virtual void reopen() = 0;

    mpg123_handle * mp3;
    double base_volume;
};
#endif
/* Interface for mp3s */
class Mp3Player: public MusicPlayer {
public:
    Mp3Player(const Path::AbsolutePath & path);
    virtual void setVolume(double volume);
    virtual void render(void * data, int length);

    virtual ~Mp3Player();
protected:    
#ifdef HAVE_MP3_MPG123
    ReferenceCount<Mpg123Handler> handler;
#elif HAVE_MP3_MAD
    void output(mad_header const * header, mad_pcm * pcm);
    static mad_flow error(void * data, mad_stream * stream, mad_frame * frame);
    static mad_flow input(void * data, mad_stream * stream);
    void discoverInfo(unsigned char * raw, int length, int * rate, int * channels);
    void fill(int frames);

    mad_stream stream;
    mad_frame frame;
    mad_synth synth;

    char * available;
    int bytesLeft;
    int position;
    bool readMore;
    struct Data{
        Data():
            data(NULL),
            length(0){
            }

        Data(char * data, int length):
            data(data), length(length){
            }

        ~Data(){
        }

        char * data;
        int length;
    };
    std::vector<Data> pages;
    unsigned char * raw;
    int rawLength;
#endif
};
#endif

/* interface to DUMB, plays mod/s3m/xm/it */
class DumbPlayer: public MusicPlayer {
public:
    DumbPlayer(const Filesystem::AbsolutePath & path);
    virtual void setVolume(double volume);
    virtual void render(void * data, int samples);

    virtual ~DumbPlayer();

    class DumbSystem{
    public:
        DumbSystem();
        virtual DUH * loadDumbFile() = 0;
        virtual ~DumbSystem();
    };

    // DUH * loadDumbFile(std::string path);

protected:
    DUH * music_file;
    DUH_SIGRENDERER * renderer;
    ReferenceCount<DumbSystem> system;
};

}

#endif
