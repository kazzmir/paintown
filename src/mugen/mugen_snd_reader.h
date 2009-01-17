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
	
	int location;
	
	// Our collection is setup by group and sprite number
	std::map< int, std::map< int, MugenSound * > >collection;
	
	// Add another sound
	void addSound( MugenSound * );
	
	// Get next sound data
	void getNext();
};

#endif
