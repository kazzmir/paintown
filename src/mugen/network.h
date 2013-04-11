#ifdef HAVE_NETWORKING

#include "stage.h"

namespace Mugen{

class NetworkObserver: public StageObserver {
public:
    NetworkObserver();

    virtual ~NetworkObserver();

    virtual void start() = 0;
    virtual void kill() = 0;
};

}

#endif
