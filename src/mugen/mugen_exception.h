#ifndef mugen_exception_h
#define mugen_exception_h

#include <exception>
#include <string>

using namespace std;

class MugenException : public exception {
public:
	MugenException();
	MugenException( const string & reason );

	~MugenException() throw();

	inline const string & getReason() const{
		return reason;
	}

protected:
	string reason;

};

#endif
