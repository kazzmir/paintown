#ifndef _paintown_configuration_h
#define _paintown_configuration_h

class Configuration{
public:

	static Configuration & config( int x );

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

protected:
	Configuration();
	Configuration( const Configuration & config );

	Configuration & operator=( const Configuration & config );

	static Configuration defaultPlayer1Keys();
	static Configuration defaultPlayer2Keys();

private:
	int right;
	int left;
	int up;
	int down;
	int attack1;
	int attack2;
	int attack3;
	int jump;
};

#endif
