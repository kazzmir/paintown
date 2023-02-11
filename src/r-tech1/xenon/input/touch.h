#ifndef _rtech1_touch
#define _rtech1_touch

#include "../pointer.h"
#include <vector>

namespace Graphics{
    class Bitmap;
}

/* Manages devices that respond to touch input, such as android/ios
 *
 * A specific area of the screen will correspond to specific buttons. Something like
 * (0, 0) - (10, 10) = up button
 * (0, 10) - (10, 20) = down button
 *
 * When a touch occurs in an area defined with a button then a logical key press will be created.
 *
 * The scroll events will be generated when a touch is registered outside a button area and
 * there is movement of a sufficient amount in one direction.
 */

namespace DeviceInput{

class Touch{
public:
    Touch();
    virtual void poll() = 0;
    virtual ~Touch();

    enum Key{
        Invalid = -1,
        Up = 0,
        Down,
        Left,
        Right,
        Button1,
        Button2,
        Button3,
        Button4,
        Button5,
        Button6,
        ScrollUp,
        ScrollDown,
        ScrollLeft,
        ScrollRight,
        Quit,
        Start
    };

    virtual void setZone(Key key, int x1, int y1, int x2, int y2) = 0;

    virtual void drawTouchOverlay(const Graphics::Bitmap & bitmap) = 0;

    class Event{
    public:
        Event(Key key, bool enabled);

        Key key;
        bool enabled;
    };

    const std::vector<Event> & getEvents();

    std::vector<Event> events;
};

Util::ReferenceCount<Touch> getTouchDevice();

}

#endif
