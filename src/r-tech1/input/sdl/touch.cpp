#include "r-tech1/pointer.h"
#include "r-tech1/input/touch.h"

Util::ReferenceCount<DeviceInput::Touch> getTouchDevice(){
    return Util::ReferenceCount<DeviceInput::Touch>();
}
