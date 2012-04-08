#ifndef _paintown_display_character_h
#define _paintown_display_character_h

#include "character.h"
#include "util/load_exception.h"
#include "util/file-system.h"
#include <string>
#include <vector>
#include "util/thread.h"

namespace Paintown{

/* loads some parts of the character to be displayed in
 * the character selection screen. this is faster than
 * loading the entire character + all his animations.
 */
class DisplayCharacterLoader;
class DisplayCharacter: public Character {
public:
	DisplayCharacter(const Filesystem::AbsolutePath & str);
	virtual ~DisplayCharacter();

        virtual bool isLoaded();

        friend class DisplayCharacterLoader;
protected:
        /* called by the loader */
        virtual void load();
        virtual void loadDone();

        const Filesystem::AbsolutePath path;
private:
        bool loaded;
        Util::Thread::LockObject load_lock;
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
    Util::Thread::LockObject data_lock;
    bool forceQuit;
};

}

#endif
