#ifndef paintown_mugen_game_h
#define paintown_mugen_game_h

#include <string>

#include "mugen/mugen_util.h"

class Bitmap;
class MugenStage;

namespace Mugen{

/* Our game definition, this is to facilitate running a game */
class Game {
    public:
	Game(GameType type, const std::string & systemFile);
	virtual ~Game();
	
	/*! Runs everything */
	virtual void run();
    private:
#if 0
	/*! Run the actual game */
	void runGame(MugenStage * stage, const Bitmap & screen);
	/*! Setup the stage and players */
	MugenStage * setupStage(Mugen::SelectedChars * gameInfo);
#endif
	/*! Game type */
	GameType type;
	/*! System file */
	std::string systemFile;
	/*! Motif Base Directory */
	std::string motifDirectory;
	/*! How many players for team1 */
	int team1;
	/*! How many players for team2 */
	int team2;
};

}


#endif

