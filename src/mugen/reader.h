#ifndef mugen_reader_h
#define mugen_reader_h

#include <fstream>
#include <string>
#include <vector>
#include "mugen/exception.h"

class MugenSection;
class MugenItemContent;

class MugenReader{
public:
	MugenReader( const std::string & file );
	MugenReader( const char * file );
	MugenReader( const std::vector<std::string> &text );

	~MugenReader();
	
	const std::vector< MugenSection * > & getCollection();

protected:

	std::ifstream ifile;
	std::string myfile;
	
	// in case we are using text
	std::vector<std::string> configData;
	
	bool usingText;
	
	std::vector< MugenSection * >collection;
	
	enum SearchState{
	  Section = 0,
	  ContentGet
	};
	
	void addSection( MugenSection * section );
	
	void addContent( MugenItemContent *content );
	
	bool isSection( const std::string &line );
	
	bool hasContent( const std::string &line );
	
	MugenSection *extractSection( const std::string &line );
	
	MugenItemContent *extractContent( const std::string &line );
	
};

#endif
