#ifndef _paintown_script_object_h
#define _paintown_script_object_h

class Object;
namespace Script{

    /* mirrors an enemy in the game */
    class Object{
    public:
        Object(::Object * const guy);
        virtual ~Object();

        virtual void takeDamage(Object * him, int damage);
        virtual void tick();
        virtual void collided(Object * him);

        virtual inline ::Object* const getObject(){
            return guy;
        }

        /* should really be protected */
    public:
        virtual inline void * getHandle() const {
            return handle;
        }

    protected:
        /* opaque object that the engine used to communicate between
         * the paintown engine and the script engine
         */
        void * handle;
        ::Object * const guy;
    };
}

#endif
