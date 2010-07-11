#ifndef _paintown_music_class_h
#define _paintown_music_class_h

#include <string>
#include <vector>
#include "util/file-system.h"

struct AL_DUH_PLAYER;
struct DUH;

namespace Util{
    class MusicPlayer;
}

/* The music class. Dont be late or youll get an F!
 */
class Music{
public:
	Music( bool on );

	virtual ~Music();

	static bool loadSong( const char * song );
	static bool loadSong( const std::string & song );

        static void changeSong();

	/* load one of the songs in 'songs' */
	static void loadSong( const std::vector<Filesystem::AbsolutePath> & songs );

	static void pause();
	static void play();
	static void soften();
	static void louden();
	static void fadeIn( double vol );
	static void fadeOut( double vol );
	static void setVolume( double v );
	static double getVolume();
	static void mute();
	static void unmute();
	
	void doPlay();

protected:

	void _setVolume( double vol );
	void _play();
	void _pause();
	void _soften();
	void _louden();
	void _fadeIn();
	void _fadeOut();

	bool playing;
        bool enabled;

	int fading;

	bool internal_loadSong( const char * path );

        Util::MusicPlayer * musicPlayer;
	
	std::string currentSong;
};

#endif
