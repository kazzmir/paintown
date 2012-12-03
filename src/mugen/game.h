#ifndef paintown_mugen_game_h
#define paintown_mugen_game_h

#include <string>

#include "util.h"
#include "util/gui/fadetool.h"

namespace Graphics{
class Bitmap;
}

namespace Mugen{

class HumanBehavior;
class Searcher;
class CharacterSelect;
class Stage;

class RunMatchOptions{
public:
    RunMatchOptions();
    // For demo mode
    RunMatchOptions(int endTime);
    RunMatchOptions(const RunMatchOptions & copy);
    const RunMatchOptions & operator=(const RunMatchOptions & copy);
    void act();
    void draw(const Graphics::Bitmap & work);
    bool isDemoMode() const;
    void setBehavior(HumanBehavior * player1, HumanBehavior * player2);
    HumanBehavior * getPlayer1Behavior() const;
    HumanBehavior * getPlayer2Behavior() const;

protected:
    int ticker;
    int endTime;
    bool demoMode;
    Gui::FadeTool fader;
    HumanBehavior * player1Behavior;
    HumanBehavior * player2Behavior;
};

/* Our game definition, this is to facilitate running a game */
class Game {
    public:
        Game(const PlayerType &, const GameType &, const Filesystem::AbsolutePath & systemFile);
        virtual ~Game();

        //! Runs everything
        virtual void run(Searcher & searcher);
        
        /* in run-match.cpp */
        static void runMatch(Mugen::Stage * stage, const std::string & musicOverride = "", RunMatchOptions options = RunMatchOptions());

        /* just start a training match */
        static void startTraining(const std::string & player1, const std::string & player2, const std::string & stage);
        static void startNetworkVersus(const std::string & player1Name, const std::string & player2Name, const std::string & stageName, bool server, int port);
        /* start an arcade match */
        static void startArcade(const std::string & player1, const std::string & player2, const std::string & stage);
        /* start a watch match */
        static void startWatch(const std::string & player1Name, const std::string & player2Name, const std::string & stageName);
        /* start a scripted match */
        static void startScript(const std::string & player1Name, const std::string & player1Script, const std::string & player2Name, const std::string & player2Script, const std::string & stageName);
    private:
        void doArcade(Searcher &);
        void doTraining(Searcher &);
        void doVersus(Searcher &);
        void doNetworkVersus(bool isServer, Searcher &);
        void doWatch(Searcher &);
        void doSurvival(Searcher &);
        void startDemo(Searcher &);

        //! Player type
        PlayerType playerType;
        //! Game type
        GameType gameType;
        //! System file
        Filesystem::AbsolutePath systemFile;
        //! Motif Base Directory
        Filesystem::AbsolutePath motifDirectory;
};

}

#endif

