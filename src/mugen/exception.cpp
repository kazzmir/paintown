#include <string>
#include <sstream>
#include "exception.h"
	
MugenException::MugenException():
Exception::Base("", -1),
reason("unspecified"),
where("?"),
line(0){
}

MugenException::MugenException(const std::string & reason, const std::string & where, int line):
Exception::Base(where, line),
reason(reason),
where(where),
line(line){
}
        
Exception::Base * MugenException::copy() const {
    return new MugenException(reason, where, line);
}
        
const std::string MugenException::getFullReason() const {
    std::ostringstream out;
    out << where << ":" << line << " " << reason;
    return out.str();
}
	
MugenException::~MugenException() throw() {
}

ReloadMugenException::ReloadMugenException(){
}

ReloadMugenException::~ReloadMugenException() throw(){
}
    
CanceledException::CanceledException(){
}

CanceledException::~CanceledException() throw(){
}

QuitGameException::QuitGameException():
MugenException("Quit game", __FILE__, __LINE__){
}

QuitGameException::~QuitGameException() throw (){
}
    
MugenRuntimeException::MugenRuntimeException(){
}

MugenRuntimeException::MugenRuntimeException(const std::string & reason, const std::string & where, int line):
MugenException(reason, where, line){
}

MugenNormalRuntimeException::MugenNormalRuntimeException(){
}

MugenNormalRuntimeException::MugenNormalRuntimeException(const std::string & reason, const std::string & where, int line):
MugenRuntimeException(reason, where, line){
}

MugenFatalRuntimeException::MugenFatalRuntimeException(){
}

MugenFatalRuntimeException::MugenFatalRuntimeException(const std::string & reason, const std::string & where, int line):
MugenRuntimeException(reason, where, line){
}

