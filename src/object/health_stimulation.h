#ifndef _paintown_health_stimulation_
#define _paintown_health_stimulation_

#include "stimulation.h"

class Character;

class HealthStimulation : public Stimulation {
public:
	HealthStimulation( int value );
	HealthStimulation( const HealthStimulation & h );

	virtual void stimulate( Object & o ) const;
	virtual void stimulate( Character & c ) const;
	virtual Stimulation * copy() const;

protected:
	int value;
};

#endif
