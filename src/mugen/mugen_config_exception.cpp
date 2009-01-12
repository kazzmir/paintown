#include <exception>
#include <string>
#include "mugen_config_exception.h"
	
MugenConfigException::MugenConfigException():
exception(){
}

MugenConfigException::MugenConfigException( const string & reason ):
exception(){
	this->reason = reason;
}
	
MugenConfigException::~MugenConfigException() throw() {
}
