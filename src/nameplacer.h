#ifndef _name_placer_h
#define _name_placer_h

#include <map>

using namespace std;

class NamePlacer{
public:

	static void getPlacement( int & x, int & y, int & id );
	static void destroy();

protected:
	NamePlacer();
	void doPlacement( int & x, int & y, int & id );

protected:
	static NamePlacer * placer;
	int cur_id;

	/* id -> timer */
	map< int, int > places;

};

#endif
