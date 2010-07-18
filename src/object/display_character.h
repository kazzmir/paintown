#ifndef _paintown_display_character_h
#define _paintown_display_character_h

#include "character.h"
#include "util/load_exception.h"
#include <string>
#include <vector>
#include "util/thread.h"

/* loads some parts of the character to be displayed in
 * the character selection screen. this is faster than
 * loading the entire character + all his animations.
 */
class DisplayCharacterLoader;
class DisplayCharacter: public Character{
public:
	DisplayCharacter(const std::string & str);
	virtual ~DisplayCharacter();

        virtual bool isLoaded();

        friend class DisplayCharacterLoader;
protected:
        /* called by the loader */
        virtual void load();

private:
        const std::string path;
        bool loaded;
        Util::Thread::Lock load_lock;
};

/* loads display characters asynchronously */
class DisplayCharacterLoader{
public:
    /* load all the characters asynchronously */
    DisplayCharacterLoader(const std::vector<DisplayCharacter*> & characters);

    /* start loading */
    void load();

    /* stop loading */
    void stop();
    
    /* done loading */
    bool done();

    /* try to load this character immediately */
    void update(DisplayCharacter* character);

    virtual ~DisplayCharacterLoader();
protected:
    DisplayCharacter * nextCharacter();

    std::vector<DisplayCharacter*> characters;
    Util::Thread::Lock data_lock;
    bool forceQuit;
};

#endif
