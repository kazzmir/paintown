#ifndef _Parser_H
#define _Parser_H

#include <vector>

using namespace std;

struct token{
	vector< string > inputs;
	vector< token * > tokens;
	token * parent;
};

token * parseFile( string file );

#endif
