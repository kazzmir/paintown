#include <iostream>
#include <vector>
#include "heart_factory.h"
#include "object/heart.h"
#include "globals.h"
#include "util/debug.h"

using namespace std;

HeartFactory * HeartFactory::my_heart = NULL;
Heart * HeartFactory::createHeart(){
	if ( my_heart == NULL )
		my_heart = new HeartFactory();

	return my_heart->makeHeart();
}

void HeartFactory::destroy(){
    if ( my_heart != NULL ){
        if ( my_heart->clearHearts() ){
            delete my_heart;
            my_heart = NULL;
        } else {
            Global::debug(0) <<"Heart factory has hearts left!"<<endl;
        }
    }
}

Heart * HeartFactory::makeHeart(){
    Heart * h = new Heart();
    hearts.push_back( h );

    return h;
}

bool HeartFactory::clearHearts(){
    for ( vector< Heart * >::iterator it = hearts.begin(); it != hearts.end(); ){
        Heart * k = *it;
        if ( ! k->getAlive() ){
            delete k;
            it = hearts.erase( it );
        } else it++;
    }

    return hearts.empty();
}

HeartFactory::HeartFactory(){
} 

HeartFactory::~HeartFactory(){
}
