#ifndef _mugen_config_h
#define _mugen_config_h

#include <string>


namespace Mugen{

/*! Configuration for mugen includes the Options, Motif and Rules */
class Data{
    public:

        static void initialize();

        static std::string getMugenDirectory();

        static std::string getMugenDataDirectory();

        static std::string getMugenCharDirectory();
        
        static std::string getMugenFontDirectory();

        static std::string getMugenStageDirectory();
        
        static void setMotif(const std::string & motif);

        static const std::string & getMotif();

        static void setDifficulty(int difficulty);

        static int getDifficulty();

        static void setLife(int life);

        static int getLife();

        static void setTime(int time);
        
        static int getTime();

        static void setSpeed(int speed);
        
        static int getSpeed();

        static void setTeam1vs2Life(int life);

        static int getTeam1vs2Life();

        static void setTeamLoseOnKO(bool lose);

        static bool getTeamLoseOnKO();

        static const std::string & getGameType();

        static double getDefaultAttackLifeToPowerMultiplier();

        static double getDefaultGetHitLifeToPowerMultiplier();

        static double getSuperTargetDefenceMultiplier();

        static void setGameSpeed(int speed);
        
        static int getGameSpeed();

        static bool getDrawShadows();

        /* *TODO add in remaining getters */ 

    private:
        Data(const std::string & configFile);
        ~Data();

        //! Current Set Motif
        std::string motif;
        
        //! Current set Difficulty (default 4)
        int difficulty;
        //! Default life in percentage, usefull for handicaps (default is 100%)
        int life;
        //! Default Stage Time, set to -1 to disable time (default 99)
        int time;
        //! Speed (modifier? Speed is either positive faster or negative slower)
        int speed;
        //! Team Game: 1 player against Team advantage represented in percentage (default 150%)
        int team1vs2Life;
        //! Team Game: if player is KOed AI keeps fighting otherwise team loses (default is lose)
        bool teamLoseOnKO;
        //! Default Game Type this is VS all the time since it's the only option supported
        std::string gameType;
        /*!
         * ;This is the amount of power the attacker gets when an attack successfully
         * ;hits the opponent. It's a multiplier of the damage done. For example,
         * ;for a value of 3, a hit that does 10 damage will give 30 power. (defaults to .7) 
         */
        double defaultAttackLifeToPowerMultiplier;

        /*
         * ;This is like the above, but it's for the person getting hit.
         * ;These two multipliers can be overridden in the Hitdef controller in the
         * ;CNS by using the "getpower" and "givepower" options. (defaults to .6)
         */
        double defaultGetHitLifeToPowerMultiplier;

        /*
         * ;This controls how much damage a super does when you combo into it. 
         * ;It's actually a multiplier for the defensive power of the opponent. 
         * ;A large number means the opponent takes less damage. Leave it at 
         * ;1 if you want supers to do the normal amount of damage when comboed into. 
         * ;Note 1: this increase in defence stays effective until the opponent gets up from the ground. 
         * ;Note 2: the program knows you've done a super when the "superpause" 
         * ;        controller is executed. That's the instance when this change 
         * ;        becomes effective. (defaults to 1.5)
         */
         double superTargetDefenceMultiplier;
         
         /*;Set the game speed here. The default is 60 frames per second. The
          * ;larger the number, the faster it goes. Don't use a value less than 10.*/
         int gameSpeed;
         
         /*;Set to 1 to draw shadows (default). Set to 0 if you have a slow
          * ;machine, and want to improve speed by not drawing shadows.*/
         bool drawShadows;
    
         /*;Number of simultaneous afterimage effects allowed.
         ;Set to a lower number to save memory (minimum 1) (default 8).*/
         int afterImageMax;
    
         /*;Maximum number of layered sprites that can be drawn.
         ;Set to a lower number to save memory (minimum 32) (default 128).*/
         int layeredSpriteMax;
    
         /*;Maximum number of explods allowed in total. Note that hitsparks
         ;also count as explods.
         ;Set to a lower number to save memory (minimum 8) (default 64).*/
         int explodMax;
    
         /*;Maximum number of system explods allowed.
         ;Set to a lower number to save memory (minimum 8) (default 48).*/
         int sysExplodMax;
    
         /*;Maximum number of helpers allowed in total.
         ;Set to a lower number to save memory (minimum 4, maximum 56) (default 32).*/
         int helperMax;
    
         /*;Maximum number of projectiles allowed per player.
         ;Set to a lower number to save memory (minimum 5) (default 50).*/
         int playerProjectileMax;
    
         /*;This is 1 the first time you run MUGEN.*/
         bool firstRun;
};

}

#endif




