#ifndef _mugen_argument_h
#define _mugen_argument_h

#include "util/argument.h"
#include "util/pointer.h"

namespace Mugen{
    std::vector< ::Util::ReferenceCount<Argument> > arguments();
}

#endif
