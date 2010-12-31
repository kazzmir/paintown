#include "stimulation.h"
#include "object.h"

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

Stimulation::~Stimulation(){
}

}
