#ifndef _paintown_stimulation_h
#define _paintown_stimulation_h

#include <string>

namespace Network{
    struct Message;
}

namespace Paintown{

class Object;
class Character;

class Stimulation{
public:
    Stimulation();
    Stimulation(const Stimulation & copy );

    /* for network mode to translate types back to objects */
    enum Type{
        Health,
        Invincibility
    };

    /* translate type to the string name so a block object can be made */
    static std::string typeToName(Type type);

    virtual void stimulate(Object & o) const;
    virtual void stimulate(Character & c) const;
    virtual Stimulation * copy() const;
    virtual void createMessage(Network::Message & message) const;

    virtual ~Stimulation();
};

/* adds health to a player */
class HealthStimulation: public Stimulation {
public:
    HealthStimulation(int value);
    HealthStimulation(const HealthStimulation & h);

    virtual void stimulate(Object & o) const;
    virtual void stimulate(Character & c) const;
    virtual Stimulation * copy() const;
    virtual void createMessage(Network::Message & message) const;

protected:
    int value;
};

class InvincibilityStimulation: public Stimulation {
public:
    InvincibilityStimulation(int duration);
    InvincibilityStimulation(const InvincibilityStimulation & copy);

    virtual void stimulate(Object & o) const;
    virtual void stimulate(Character & c) const;
    virtual Stimulation * copy() const;
    virtual void createMessage(Network::Message & message) const;

protected:
    int duration;
};

}

#endif
