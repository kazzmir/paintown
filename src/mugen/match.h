#ifndef mugen_match_h
#define mugen_match_h

#include <vector>

#include "util.h"

namespace Mugen{
    
/*! Namespace Match for now to avoid conflicts within the HUD */
namespace Match{
    
enum Side{
    NotAwarded,
    Player1,
    Player2,
    Draw,
};
    
/*! Round data */
class Round{
public:
    Round(int number);
    virtual ~Round();
    
    void act();
    
    void setTime();
    int getTime() const;
    void setWinner(const Side player);
    const Side & getWinner();
    
    inline static void setFramesPerCount(int count){
        Round::framesPerCount = count;
    }
    
    enum State{
        NotStarted,
        Started,
        Ended,
    };
    
protected:
    static int framesPerCount;
    
    const int roundNumber;
    int time;
    int ticker;
    Side winner;
    State state;
};

/*! Mugen Match Information and Data, houses information regarding the rounds and match */
class Manager{
public:
    Manager(const Filesystem::AbsolutePath &);
    virtual ~Manager();
    
    void act();
    
protected:
    /*! Round information
     */
    // Rounds to win
    int maxWins;
    // Max Draw Games
    int maxDrawGames;
    // Current Round
    unsigned int currentRound;
    std::vector<Round> rounds;
};

}
}
#endif