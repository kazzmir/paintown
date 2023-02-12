#include <string>
#include "r-tech1/exceptions/load_exception.h"

using namespace std;

LoadException::LoadException(const string & file, int line, const string & reason):
Exception::Base(file, line),
reason(reason){
}
    
LoadException::LoadException(const std::string & file, int line, const Exception::Base & nested, const std::string & reason):
Exception::Base(file, line, nested),
reason(reason){
}

LoadException::~LoadException() throw (){
}

LoadException::LoadException(const LoadException & copy):
Exception::Base(copy),
reason(copy.reason){
}

Exception::Base * LoadException::copy() const {
    return new LoadException(*this);
}
    
const std::string LoadException::getReason() const {
    return reason;
}
