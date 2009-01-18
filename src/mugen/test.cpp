#include <cstring>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <vector>
#include <map>

#include "mugen/mugen_reader.h"
#include "mugen/mugen_section.h"
#include "mugen/mugen_item_content.h"
#include "mugen/mugen_item.h"
#include "mugen/mugen_character.h"

using namespace std;


static bool isArg( const char * s1, const char * s2 ){
	return strcasecmp( s1, s2 ) == 0;
}

static void showOptions(){
	cout << "M.U.G.E.N. Config Reader:" << endl;
	cout << "-f <file>: Load a M.U.G.E.N. config file and output to screen." << endl;
	cout << "-c <name>: Load a M.U.G.E.N. character and output some data about it.\n         ie: 'data/mugen/chars/name' only pass name." << endl;
	cout << endl;
}

int main( int argc, char ** argv ){
	
	if(argc <= 1){
	    showOptions();
	    return 0;
	}
	const char * FILE_ARG = "-f";
	const char * LOC_ARG = "-c";
	std::string ourFile;
	bool configLoaded = false;

	//Global::setDebug( 0 );
	
	for ( int q = 1; q < argc; q++ ){
		if ( isArg( argv[ q ], FILE_ARG ) ){
			q += 1;
			if ( q < argc ){
				ourFile = std::string( argv[ q ] );
				configLoaded = !configLoaded;
			}
			else{
			  cout << "Error no file given!" << endl;
			  showOptions();
			  return 0;
			}
		}
		else if ( isArg( argv[ q ], LOC_ARG ) ){
			q += 1;
			if ( q < argc ){
				ourFile = std::string( argv[ q ] );
			}
			else{
			  cout << "Error no file given!" << endl;
			  showOptions();
			  return 0;
			}
		}
		else{
		    // WHAT?
		    showOptions();
		    return 0;
		}
	}
	
	if( configLoaded ){
	    MugenReader reader( ourFile );
	    
	    std::vector< MugenSection * > collection;
	    
	    try{
		collection = reader.getCollection();
		cout << endl << "---------------------------------------------------------" << endl;
		for( unsigned int i = 0; i < collection.size(); ++i ){
		    cout << collection[i]->getHeader() << endl;
		    cout << "---------------------------------------------------------" << endl;
		    while( collection[i]->hasItems() ){
			MugenItemContent *content = collection[i]->getNext();
			while( content->hasItems() ){
			cout << content->getNext()->query();
			if( content->hasItems() ) cout << ",";
			}
			cout << endl;
		    }
		    cout << "---------------------------------------------------------" << endl;
		}
		cout << endl;
	    }
	    catch( MugenException &ex){
		cout << "Problem loading file, error was: " << ex.getReason() << endl;
		return 1;
	    }
	}
	else{
	    try{
		cout << "Trying to load character: " << ourFile << "..." << endl;
		MugenCharacter character( ourFile );
		character.load();
		cout << "Loaded character: \"" << character.getName() << "\" successfully." << endl;
	    }
	    catch( MugenException &ex){
		cout << "Problem loading file, error was: " << ex.getReason() << endl;
		return 1;
	    }
	}
	
	return 0;
}
