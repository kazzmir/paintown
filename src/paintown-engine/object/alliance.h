#ifndef _paintown_alliance_h
#define _paintown_alliance_h

/* These are integers instead of an enum because for a free for all each player will get
 * their own alliance id so an enum cannot capture all the possibilities.
 */
namespace Paintown{
    extern const int ALLIANCE_NONE;
    extern const int ALLIANCE_PLAYER;
    extern const int ALLIANCE_ENEMY;
    extern const int ALLIANCE_FREE_FOR_ALL;
}

#endif
