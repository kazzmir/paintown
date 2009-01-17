#ifndef mugen_snd_reader_h
#define mugen_snd_reader_h

#include <fstream>
#include <string>
#include <map>
#include "mugen_exception.h"

class MugenSound;

class MugenSndReader{
public:
	MugenSndReader( const string & s );
	MugenSndReader( const char * filename );

	~MugenSndReader();
	
	const std::map< int, std::map< int, MugenSound * > > & getCollection() throw(MugenException);

protected:

	std::ifstream ifile;
	std::string myfile;
	
	// Our collection is setup by group and sprite number
	std::map< int, std::map< int, MugenSound * > >collection;
	
	// Add another sprite
	void addSound( int group, int number, MugenSound * );
};

#endif
