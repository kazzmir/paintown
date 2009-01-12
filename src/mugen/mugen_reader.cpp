#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "mugen_section.h"
#include "mugen_reader.h"

using namespace std;

MugenReader::MugenReader( const char * file ){
	
}

MugenReader::MugenReader( const string & file ){
	
}

MugenReader::~MugenReader(){
	
}

const std::vector< MugenSection > & MugenReader::getCollection() throw(MugenException){
  
  
  return collection;
}



