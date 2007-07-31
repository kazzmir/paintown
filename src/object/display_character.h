#ifndef _paintown_display_character_h
#define _paintown_display_character_h

#include "character.h"
#include "util/load_exception.h"
#include <string>

/* loads some parts of the character to be displayed in
 * the character selection screen. this is faster than
 * loading the entire character + all his animations.
 */
class DisplayCharacter: public Character{
public:
	DisplayCharacter( const std::string & str ) throw( LoadException );

	virtual ~DisplayCharacter();
};

#endif
