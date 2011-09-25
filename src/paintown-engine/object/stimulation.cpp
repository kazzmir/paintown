#include "stimulation.h"
#include "object.h"
#include "util/bitmap.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "character.h"
#include "draw-effect.h"

namespace Paintown{

Stimulation::Stimulation(){
}

Stimulation::Stimulation( const Stimulation & copy ){
}

void Stimulation::stimulate( Object & obj ) const {
}

void Stimulation::stimulate( Character & obj ) const {
}
    
Stimulation * Stimulation::create(const Token & token){
    try{
        const Token * child;
        token.view() >> child;
        if (*child == "health"){
            return new HealthStimulation(*child);
        }
        if (*child == "invincibility"){
            return new InvincibilityStimulation(*child);
        }
        if (*child == "speed"){
            return new SpeedStimulation(*child);
        }
        Global::debug(0) << "Unknown stimulation type: " << child->getName() << std::endl;
    } catch (const TokenException & fail){
        Global::debug(0) << "Could not parse stimulation: " << fail.getTrace() << std::endl;
    }
    return NULL;
}

Stimulation * Stimulation::create(Network::Message & message){
    int type = 0;
    message >> type;
    switch (type){
        case Health: return new HealthStimulation(message);
        case Invincibility: return new InvincibilityStimulation(message);
        case Speed: return new SpeedStimulation(message);
    }
    return NULL;
}

Stimulation * Stimulation::copy() const {
	return new Stimulation(*this);
}

void Stimulation::createMessage(Network::Message & message) const {
}

/*
std::string Stimulation::typeToName(Type type){
    switch (type){
        case Health: return "health";
        case Invincibility: return "invincibility";
        case Speed: return "speed";
    }

    return "";
}
*/

Stimulation::~Stimulation(){
}

HealthStimulation::HealthStimulation( int value ):
value(value){
}
    
HealthStimulation::HealthStimulation(const Token & data){
    if (!data.match("_", value)){
        throw TokenException(__FILE__, __LINE__, "Expected an integer following health: (health 10)");
    }
}

HealthStimulation::HealthStimulation( const HealthStimulation & h ):
value(h.value){
}
    
HealthStimulation::HealthStimulation(Network::Message & message){
    message >> value;
}
	
void HealthStimulation::stimulate( Object & o ) const {
}
	
void HealthStimulation::stimulate( Character & c ) const {
    /* cause negative hurt */
    c.hurt(-value);
    c.addEffect(new DrawCountdownEffect(new DrawGlowEffect(&c, Graphics::makeColor(50,50,0), Graphics::makeColor(190, 190, 20), 50), 150));
}

void HealthStimulation::createMessage(Network::Message & message) const {
    message << Stimulation::Health << value;
}

Stimulation * HealthStimulation::copy() const {
    return new HealthStimulation(*this);
}

InvincibilityStimulation::InvincibilityStimulation(int duration):
duration(duration){
    if (duration < 0){
        duration = 0;
    }
}
    
InvincibilityStimulation::InvincibilityStimulation(Network::Message & data){
    data >> duration;
}
    
InvincibilityStimulation::InvincibilityStimulation(const Token & data){
    if (!data.match("_", duration)){
        throw TokenException(__FILE__, __LINE__, "Expected an integer following invincibility: (invincibility 10)");
    }
}

InvincibilityStimulation::InvincibilityStimulation(const InvincibilityStimulation & copy):
duration(copy.duration){
}

void InvincibilityStimulation::stimulate(Object & o) const {
}

void InvincibilityStimulation::stimulate(Character & guy) const {
    guy.setInvincibility(duration);
}

Stimulation * InvincibilityStimulation::copy() const {
    return new InvincibilityStimulation(duration);
}

void InvincibilityStimulation::createMessage(Network::Message & message) const {
    message << Stimulation::Invincibility << duration;
}

SpeedStimulation::SpeedStimulation(double boost, int ticks):
boost(boost),
ticks(ticks){
    if (boost < 0){
        boost = 0;
    }
}
    
SpeedStimulation::SpeedStimulation(const Token & data){
    if (!data.match("_", boost, ticks)){
        throw TokenException(__FILE__, __LINE__, "Expected two numbers after speed: (speed 1.2 20)");
    }
    if (boost < 0){
        boost = 0;
    }
}
    
SpeedStimulation::SpeedStimulation(Network::Message & data){
    int value = 0;
    data >> value >> ticks;
    boost = value / 100.0;
}

SpeedStimulation::SpeedStimulation(const SpeedStimulation & copy):
boost(copy.boost),
ticks(copy.ticks){
}

void SpeedStimulation::stimulate(Object & o) const {
}

void SpeedStimulation::stimulate(Character & guy) const {
    guy.setSpeedBoost(boost, ticks);
    guy.addEffect(new DrawCountdownEffect(new DrawGlowEffect(&guy, Graphics::makeColor(64,10,0), Graphics::makeColor(190, 30, 20), 50), ticks));
}

Stimulation * SpeedStimulation::copy() const {
    return new SpeedStimulation(boost, ticks);
}

void SpeedStimulation::createMessage(Network::Message & message) const {
    message << Stimulation::Speed << (int) (boost * 100) << ticks;
}

}
