#ifndef mugen_exception_h
#define mugen_exception_h

#include "util/exceptions/exception.h"
#include <string>

/* FIXME: use a Mugen namespace here */

/* Base mugen exception */
class MugenException: public Exception::Base {
protected:
    MugenException();
public:
    /* `where' should be __FILE__ and `line' should be __LINE__ */
    MugenException(const std::string & reason, const std::string & where, int line);

    virtual ~MugenException() throw();

    virtual Base * copy() const;

    const std::string getFullReason() const;

    virtual void throwSelf() const {
        throw *this;
    }

    inline const std::string getReason() const{
        return reason;
    }

    const std::string getWhere() const {
        return where;
    }

    int getLine() const {
        return line;
    }

protected:
    std::string reason;
    std::string where;
    int line;
};

class ReloadMugenException: public MugenException {
public:
    ReloadMugenException();
    virtual ~ReloadMugenException() throw();
};

/* Thrown when some action is canceled from the UI */
class CanceledException: public MugenException {
public:
    CanceledException();
    virtual ~CanceledException() throw();
};

/* thrown from the compiler/state-controller when mugen code screws up */
class MugenRuntimeException: public MugenException {
protected:
    MugenRuntimeException();
public:
    MugenRuntimeException(const std::string & reason, const std::string & where, int line);

    virtual void throwSelf() const {
        throw *this;
    }
};

/* For errors that we don't care a whole lot about */
class MugenNormalRuntimeException: public MugenRuntimeException {
protected:
    MugenNormalRuntimeException();
public:
    MugenNormalRuntimeException(const std::string & reason, const std::string & where = "?", int line = 0);

    virtual void throwSelf() const {
        throw *this;
    }
};

/* For errors that are serious */
class MugenFatalRuntimeException: public MugenRuntimeException {
protected:
    MugenFatalRuntimeException();
public:
    MugenFatalRuntimeException(const std::string & reason, const std::string & where, int line);

    virtual void throwSelf() const {
        throw *this;
    }
};

#endif
