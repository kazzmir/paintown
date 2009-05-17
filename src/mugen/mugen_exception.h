#ifndef mugen_exception_h
#define mugen_exception_h

#include <exception>
#include <string>

class MugenException : public std::exception {
public:
	MugenException();
	MugenException( const std::string & reason );

	~MugenException() throw();

	inline const std::string & getReason() const{
		return reason;
	}

protected:
        std::string reason;
};

#endif
