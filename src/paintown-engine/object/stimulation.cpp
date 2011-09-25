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
        case Speed: return "speed";
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

SpeedStimulation::SpeedStimulation(double boost):
boost(boost){
    if (boost < 0){
        boost = 0;
    }
}

SpeedStimulation::SpeedStimulation(const SpeedStimulation & copy):
boost(copy.boost){
}

void SpeedStimulation::stimulate(Object & o) const {
}

void SpeedStimulation::stimulate(Character & guy) const {
    /* FIXME: get the ticks from the data somehow */
    int ticks = 90 * 10;
    guy.setSpeedBoost(boost, ticks);
    guy.addEffect(new DrawCountdownEffect(new DrawGlowEffect(&guy, Graphics::makeColor(64,10,0), Graphics::makeColor(190, 30, 20), 50), ticks));
}

Stimulation * SpeedStimulation::copy() const {
    return new SpeedStimulation(boost);
}

void SpeedStimulation::createMessage(Network::Message & message) const {
    /* FIXME: pass (int)(boost*100) to the message and then deconstruct it
     * by dividing by 100 again.
     */
    message << Stimulation::Speed << (int) boost;
}

}
