#ifndef _paintown_stimulation_h
#define _paintown_stimulation_h

#include <string>

class Token;

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
        Invincibility,
        Speed
    };

    /* translate type to the string name so a block object can be made */
    // static std::string typeToName(Type type);

    /* create from a token */
    static Stimulation * create(const Token & token);

    /* create from a message */
    static Stimulation * create(Network::Message & message);

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
    HealthStimulation(const Token & data);
    HealthStimulation(Network::Message & message);
    HealthStimulation(const HealthStimulation & h);

    virtual void stimulate(Object & o) const;
    virtual void stimulate(Character & c) const;
    virtual Stimulation * copy() const;
    virtual void createMessage(Network::Message & message) const;

protected:
    int value;
};

/* makes a player temporarily invincibile */
class InvincibilityStimulation: public Stimulation {
public:
    InvincibilityStimulation(int duration);
    InvincibilityStimulation(const Token & data);
    InvincibilityStimulation(Network::Message & data);
    InvincibilityStimulation(const InvincibilityStimulation & copy);

    virtual void stimulate(Object & o) const;
    virtual void stimulate(Character & c) const;
    virtual Stimulation * copy() const;
    virtual void createMessage(Network::Message & message) const;

protected:
    int duration;
};

/* adds speed to a player */
class SpeedStimulation: public Stimulation {
public:
    SpeedStimulation(double boost, int ticks);
    SpeedStimulation(const Token & data);
    SpeedStimulation(Network::Message & data);
    SpeedStimulation(const SpeedStimulation & copy);

    virtual void stimulate(Object & o) const;
    virtual void stimulate(Character & c) const;
    virtual Stimulation * copy() const;
    virtual void createMessage(Network::Message & message) const;

protected:
    double boost;
    int ticks;
};

}

#endif
