#ifndef _paintown_script_character_h
#define _paintown_script_character_h

class Character;
namespace Script{

    /* mirrors an enemy in the game */
    class Character{
    public:
        Character(::Character * guy);
        virtual ~Character();

        virtual void tick();

        virtual inline const ::Character* getCharacter(){
            return guy;
        }

    private:
        /* opaque object that the engine used to communicate between
         * the paintown engine and the script engine
         */
        void * handle;
        const ::Character * guy;
    };

}

#endif
