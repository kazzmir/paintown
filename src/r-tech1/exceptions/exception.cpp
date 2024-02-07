#include "r-tech1/exceptions/exception.h"
#include <string>
#include <exception>
#include <sstream>

namespace Exception{

Base::Base(const std::string & file, int line, const std::string & reason):
file(file),
reason(reason),
line(line),
nested(NULL){
}

Base::Base(const std::string & file, int line, const Base & nested):
file(file),
reason("no reason given"),
line(line),
nested(nested.copy()){
}

Base::~Base() throw (){
    if (nested){
        delete nested;
    }
}
    
const std::string Base::getReason() const { 
    return this->reason;
}
    
const std::string Base::getTrace() const {
    std::ostringstream out;
    out << file << ":" << line << " " << getReason();
    if (nested != NULL){
        out << "\n";
        out << nested->getTrace();
    }
    return out.str();
}

Base::Base(const Base & copy):
file(copy.file),
line(copy.line),
nested(NULL){
    if (copy.nested != NULL){
        nested = copy.nested->copy();
    }
}

void Base::set(const Base & him){
    nested = him.copy();
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

void Return::throwSelf() const {
    throw *this;
}

Base * Return::copy() const {
    return new Return(*this);
}

Quit::Quit(const std::string & file, int line):
Base(file, line){
}

Quit::Quit(const std::string & file, int line, const Base & nested):
Base(file, line, nested){
}

Quit::~Quit() throw(){
}

void Quit::throwSelf() const {
    throw *this;
}

Base * Quit::copy() const {
    return new Quit(*this);
}

FontException::FontException(const std::string & file, int line, const std::string & reason):
Base(file, line),
reason(reason){
}

FontException::FontException(const std::string & file, int line, const Base & nested, const std::string & reason):
Base(file, line, nested),
reason(reason){
}

FontException::~FontException() throw(){
}
    
const std::string FontException::getReason() const {
    return reason;
}

void FontException::throwSelf() const {
    throw *this;
}

Base * FontException::copy() const {
    return nullptr;
}

}
