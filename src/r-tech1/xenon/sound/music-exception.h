#ifndef _rtech1_music_exception_h
#define _rtech1_music_exception_h

#include "../exceptions/exception.h"

namespace Util{

class MusicException: public Exception::Base {
public:
    MusicException(const std::string & file, int line, const std::string & reason):
        Exception::Base(file, line),
        reason(reason){
        }

    MusicException(const MusicException & copy):
    Exception::Base(copy),
    reason(copy.reason){
    }

    virtual ~MusicException() throw(){
    }

protected:
    virtual const std::string getReason() const {
        return reason;
    }

    virtual Exception::Base * copy() const {
        return new MusicException(*this);
    }

    std::string reason;
};

}

#endif
