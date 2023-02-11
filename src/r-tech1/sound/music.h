#ifndef _rtech1_music_class_h
#define _rtech1_music_class_h

#include <string>
#include <vector>
#include "../file-system.h"
#include "../pointer.h"

namespace Util{
    class MusicPlayer;
}

/* The music class. Dont be late or youll get an F!
 */
class Music{
public:
	Music( bool on );

	virtual ~Music();

	static bool loadSong(const std::string & song);
        static void changeSong();

	/* randomnly select a song from 'songs' and load it */
	static void loadSong(std::vector<Filesystem::AbsolutePath> songs);

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
	static bool doLoadSong(std::string song);

        std::string supportedExtensions();

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

	bool internal_loadSong(std::string path);

        Util::ReferenceCount<Util::MusicPlayer> musicPlayer;
	
	std::string currentSong;
};

#endif
