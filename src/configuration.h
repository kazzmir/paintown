#ifndef _paintown_configuration_h
#define _paintown_configuration_h

namespace Configuration{

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

	int getRight();
	int getLeft();
	int getUp();
	int getDown();
	int getAttack1();
	int getAttack2();
	int getAttack3();
	int getJump();

	}

#endif
