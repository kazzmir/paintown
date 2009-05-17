#ifndef mugen_reader_h
#define mugen_reader_h

#include <fstream>
#include <string>
#include <vector>
#include "mugen_exception.h"

class MugenSection;

class MugenReader{
public:
	MugenReader( const std::string & file );
	MugenReader( const char * file );

	~MugenReader();
	
	const std::vector< MugenSection * > & getCollection() throw(MugenException);

protected:

	std::ifstream ifile;
	std::string myfile;
	
	std::vector< MugenSection * >collection;
	
	enum SearchState{
	  Section = 0,
	  ContentGet
	};
	
	void addSection( MugenSection * section );
};

#endif
