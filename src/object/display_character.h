#ifndef _paintown_display_character_h
#define _paintown_display_character_h

#include "character.h"
#include "util/load_exception.h"
#include <pthread.h>
#include <string>
#include <vector>

/* loads some parts of the character to be displayed in
 * the character selection screen. this is faster than
 * loading the entire character + all his animations.
 */
class DisplayCharacterLoader;
class DisplayCharacter: public Character{
public:
	DisplayCharacter(const std::string & str) throw (LoadException);
	virtual ~DisplayCharacter();

        virtual bool isLoaded();

        friend class DisplayCharacterLoader;
protected:
        /* called by the loader */
        void load() throw (LoadException);

private:
        const std::string path;
        bool loaded;
        pthread_mutex_t load_lock;
};

/* loads display characters asynchronously */
class DisplayCharacterLoader{
public:
    /* load all the characters asynchronously */
    DisplayCharacterLoader(const vector<DisplayCharacter*> & characters);

    /* start loading */
    void load();

    /* stop loading */
    void stop();

    /* try to load this character immediately */
    void update(DisplayCharacter* character);

    virtual ~DisplayCharacterLoader();
protected:
    bool done();
    DisplayCharacter * nextCharacter();

    std::vector<DisplayCharacter*> characters;
    pthread_mutex_t data_lock;
    bool forceQuit;
};

#endif
