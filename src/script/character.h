#ifndef _paintown_script_character_h
#define _paintown_script_character_h

#include "object.h"

class Character;
namespace Script{

    /* mirrors an enemy in the game */
    class Character: public Script::Object{
    public:
        Character(::Character * const guy);
        virtual ~Character();

        virtual void attacked(Object * who);

        virtual inline ::Character* const getCharacter(){
            return (::Character*) getObject();
        }
    };
}

#endif
