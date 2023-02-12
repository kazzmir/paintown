#ifndef _mugen_argument_h
#define _mugen_argument_h

#include "r-tech1/argument.h"
#include "r-tech1/pointer.h"

namespace Mugen{
    std::vector< ::Util::ReferenceCount<Argument::Parameter> > arguments();
}

#endif
