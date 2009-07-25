#include <exception>
#include <string>
#include "parse-exception.h"

namespace Mugen{

ParserException::ParserException(const std::string & reason):
exception(),
reason(reason){
}

ParserException::~ParserException() throw (){
}

}
