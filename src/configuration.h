#ifndef _paintown_configuration_h
#define _paintown_configuration_h

namespace Configuration{

	/* return a Keyboard::Key_X based on some PAIN_KEY_X */
	int getKey( int which, int facing );

	extern int right;
	extern int left;
	extern int up;
	extern int down;
	extern int attack1;
	extern int attack2;
	extern int attack3;
	extern int jump;
}

#endif
