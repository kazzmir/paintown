#ifdef USE_SDL2

#include "r-tech1/pointer.h"
#include "r-tech1/input/touch.h"

Util::ReferenceCount<DeviceInput::Touch> DeviceInput::getTouchDevice(){
    return Util::ReferenceCount<DeviceInput::Touch>();
}

#endif
