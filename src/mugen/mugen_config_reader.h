#ifndef mugen_config_reader_h
#define mugen_config_reader_h

#include <fstream>
#include <string>
#include <vector>
#include "mugen_config_exception.h"

using namespace std;

class MugenConfig;

class MugenConfigReader{
public:
	MugenConfigReader( const string & s );
	MugenConfigReader( const char * filename );

	~MugenConfigReader();

protected:

	ifstream ifile;
	string myfile;
};

#endif
