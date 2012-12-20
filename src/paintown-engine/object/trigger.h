#ifndef _paintown_trigger_h
#define _paintown_trigger_h

namespace Paintown{

enum TriggerType{
    OnDeath
};

class Trigger{
public:
    Trigger();
    virtual Trigger * copy() const = 0;
    virtual ~Trigger();
};

}

#endif
