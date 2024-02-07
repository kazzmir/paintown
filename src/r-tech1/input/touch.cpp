#include "touch.h"
#include <vector>

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
