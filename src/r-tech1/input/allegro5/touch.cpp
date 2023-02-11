#ifdef USE_ALLEGRO5

#include "r-tech1/input/touch.h"
#include "r-tech1/pointer.h"
#include "r-tech1/debug.h"
#include "r-tech1/graphics/bitmap.h"
#include <allegro5/allegro.h>
#include <map>

namespace DeviceInput{

class TouchTrack{
public:
    TouchTrack(int id, double x, double y):
    id(id),
    x(x),
    y(y),
    last_x(x),
    last_y(y){
    }

    void update(double x, double y){
        this->last_x = this->x;
        this->last_y = this->y;
        this->x = x;
        this->y = y;
    }

    int id;
    double x;
    double y;
    double last_x;
    double last_y;
};

class Allegro5Touch: public Touch {
public:
    Allegro5Touch();
    virtual void poll();
    virtual ~Allegro5Touch();

    void handleTouch(const Util::ReferenceCount<TouchTrack> & touch, std::map<Key, bool> & buttons, bool press);
    void generateEvents(std::map<Key, bool> & new_buttons);

    void setZone(Key key, int x1, int y1, int x2, int y2){
        zones[key] = Zone(x1, y1, x2, y2);
    }

    bool inZone(Key key, double x, double y);

    virtual void drawTouchOverlay(const Graphics::Bitmap & bitmap);

    struct Zone{
        Zone():
        x1(-1),
        y1(-1),
        x2(-1),
        y2(-1){
        }

        Zone(int x1, int y1, int x2, int y2):
        x1(x1),
        y1(y1),
        x2(x2),
        y2(y2){
        }

        int x1, y1, x2, y2;
    };

protected:
    ALLEGRO_EVENT_QUEUE * queue;
    std::map<int, Util::ReferenceCount<TouchTrack> > touches;
    std::map<Key, bool> buttons;
    std::map<Key, Zone> zones;
};

Allegro5Touch::Allegro5Touch():
queue(NULL){
    if (al_install_touch_input()){
        queue = al_create_event_queue();
        if (queue != NULL){
            al_register_event_source(queue, al_get_touch_input_event_source());
        }
    }
}
    
bool Allegro5Touch::inZone(Key key, double x, double y){
    // _xdebug << "Check for zone " << key << " in " << x << ", " << y << std::endl;
    const Zone & zone = zones[key];
    return x >= zone.x1 && x <= zone.x2 &&
           y >= zone.y1 && y <= zone.y2;
}

static std::vector<Touch::Key> allKeys(){
    std::vector<Touch::Key> keys;
    keys.push_back(Touch::Up);
    keys.push_back(Touch::Down);
    keys.push_back(Touch::Left);
    keys.push_back(Touch::Right);
    keys.push_back(Touch::Button1);
    keys.push_back(Touch::Button2);
    keys.push_back(Touch::Button3);
    keys.push_back(Touch::Button4);
    keys.push_back(Touch::Button5);
    keys.push_back(Touch::Button6);
    keys.push_back(Touch::Quit);
    keys.push_back(Touch::Start);
    return keys;
}

void Allegro5Touch::handleTouch(const Util::ReferenceCount<TouchTrack> & touch, std::map<Key, bool> & new_buttons, bool press){
    std::vector<Key> all = allKeys();
    for (std::vector<Key>::iterator it = all.begin(); it != all.end(); it++){
        if (inZone(*it, touch->x, touch->y)){
            new_buttons[*it] = press;
        }
    }
}

/* Generate a 'pressed' event if the current state was not pressd and the new state is pressed.
 * Similarly, generate a 'release' event if the current state was pressed and the new state was not pressed
 *
 */
void Allegro5Touch::generateEvents(std::map<Key, bool> & new_buttons){
    std::vector<Key> all = allKeys();
    for (std::vector<Key>::iterator it = all.begin(); it != all.end(); it++){
        Key key = *it;
        if (buttons[key] ^ new_buttons[key]){
            events.push_back(Event(key, new_buttons[key]));
        }
    }
}

void Allegro5Touch::poll(){
    if (queue == NULL){
        return;
    }
    events.clear();
    std::map<Key, bool> new_buttons;
    ALLEGRO_EVENT event;
    while (al_get_next_event(queue, &event)){
        switch (event.type){
            case ALLEGRO_EVENT_TOUCH_BEGIN: {
                // Global::debug(0) << "Touch begin id (" << event.touch.id << ") x (" << (double) event.touch.x << ") y (" << (double) event.touch.y << ")" << std::endl;
                int id = event.touch.id;
                double x = event.touch.x;
                double y = event.touch.y;

                touches[id] = Util::ReferenceCount<TouchTrack>(new TouchTrack(id, x, y));
                handleTouch(touches[id], new_buttons, true);

                break;
            }
            case ALLEGRO_EVENT_TOUCH_END: {
                int id = event.touch.id;
                double x = event.touch.x;
                double y = event.touch.y;

                handleTouch(touches[id], new_buttons, false);
                touches[id] = NULL;

                // Global::debug(0) << "Touch end id (" << event.touch.id << ") x (" << (double) event.touch.x << ") y (" << (double) event.touch.y << ")" << std::endl;
                break;
            }
            case ALLEGRO_EVENT_TOUCH_MOVE: {
                int id = event.touch.id;
                double x = event.touch.x;
                double y = event.touch.y;

                if (touches[id] != NULL){
                    touches[id]->update(x, y);
                } else {
                    touches[id] = Util::ReferenceCount<TouchTrack>(new TouchTrack(id, x, y));
                }
                
                handleTouch(touches[id], new_buttons, true);

                // Global::debug(0) << "Touch move id (" << event.touch.id << ") x (" << (double) event.touch.x << ") y (" << (double) event.touch.y << ")" << std::endl;
                break;
            }
            case ALLEGRO_EVENT_TOUCH_CANCEL: {
                // Global::debug(0) << "Touch cancel id (" << event.touch.id << ") x (" << (double) event.touch.x << ") y (" << (double) event.touch.y << ")" << std::endl;
                int id = event.touch.id;
                handleTouch(touches[id], new_buttons, false);
                touches[id] = NULL;
                break;
            }
        }
    }

    generateEvents(new_buttons);
}
    
void Allegro5Touch::drawTouchOverlay(const Graphics::Bitmap & bitmap){
    Graphics::TranslucentBitmap translucent = bitmap.translucent(0, 0, 0, 96);
    for (std::map<Key, Zone>::iterator it = zones.begin(); it != zones.end(); it++){
        Key key = it->first;
        Zone zone = it->second;

        translucent.rectangleFill(zone.x1, zone.y1, zone.x2, zone.y2, Graphics::makeColor(128, 128, 200));
    }
}

Allegro5Touch::~Allegro5Touch(){
    if (queue != NULL){
        al_destroy_event_queue(queue);
        queue = NULL;
    }
}

Util::ReferenceCount<Touch> getTouchDevice(){
    return Util::ReferenceCount<Touch>(new Allegro5Touch());
}

}

#endif
