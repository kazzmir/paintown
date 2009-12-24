#ifndef _paintown_chat_stuff_h
#define _paintown_chat_stuff_h

enum ChatType{
	CHANGE_NAME = 1,
	ADD_MESSAGE,
	REMOVE_BUDDY,
	ADD_BUDDY,
	START_THE_GAME,
	OK_TO_START,
        /* first message a client sends to the server */
        HELLO,
};

#endif
