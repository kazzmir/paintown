#ifndef _paintown_argument_h
#define _paintown_argument_h

#include "r-tech1/argument.h"
#include "r-tech1/pointer.h"

namespace Paintown{
    std::vector< ::Util::ReferenceCount<Argument::Parameter> > arguments();
}

#endif
