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

Base::Base(const Base & copy):
file(copy.file),
line(copy.line),
nested(NULL){
    if (copy.nested != NULL){
        nested = copy.nested->copy();
    }
}

Base * Base::copy() const {
    return new Base(*this);
}

Return::Return(const std::string & file, int line):
Base(file, line){
}

Return::Return(const std::string & file, int line, const Base & nested):
Base(file, line, nested){
}

Return::~Return() throw(){
}

Base * Return::copy() const {
    return new Return(*this);
}

}
