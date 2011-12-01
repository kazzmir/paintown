#ifndef mugen_exception_h
#define mugen_exception_h

#include "exceptions/exception.h"
#include <string>

/* FIXME: use a Mugen namespace here */

/* Base mugen exception */
class MugenException: public Exception::Base {
public:
    MugenException();
    MugenException(const std::string & reason, const std::string & where = "?", int line = 0);

    virtual ~MugenException() throw();

    virtual Base * copy() const;

    const std::string getFullReason() const;

    virtual void throwSelf() const {
        throw *this;
    }

    inline const std::string getReason() const{
        return reason;
    }

protected:
    std::string reason;
    std::string where;
    int line;
};

/* thrown from the compiler/state-controller when mugen code screws up */
class MugenRuntimeException: public MugenException {
public:
    MugenRuntimeException();
    MugenRuntimeException(const std::string & reason, const std::string & where = "?", int line = 0);

    virtual void throwSelf() const {
        throw *this;
    }
};

/* For errors that we don't care a whole lot about */
class MugenNormalRuntimeException: public MugenRuntimeException {
public:
    MugenNormalRuntimeException();
    MugenNormalRuntimeException(const std::string & reason, const std::string & where = "?", int line = 0);

    virtual void throwSelf() const {
        throw *this;
    }
};

/* For errors that are serious */
class MugenFatalRuntimeException: public MugenRuntimeException {
public:
    MugenFatalRuntimeException();
    MugenFatalRuntimeException(const std::string & reason, const std::string & where = "?", int line = 0);

    virtual void throwSelf() const {
        throw *this;
    }
};

#endif
