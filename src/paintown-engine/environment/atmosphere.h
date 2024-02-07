#ifndef _paintown_atmosphere_h
#define _paintown_atmosphere_h

namespace Graphics{
class Bitmap;
}

#include <string>
#include <vector>

namespace Paintown{
    class Object;
}

class Scene;
class Token;
class Atmosphere{
protected:
    Atmosphere();
public:
    virtual ~Atmosphere();

    /* drawn in front of all background things but before objects */
    virtual void drawBackground(const Graphics::Bitmap & work, int x) = 0;

    /* drawn in front of all objects. you might think this method should
     * be called drawFront, but its called drawForeground because its
     * in front of the background. drawFront is just for drawing on
     * what the scene calls foreground sprites.
     */
    virtual void drawForeground(const Graphics::Bitmap & work, int x) = 0;

    /* drawn only on the foreground */
    virtual void drawFront(const Graphics::Bitmap & work, int x) = 0;

    /* drawn on the entire screen */
    virtual void drawScreen(const Graphics::Bitmap & work, int x) = 0;

    virtual void act(const Scene & level, const std::vector<Paintown::Object*>*) = 0;

    /* interpret a trigger's message */
    virtual void interpret(const Token * message) = 0;

    static Atmosphere * createAtmosphere( const std::string & name );
};

#endif
