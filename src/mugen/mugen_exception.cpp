#include <exception>
#include <string>
#include "mugen_exception.h"
	
MugenException::MugenException():
std::exception(){
}

MugenException::MugenException( const std::string & reason ):
std::exception(){
	this->reason = reason;
}
	
MugenException::~MugenException() throw() {
}
