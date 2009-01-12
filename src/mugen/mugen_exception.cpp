#include <exception>
#include <string>
#include "mugen_exception.h"
	
MugenException::MugenException():
exception(){
}

MugenException::MugenException( const string & reason ):
exception(){
	this->reason = reason;
}
	
MugenException::~MugenException() throw() {
}
