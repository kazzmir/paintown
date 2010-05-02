#include <exception>
#include <string>
#include <sstream>
#include "exception.h"
	
MugenException::MugenException():
std::exception(),
reason("unspecified"),
where("?"),
line(0){
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
