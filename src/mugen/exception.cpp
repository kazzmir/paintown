#include <exception>
#include <string>
#include <sstream>
#include "mugen/exception.h"
	
MugenException::MugenException():
std::exception(){
}

MugenException::MugenException(const std::string & reason, const std::string & where, int line):
std::exception(),
reason(reason),
where(where),
line(line){
}
        
const std::string MugenException::getFullReason() const {
    std::ostringstream out;
    out << where << ":" << line << " " << reason;
    return out.str();
}
	
MugenException::~MugenException() throw() {
}
