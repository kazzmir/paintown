#include "touch.h"
#include <vector>

#ifdef USE_SDL
#include "sdl/touch.cpp"
#endif

namespace DeviceInput{

Touch::Touch(){
}

Touch::~Touch(){
}

const std::vector<Touch::Event> & Touch::getEvents(){
    return events;
}

Touch::Event::Event(Key key, bool enabled):
key(key),
enabled(enabled){
}

}
