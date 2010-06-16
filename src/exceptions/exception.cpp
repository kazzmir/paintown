#include "exception.h"
#include <string>
#include <exception>

namespace Exception{

Base::Base(const std::string & file, int line):
file(file),
line(line),
nested(NULL){
}

Base::Base(const std::string & file, int line, const Base & nested):
file(file),
line(line),
nested(nested.copy()){
}

Base::~Base() throw (){
    if (nested){
        delete nested;
    }
}

Base::Base(const Base & copy){
    file = copy.file;
    line = copy.line;
    if (copy.nested != NULL){
        nested = copy.nested->copy();
    }
}

Base::Base(const std::string & file, int line, Base * nested):
file(file),
line(line),
nested(nested){
}

Base * Base::copy() const {
    if (nested != NULL){
        return new Base(file, line, nested->copy());
    } else {
        return new Base(file, line);
    }
}

}
