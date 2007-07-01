#include "object/stimulation.h"
#include "object/object.h"

Stimulation::Stimulation(){
}

Stimulation::Stimulation( const Stimulation & copy ){
}

void Stimulation::stimulate( Object & obj ) const {
}

void Stimulation::stimulate( Character & obj ) const {
}
	
Stimulation * Stimulation::copy() const {
	return new Stimulation( *this );
}

Stimulation::~Stimulation(){
}
