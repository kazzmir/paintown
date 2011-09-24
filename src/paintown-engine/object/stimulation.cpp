#include "stimulation.h"
#include "object.h"
#include "util/bitmap.h"
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
	
Stimulation * Stimulation::copy() const {
	return new Stimulation(*this);
}

void Stimulation::createMessage( Network::Message & message ) const {
}
    
std::string Stimulation::typeToName(Type type){
    switch (type){
        case Health: return "health";
        case Invincibility: return "invincibility";
    }

    return "";
}

Stimulation::~Stimulation(){
}

HealthStimulation::HealthStimulation( int value ):
value(value){
}

HealthStimulation::HealthStimulation( const HealthStimulation & h ):
value(h.value){
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

}
