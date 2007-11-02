#include "util/bitmap.h"
#include "atmosphere.h"
#include "rain_atmosphere.h"
#include "snow_atmosphere.h"
#include "night_atmosphere.h"
#include "fog_atmosphere.h"

Atmosphere::Atmosphere(){
}
	
Atmosphere::~Atmosphere(){
}
	
Atmosphere * Atmosphere::createAtmosphere( const std::string & name ){
	if ( name == "rain" ){
		return new RainAtmosphere();
	}
	if ( name == "snow" ){
		return new SnowAtmosphere();
	}
	if ( name == "night" ){
		return new NightAtmosphere();
	}
	if ( name == "fog" ){
		return new FogAtmosphere();
	}

	return NULL;
}
