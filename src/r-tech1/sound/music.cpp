#include "r-tech1/sound/music.h"
#include <string>
#include <iostream>
#include <algorithm>
// #include "defs.h"

#include "r-tech1/configuration.h"
#include "r-tech1/thread.h"
#include "r-tech1/funcs.h"
#include "r-tech1/file-system.h"
#include "r-tech1/sound/music-player.h"
#include <thread>

using namespace std;

static Music * instance = NULL;

static double volume = 1.0;
// static bool muted = false;
static std::thread musicThread;
static Util::Thread::Lock musicMutex;
static bool alive = true;

static void playMusic(Music* music);

#define LOCK Util::Thread::acquireLock(&musicMutex);
#define UNLOCK Util::Thread::releaseLock(&musicMutex);

/*
#undef LOCK
#undef UNLOCK
#define LOCK
#define UNLOCK
*/

Music::Music(bool on):
playing(false),
enabled(on),
fading(0),
musicPlayer(NULL),
currentSong(""){

    if (instance != NULL){
        cerr << "Trying to instantiate music object twice!" << endl;
        return;
    }

    volume = (double) Configuration::getMusicVolume() / 100.0;

    instance = this;

    Global::debug(0) << "Supported music extensions " << supportedExtensions() << std::endl;

    Util::Thread::initializeLock(&musicMutex);
    Global::debug(1) << "Creating music thread" << endl;
    if (on){
        musicThread = std::thread(playMusic, this);
    } else {
        /* FIXME: just don't create a thread at all.. */
        musicThread = std::thread([](){});
    }
}

/*
static bool isAlive(){
	bool f = false;
	synchronized{
		f = alive;
	}
	return f;
}
*/

static void playMusic(Music* music){
    Global::debug(1) << "Playing music" << endl;

    /*
       unsigned int tick = 0;
       unsigned int counter;
       */

    bool playing = true;
    while (playing){
        LOCK;{
            playing = alive;
            if (music != NULL){
                music->doPlay();
            }
        }
        UNLOCK;
        Util::rest(10);

        // Util::YIELD();
        // pthread_yield();
    }

    // cout << "Done with music thread" << endl;
}

double Music::getVolume(){
    double vol = 0;
    LOCK;{
        vol = volume;
    }
    UNLOCK;
    return vol;
}

void Music::doPlay(){
    if (this->playing){
        double f = fading / 500.0;
        switch (fading){
            case -1: {
                if (volume + f < 0){
                    fading = 0;
                    volume = 0;
                } else {
                    volume += f;
                    this->_setVolume(volume);
                }
                break;
            }
            case 1: {
                if (volume + f > 1.0){
                    fading = 0;
                    volume = 1.0;
                } else {
                    volume += f;
                    this->_setVolume(volume);
                }
                break;
            }
        }
        if (musicPlayer != NULL){
            musicPlayer->poll();
        }
    }
}

/*
Music::Music( const char * song ):
volume( 1.0 ),
muted( false ),
player( NULL ),
music_file( NULL ){

	loadSong( song );

}

Music::Music( const string & song ):
volume( 1.0 ),
muted( false ),
player( NULL ),
music_file( NULL ){
	loadSong( song );
}
*/

void Music::fadeIn(double vol){
    LOCK;{
        // volume = vol;
        instance->_fadeIn();
    }
    UNLOCK;
}

void Music::fadeOut( double vol ){
    LOCK;{
        // volume = vol;
        instance->_fadeOut();
    }
    UNLOCK;
}

/* FIXME */
void Music::_fadeIn(){
    // fading = 1;
}

void Music::_fadeOut(){
    // fading = -1;
}

bool Music::doLoadSong(string song){
    bool loaded = false;
    LOCK;{
        if (instance != NULL){
            loaded = instance->internal_loadSong(song);
        }
    }
    UNLOCK;
    return loaded;
    // muted = false;
}

/* remove an element from a vector at index 'pos' and return it */
template< class Tx_ >
static Tx_ removeVectorElement( vector< Tx_ > & toRemove, int pos ){
    int count = 0;
    typename vector< Tx_ >::iterator it;
    for ( it = toRemove.begin(); it != toRemove.end() && count < pos; count++, it++ );

    if ( it == toRemove.end() ){
        /* this isnt right, but whatever */
        return toRemove.front();
    }

    Tx_ removed = toRemove[pos];
    toRemove.erase(it);
    return removed;

}
	
void Music::loadSong(vector<Filesystem::AbsolutePath> songs){
    std::random_shuffle(songs.begin(), songs.end());
    for (vector<Filesystem::AbsolutePath>::iterator it = songs.begin(); it != songs.end(); it++){
        DebugLog1 << "Trying to load song " << (*it).path() << endl;
        if (doLoadSong((*it).path())){
            break;
        }
    }
}

bool Music::loadSong(const string & song){
    return doLoadSong(song);
}

void Music::_play(){
    if (playing == false && musicPlayer != NULL){
        musicPlayer->play();
        playing = true;
    }
}

void Music::play(){
    LOCK;{
        instance->_play();
    }
    UNLOCK;
}

void Music::_pause(){
    playing = false;
    if (musicPlayer != NULL){
        musicPlayer->pause();
    }
}

void Music::pause(){
    LOCK;{
        instance->_pause();
    }
    UNLOCK;
}

void Music::soften(){
    LOCK;{
        instance->_soften();
    }
    UNLOCK;
}

void Music::_soften(){
    if (volume > 0.1){
        volume -= 0.1;
    } else {
        volume = 0.0;
    }

    _setVolume(volume);
}

void Music::louden(){
    LOCK;{
        instance->_louden();
    }
    UNLOCK;
}

void Music::_louden(){
    if ( volume < 0.9 ){
        volume += 0.1;
    } else {
        volume = 1.0;
    }

    _setVolume(volume);
}

void Music::mute(){
    setVolume(0);
}

void Music::setVolume( double vol ){

    LOCK;{
        volume = vol;
        if ( volume > 1.0 ){
            volume = 1.0;
        }
        if ( volume < 0 ){
            volume = 0;
        }
        instance->_setVolume( volume );
    }
    UNLOCK;
}

void Music::_setVolume(double vol){
    if (musicPlayer != NULL){
        musicPlayer->setVolume(vol);
    }
}
	
Music::~Music(){

    LOCK;{
        alive = false;
        playing = false;
    }
    UNLOCK;

    Global::debug(1) << "Waiting for music thread to die" << endl;
    musicThread.join();
        
    LOCK;{
        musicPlayer = NULL;
    }
    UNLOCK;
}

static string getExtension(string path){
    if (path.rfind('.') != string::npos){
        return Util::lowerCaseAll(path.substr(path.rfind('.') + 1));
    }
    return "";
}

/* true if the file extension is something DUMB will probably recognize */
static bool isDumbFile(string path){
    string extension = getExtension(path);
    return extension == "mod" ||
           extension == "s3m" ||
           extension == "it" ||
           extension == "xm";
}

static bool isGMEFile(string path){
    string extension = getExtension(path);
    return extension == "nsf" ||
           extension == "spc" ||
           extension == "gym";
}

static bool isOggFile(string path){
    string extension = getExtension(path);
    return extension == "ogg";
}

static bool isMp3File(string path){
    string extension = getExtension(path);
    return extension == "mp3";
}

/* returns a string that lists all the extensions we can play */
string Music::supportedExtensions(){
    ostringstream out;
    vector<string> all;
    all.push_back("xm (extended module)");
    all.push_back("s3m (scream tracker 3)");
    all.push_back("mod (module)");
    all.push_back("it (impulse tracker)");
    all.push_back("nsf (nintendo sound file / famicom)");
    all.push_back("spc (super nintendo / super famicom)");
    all.push_back("gym (sega genesis / mega drive)");
#if defined(HAVE_MP3_MPG123) || defined(HAVE_MP3_MAD)
    all.push_back("mp3 (Mpg Layer 3)");
#endif
#ifdef HAVE_OGG
    all.push_back("ogg (Ogg Vorbis)");
#endif
    bool first = true;
    for (vector<string>::iterator it = all.begin(); it != all.end(); it++){
        if (!first){
            out << ", ";
        } else {
            first = false;
        }
        out << *it;
    }
    return out.str();
}

bool Music::internal_loadSong(string path){
    if (!enabled){
        return false;
    }

    // Check current song and/or set it
    if (currentSong.compare(path)==0){
        return true;
    } else {
        currentSong = path;
    }
    
    DebugLog1 << "Trying to load '" << path << "'" << endl;

    /*
    if (musicPlayer != NULL){
        Global::debug(0) << "Destroy music player" << std::endl;
        delete musicPlayer;
        musicPlayer = NULL;
    }
    */

    try {
        if (isDumbFile(path)){
            musicPlayer = new Util::DumbPlayer(Filesystem::AbsolutePath(path));
            musicPlayer->play();
            playing = true;
        } else if (isGMEFile(path)){
            musicPlayer = new Util::GMEPlayer(path);
            musicPlayer->play();
            playing = true;
#ifdef HAVE_OGG
        } else if (isOggFile(path)){
            musicPlayer = new Util::OggPlayer(Filesystem::AbsolutePath(path));
            musicPlayer->play();
            playing = true;
#endif

#if defined(HAVE_MP3_MPG123) || defined(HAVE_MP3_MAD)
	} else if (isMp3File(path)){
	    /* Utilize SDL mixer to handle mp3 */
	    musicPlayer = new Util::Mp3Player(Filesystem::AbsolutePath(path));
	    musicPlayer->play();
	    playing = true;
#endif
        } else {
            return false;
        }
        if (musicPlayer != NULL){
            musicPlayer->setVolume(volume);
        }
    } catch (const Exception::Base & ex){
        Global::debug(0) << "Could not open music file '" << path << "' because " << ex.getTrace() << endl;
        //! FIXME Change from Base exception in the future
        return false;
    }

    return true;
}

void Music::changeSong(){
    // pause();
    fadeIn(0.3);
    loadSong(Storage::instance().getFiles(Storage::instance().find(Filesystem::RelativePath("music/")), "*"));
    play();
}

#undef LOCK
#undef UNLOCK
