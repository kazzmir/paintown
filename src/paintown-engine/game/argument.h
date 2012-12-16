#ifndef _paintown_argument_h
#define _paintown_argument_h

#include "util/argument.h"
#include "util/pointer.h"

namespace Paintown{
    std::vector< ::Util::ReferenceCount<Argument> > arguments();
}

#endif
