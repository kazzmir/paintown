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

using namespace std;


static bool isArg( const char * s1, const char * s2 ){
	return strcasecmp( s1, s2 ) == 0;
}

static void showOptions(){
	cout << "Mugen Config Reader" << endl;
	cout << "-f <file>: Load a Mugen config file." << endl;
	cout << endl;
}

int main( int argc, char ** argv ){
	
	if(argc <= 1){
	    showOptions();
	    return 0;
	}
	const char * FILE_ARG = "-f";
	std::string ourFile;

	//Global::setDebug( 0 );
	
	for ( int q = 1; q < argc; q++ ){
		if ( isArg( argv[ q ], FILE_ARG ) ){
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
	}
	
	//MugenReader reader( ourFile );
	
	//std::vector< MugenSection > collection;
	/*
	try{
	  collection = reader.getCollection();
	  cout << endl << "---------------------------------------------------------" << endl;
	  for( unsigned int i = 0; i < collection.size(); ++i ){
	    cout << collection[i].getHeader() << endl;
	    cout << "---------------------------------------------------------" << endl;
	    while( collection[i].hasItems() ){
		MugenItemContent content = collection[i].getNext();
		while( content.hasItems() ){
		  cout << content.getNext().query() << ",";
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
	*/
	
	MugenSection test;
	std::string st = "stuff";
	test.setHeader( st );
	MugenItemContent cont;
	cont << "one";
	cont << "two";
	cont << "three";
	cont << "four";
	test << &cont;
	cont.clear();
	cont << "five";
	cont << "six";
	cont << "seven";
	cont << "eight";
	test << &cont;
	
	while( test.hasItems() ){
		const MugenItemContent *content = test.getNext();
		while( content->hasItems() ){
		  const MugenItem *temp = content->getNext();
		  cout << temp->query() << ",";
		}
		cout << endl;
	    }
	    cout << endl;
	
	return 0;
}
