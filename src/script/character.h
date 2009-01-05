#ifndef _paintown_script_character_h
#define _paintown_script_character_h

namespace Script{

    /* mirrors an enemy in the game */
    class Character{
    public:
        Character(::Character * guy);
        virtual ~Character();
    private:
        /* opaque object that the engine used to communicate between
         * the paintown engine and the script engine
         */
        void * handle;
        ::Character * guy;
    };

}

#endif
