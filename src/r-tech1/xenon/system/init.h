#ifndef _rtech1_system_init_h
#define _rtech1_system_init_h

#include "r-tech1/debug.h"
#include "r-tech1/init.h"

namespace System{
    void initSystem(const Global::InitConditions & conditions, Global::stream_type & out);
}

#endif
