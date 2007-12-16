#include "network.h"
#include "chat_server.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include "util/keyboard.h"
#include "init.h"

ChatServer::ChatServer( Network::Socket socket ):
need_update( true ),
socket( socket ),
messages( 300, 300 ){
	background = new Bitmap( Util::getDataPath() + "/paintown-title.png" );
}

void ChatServer::logic(){
}

bool ChatServer::needToDraw(){
	return need_update;
}

void ChatServer::draw( const Bitmap & work ){
	background->Blit( work );
	messages.draw( 20, 20, work );
	need_update = false;
}
	
void ChatServer::run(){
	Global::speed_counter = 0;
	Bitmap work( GFX_X, GFX_Y );
	Keyboard keyboard;

	bool done = false;
	while ( ! done ){
		int think = Global::speed_counter;
		while ( think > 0 ){
			keyboard.poll();
			logic();
			think -= 1;
			Global::speed_counter = 0;
			done = keyboard[ Keyboard::Key_ESC ];
		}

		if ( needToDraw() ){
			draw( work );
			work.BlitToScreen();
			work.clear();
		}

		while ( Global::speed_counter == 0 ){
			Util::rest( 1 );
			keyboard.poll();
		}
	}
}
	
ChatServer::~ChatServer(){
	delete background;
}
