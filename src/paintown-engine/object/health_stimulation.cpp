#include "util/bitmap.h"
#include "stimulation.h"
#include "health_stimulation.h"
#include "character.h"
#include "draw-effect.h"

namespace Paintown{

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
        c.addEffect(new DrawCountdownEffect(new DrawGlowEffect(&c, Graphics::makeColor(50,50,0), Graphics::makeColor(190, 190, 20), 50), 150));
}

void HealthStimulation::createMessage( Network::Message & message ) const {
	message << value;
}

Stimulation * HealthStimulation::copy() const {
	return new HealthStimulation( *this );
}

}
