#ifndef mugen_exception_h
#define mugen_exception_h

#include <exception>
#include <string>

class MugenException: public std::exception {
public:
	MugenException();
	MugenException(const std::string & reason, const std::string & where = "?", int line = 0);

	virtual ~MugenException() throw();

        const std::string getFullReason() const;

	inline const std::string & getReason() const{
            return reason;
	}

protected:
        std::string reason;
        std::string where;
        int line;
};

#endif
