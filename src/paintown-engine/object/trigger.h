#ifndef _paintown_object_trigger_h
#define _paintown_object_trigger_h

#include <vector>

namespace Paintown{

class Object;
enum TriggerType{
    OnDeath
};

class Trigger{
public:
    Trigger();
    virtual Trigger * copy() const = 0;
    virtual void invoke(std::vector<Object*> & add) = 0;
    virtual ~Trigger();
};

}

#endif
