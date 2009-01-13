#ifndef mugen_reader_h
#define mugen_reader_h

#include <fstream>
#include <string>
#include <vector>
#include "mugen_exception.h"

class MugenSection;

class MugenReader{
public:
	MugenReader( const string & s );
	MugenReader( const char * filename );

	~MugenReader();
	
	const std::vector< MugenSection > & getCollection() throw(MugenException);

protected:

	std::ifstream ifile;
	std::string myfile;
	
	std::vector< MugenSection >collection;
	
	enum SearchState{
	  Section = 0,
	  ContentGet,
	  ContentNext
	};
	
	void addSection( MugenSection section );
};

#endif
