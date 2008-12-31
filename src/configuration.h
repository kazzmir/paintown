#ifndef _paintown_configuration_h
#define _paintown_configuration_h

class Token;

class Configuration{
public:

	static Configuration & config( int x );
	static void loadConfigurations();
	static void saveConfiguration();

        enum PlayMode{
            Cooperative = 1,
            FreeForAll = 2,
        };

	/* return a Keyboard::Key_X based on some PAIN_KEY_X */
	int getKey( int which, int facing );
	
	void setRight( int i );
	void setLeft( int i );
	void setUp( int i );
	void setDown( int i );
	void setAttack1( int i );
	void setAttack2( int i );
	void setAttack3( int i );
	void setJump( int i );

	int getRight() const;
	int getLeft() const;
	int getUp() const;
	int getDown() const;
	int getAttack1() const;
	int getAttack2() const;
	int getAttack3() const;
	int getJump() const;

public:
	static double getGameSpeed();
	static void setGameSpeed(double s);
	static bool getInvincible();
	static void setInvincible(bool i);
	static bool getFullscreen();
	static void setFullscreen(bool f);
	static int getLives();
	static void setLives(int l);
	static int getNpcBuddies();
	static void setNpcBuddies( int i );
        static PlayMode getPlayMode();
        static void setPlayMode(PlayMode mode);

protected:
	Configuration();
	Configuration( const Configuration & config );
	Configuration( const int right, const int left, const int up, const int down, const int attack1, const int attack2, const int attack3, const int jump );

	~Configuration();

	Configuration & operator=( const Configuration & config );

	static Configuration defaultPlayer1Keys();
	static Configuration defaultPlayer2Keys();

	static Token * saveKeyboard( int num, Configuration * configuration );

private:
	int right;
	int left;
	int up;
	int down;
	int attack1;
	int attack2;
	int attack3;
	int jump;

private:
	//! Game speed
	static double gamespeed;
	
	//! Invincibility
	static bool invincible;
	
	//! Fullscreen
	static bool fullscreen;
	
	//! Lives
	static int lives;

	static int npc_buddies;
        static PlayMode play_mode;
};

#endif
