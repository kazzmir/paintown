#ifndef mugen_config_exception_h
#define mugen_config_exception_h

#include <exception>
#include <string>

using namespace std;

class MugenConfigException : public exception {
public:
	MugenConfigException();
	MugenConfigException( const string & reason );

	~MugenConfigException() throw();

	inline const string & getReason() const{
		return reason;
	}

protected:
	string reason;

};

#endif
