#ifndef _paintown_stimulation_h
#define _paintown_stimulation_h

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

}

#endif
