#include "music.h"
#include <string>
#include <iostream>
#include "globals.h"
// #include "defs.h"

#include "configuration.h"
#include "util/thread.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/music-player.h"

using namespace std;

static Music * instance = NULL;

static double volume = 1.0;
// static bool muted = false;
static Util::Thread::Id musicThread;
static Util::Thread::Lock musicMutex;
static bool alive = true;

static void * playMusic( void * );

#define synchronized for( int __l( ! Util::Thread::acquireLock(&musicMutex)); __l; __l = 0, Util::Thread::releaseLock(&musicMutex) )

#define LOCK Util::Thread::acquireLock(&musicMutex);
#define UNLOCK Util::Thread::releaseLock(&musicMutex);

/*
#undef LOCK
#undef UNLOCK
#define LOCK
#define UNLOCK
*/

static void * bogus_thread( void * x){
	return NULL;
}

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

    Util::Thread::initializeLock(&musicMutex);
    Global::debug(1) << "Creating music thread" << endl;
    if (on){
        Util::Thread::createThread(&musicThread, NULL, (Util::Thread::ThreadFunction) playMusic, (void *)instance);
    } else {
        /* FIXME: just don't create a thread at all.. */
        Util::Thread::createThread(&musicThread, NULL, (Util::Thread::ThreadFunction) bogus_thread, NULL);
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

static void * playMusic( void * _music ){
    Music * music = (Music *) _music;

    Global::debug(1) << "Playing music" << endl;

    /*
       unsigned int tick = 0;
       unsigned int counter;
       */

    bool playing = true;
    while (playing){
        LOCK;{
            playing = alive;
            music->doPlay();
        }
        UNLOCK;
        Util::rest(10);

        // Util::YIELD();
        // pthread_yield();
    }

    // cout << "Done with music thread" << endl;

    return NULL;
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
        musicPlayer->poll();
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

bool Music::loadSong( const char * song ){
    bool loaded = false;
    LOCK;{
        loaded = instance->internal_loadSong(song);
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

    const Tx_ & removed = toRemove[ pos ];
    toRemove.erase( it );
    return removed;

}
	
void Music::loadSong( const vector<Filesystem::AbsolutePath> & Songs ){

    /*
       cout << "Songs = " << &Songs << endl;
       if ( ! loadSong( "music/song5.xm" ) ){
       cerr << "Could not load music/song5.xm" << endl;
       }
       return;
       */

    vector<Filesystem::AbsolutePath> _songs = Songs;
    vector<Filesystem::AbsolutePath> songs;
    while ( ! _songs.empty() ){
        int i = Util::rnd( _songs.size() );
        songs.push_back(removeVectorElement(_songs, i));
    }

    /*
       songs.clear();
       songs.push_back( "music/song3.xm" );
       */

    for ( vector<Filesystem::AbsolutePath>::iterator it = songs.begin(); it != songs.end(); it++ ){
        Global::debug( 1 ) << "Trying to load song " << (*it).path() << endl;
        if (loadSong((*it).path())){
            break;
        }
    }
}

bool Music::loadSong( const string & song ){
    return loadSong( song.c_str() );
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
    if (musicPlayer){
        musicPlayer->setVolume(vol);
    }
}
	
Music::~Music(){

    LOCK;{
        if (musicPlayer){
            delete musicPlayer;
        }

        alive = false;
        playing = false;
    }
    UNLOCK;

    Global::debug( 1 ) << "Waiting for music thread to die" << endl;
    Util::Thread::joinThread(musicThread);

}

static string getExtension(const char * path_){
    string path(path_);
    if (path.rfind('.') != string::npos){
        return Util::lowerCaseAll(path.substr(path.rfind('.') + 1));
    }
    return "";
}

/* true if the file extension is something DUMB will probably recognize */
static bool isDumbFile(const char * path){
    string extension = getExtension(path);
    return extension == "mod" ||
           extension == "s3m" ||
           extension == "it" ||
           extension == "xm";
}

static bool isGMEFile(const char * path){
    string extension = getExtension(path);
    return extension == "nsf" ||
           extension == "spc" ||
           extension == "gym";
}

static bool isOggFile(const char * path){
    string extension = getExtension(path);
    return extension == "ogg";
}


bool Music::internal_loadSong( const char * path ){
    if (!enabled){
        return false;
    }

    // cout << "Trying to load '" << path << "'" << endl;

    // Check current song and/or set it
    if (currentSong.compare(std::string(path))==0){
        return true;
    } else {
        currentSong = std::string(path);
    }

    if (musicPlayer != NULL){
        delete musicPlayer;
        musicPlayer = NULL;
    }
    try {
        if (isDumbFile(path)){
            musicPlayer = new Util::DumbPlayer(path);
            musicPlayer->play();
            playing = true;
        } else if (isGMEFile(path)){
            musicPlayer = new Util::GMEPlayer(path);
            musicPlayer->play();
            playing = true;
#ifdef HAVE_OGG
        } else if (isOggFile(path)){
            musicPlayer = new Util::OggPlayer(path);
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
        //! FIXME Change from Base exception in the futer
        return false;
    }

    return true;
}

void Music::changeSong(){
    pause();
    fadeIn(0.3);
    loadSong(Filesystem::getFiles(Filesystem::find(Filesystem::RelativePath("music/")), "*"));
    play();
}

#undef synchronized
#undef LOCK
#undef UNLOCK
