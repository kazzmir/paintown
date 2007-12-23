#include "stimulation.h"
#include "health_stimulation.h"
#include "character.h"

HealthStimulation::HealthStimulation( int value ):
value( value ){
}

HealthStimulation::HealthStimulation( const HealthStimulation & h ):
value( h.value ){
}
	
void HealthStimulation::stimulate( Object & o ) const {
}
	
void HealthStimulation::stimulate( Character & c ) const {
	/* cause negative hurt */
	c.hurt( - value );
}

void HealthStimulation::createMessage( Network::Message & message ) const {
	message << value;
}

Stimulation * HealthStimulation::copy() const {
	return new HealthStimulation( *this );
}
