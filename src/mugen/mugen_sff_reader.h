#ifndef mugen_sff_reader_h
#define mugen_sff_reader_h

#include <fstream>
#include <string>
#include <map>
#include "mugen_exception.h"

class MugenSprite;

class MugenSffReader{
public:
	MugenSffReader( const string & s );
	MugenSffReader( const char * filename );

	~MugenSffReader();
	
	const std::map< int, std::map< int, MugenSprite * > > & getCollection() throw(MugenException);

protected:

	std::ifstream ifile;
	std::string myfile;
	
	int location;
	
	// Our collection is setup by group and sprite number
	std::map< int, std::map< int, MugenSprite * > >collection;
	
	// Add another sprite
	void addSprite( MugenSprite * );
	
	// Get next sprite
	void getNext();
};

#endif
