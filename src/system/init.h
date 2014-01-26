#ifndef _paintown_system_init_h
#define _paintown_system_init_h

#include "util/debug.h"
#include "util/init.h"

namespace System{
    void initSystem(const Global::InitConditions & conditions, Global::stream_type & out);
}

#endif
