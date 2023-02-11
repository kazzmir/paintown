#ifndef _rtech1_action_speed_h
#define _rtech1_action_speed_h

class Token;

class ActionSpeed{
public:
    // Do whatever is needed for the requested item
    virtual void act();
    ActionSpeed(const Token *token);
    virtual ~ActionSpeed();

private:
    // speed
    double speed;
};

#endif
