#include "util/bitmap.h"
#include "network.h"
#include "chat_client.h"
#include "chat.h"
#include "util/bitmap.h"
#include "init.h"
#include "globals.h"
#include "util/keyboard.h"
#include "util/font.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "gui/lineedit.h"
#include "gui/keyinput_manager.h"
#include "gui/keys.h"

using namespace std;

static std::ostream & debug( int level ){
	Global::debug( level ) << "[chat-client] ";
	return Global::debug( level );
}
static void * serverInput( void * client_ ){
	ChatClient * client = (ChatClient *) client_;
	bool done = false;
	while ( ! done ){
		try{
			Network::Message message( client->getSocket() );
			int x;
			ChatType kind;
			message >> x;
			kind = (ChatType) x;
			debug( 1 ) << "Received message type " << kind << endl;
			switch ( kind ){
				case ADD_MESSAGE : {
					client->addMessage( message.path, 0 );
					break;
				}
				case CHANGE_NAME : {
					int id;
					string name;
					message >> id;
					name = message.path;
					client->changeName( id, name );
					break;
				}
				case REMOVE_BUDDY : {
					int id;
					message >> id;
					client->removeBuddy( id );
					break;
				}
				case OK_TO_START : {
					break;
				}
				case START_THE_GAME : {
					/* shut down threads and prepare to play */
					done = true;
					client->finish();
					break;
				}
				case ADD_BUDDY : {
					int id;
					string name;
					message >> id;
					name = message.path;
					client->addBuddy( id, name );
					break;
				}
			}
		} catch ( const Network::NetworkException & e ){
			debug( 0 ) << "Input thread died" << endl;
			done = true;
		}
	}

	return NULL;
}

ChatClient::ChatClient( Network::Socket socket, const string & name ):
need_update( true ),
messages( 400, 300 ),
socket( socket ),
focus( INPUT_BOX ),
finished( false ),
enterPressed( false ){
	background = new Bitmap( Global::titleScreen() );
	pthread_mutex_init( &lock, NULL );

	pthread_create( &inputThread, NULL, serverInput, this );

	try{
		Network::Message nameMessage;
		nameMessage.path = name;
		nameMessage << CHANGE_NAME;
		nameMessage.send( socket );
	} catch ( const Network::NetworkException & n ){
		debug( 0 ) << "Could not send username: " << n.getMessage() << endl;
	}

	lineEdit = new LineEdit();
	lineEdit->position.x = 20;
	lineEdit->position.y = 20 + messages.getHeight() + 5;
	lineEdit->position.width = 400;
	lineEdit->position.height = 30;
	lineEdit->position.radius = 5;
	
	lineEdit->position.body = Bitmap::makeColor( 0, 0, 0 );
	lineEdit->position.bodyAlpha = 128;
	lineEdit->position.border = Bitmap::makeColor( 255, 255, 0 );
	lineEdit->setHorizontalAlign(LineEdit::T_Left);
	lineEdit->setTextColor( Bitmap::makeColor( 255, 255, 255 ) );
	
	lineEdit->setText("Hi!");
	// lineEdit->setFont(Menu::getFont());
	lineEdit->setFont(& Font::getFont(Filesystem::find(Global::DEFAULT_FONT), 20, 20));
	keyInputManager::pressed.connect(lineEdit,&LineEdit::keyPress);
	keyInputManager::pressed.connect(this,&ChatClient::keyPress);
	keyInputManager::released.connect(this,&ChatClient::keyRelease);
	lineEdit->setFocused(true);

	editCounter = 0;
}

sigslot::slot ChatClient::keyPress(const keys &k){
	switch ( k.getValue() ){
		case keys::ENTER : {
			enterPressed = true;
			break;
		}
	}
}

sigslot::slot ChatClient::keyRelease(const keys &k){
	switch ( k.getValue() ){
		case keys::ENTER : {
			enterPressed = false;
			break;
		}
	}
}

bool ChatClient::needToDraw(){
	return need_update;
}

Focus ChatClient::nextFocus( Focus f ){
	switch ( f ){
		case INPUT_BOX : return QUIT;
		case QUIT : return INPUT_BOX;
		default : return INPUT_BOX;
	}
}
	
void ChatClient::addBuddy( int id, const std::string & s ){
	Buddy b;
	b.id = id;
	b.name = s;
	pthread_mutex_lock( &lock );
	buddies.push_back( b );
	needUpdate();
	pthread_mutex_unlock( &lock );
}
	
void ChatClient::changeName( int id, const std::string & s ){
	pthread_mutex_lock( &lock );
	for ( vector< Buddy >::iterator it = buddies.begin(); it != buddies.end(); it++ ){
		Buddy & b = *it;
		if ( b.id == id ){
			b.name = s;
		}
	}
	needUpdate();
	pthread_mutex_unlock( &lock );
}

void ChatClient::removeBuddy( int id ){
	pthread_mutex_lock( &lock );
	for ( vector< Buddy >::iterator it = buddies.begin(); it != buddies.end(); ){
		const Buddy & b = *it;
		if ( b.id == id ){
			it = buddies.erase( it );
		} else {
			it ++;
		}
	}
	needUpdate();
	pthread_mutex_unlock( &lock );
}

void ChatClient::addMessage( const string & s, unsigned int id ){
	pthread_mutex_lock( &lock );
	messages.addMessage( s );
	needUpdate();
	pthread_mutex_unlock( &lock );
}

/*
static char lowerCase( const char * x ){
	if ( x[0] >= 'A' && x[0] <= 'Z' ){
		return x[0] - 'A' + 'a';
	}
	return x[0];
}
*/

bool ChatClient::sendMessage( const string & message ){
	try{
		Network::Message net;
		net << ADD_MESSAGE;
		net.path = message;
		net.send( socket );
		return true;
	} catch ( const Network::NetworkException & e ){
		debug( 0 ) << "Client could not send message" << endl;
	}
	return false;
}
	
void ChatClient::popup( Bitmap & work, Keyboard & key, const std::string & str ){
	const Font & font = Font::getFont(Filesystem::find(Global::DEFAULT_FONT), 20, 20 );
	int length = font.textLength( str.c_str() ) + 20;
	int height = font.getHeight() * 2;
	Bitmap area( work, work.getWidth() / 2 - length / 2, work.getHeight() / 2, length, height );
	area.drawingMode( Bitmap::MODE_TRANS );
	area.rectangleFill( 0, 0, area.getWidth(), area.getHeight(), Bitmap::makeColor( 64, 0, 0 ) );
	area.drawingMode( Bitmap::MODE_SOLID );
	int color = Bitmap::makeColor( 255, 255, 255 );
	area.rectangle( 0, 0, area.getWidth() - 1, area.getHeight() - 1, color );
	font.printf( 10, area.getHeight() / 3, Bitmap::makeColor( 255, 255, 255 ), area, str, 0 );
        work.BlitToScreen();

	key.wait();
	key.readKey();
}

void ChatClient::handleInput( Keyboard & keyboard ){
	/*
	vector< int > keys;
	keyboard.readKeys( keys );

	for ( vector< int >::iterator it = keys.begin(); it != keys.end(); it++ ){
		int key = *it;
		if ( key == Keyboard::Key_BACKSPACE ){
			if ( input != "" ){
				input = input.substr( 0, input.length() - 1 );
				needUpdate();
			}
		} else if ( Keyboard::isAlpha( key ) ){
			input += lowerCase( Keyboard::keyToName( key ) );
			needUpdate();
		} else if ( key == Keyboard::Key_SPACE ){
			input += " ";
			needUpdate();
		} else if ( key == Keyboard::Key_ENTER ){
			addMessage( "You: " + input, 0 );
			if ( ! sendMessage( input ) ){
				popup( keyboard, "Could not send message" );
			}
			input = "";
			needUpdate();
		}
	}
	*/

	lineEdit->logic();

	if ( lineEdit->didChanged( editCounter ) ){
		needUpdate();
	}

	if ( enterPressed && lineEdit->getText().length() > 0 ){
		// enterPressed = false;
		addMessage( "You: " + lineEdit->getText(), 0 );
                toSend.push(lineEdit->getText());
		lineEdit->clearText();
		needUpdate();
	}
}

void ChatClient::needUpdate(){
	need_update = true;
}
	
bool ChatClient::logic( Keyboard & keyboard ){
	if ( keyboard[ Keyboard::Key_TAB ] ){
		focus = nextFocus( focus );
		needUpdate();
	}
	
	keyInputManager::update();

	lineEdit->setFocused(false);
	switch ( focus ){
		case INPUT_BOX : {
			lineEdit->setFocused(true);
			lineEdit->position.border = Bitmap::makeColor(255,255,0);
			handleInput( keyboard );
			return false;
			break;
		}
		case QUIT : {
			lineEdit->position.border = Bitmap::makeColor(255,255,255);
			if ( keyboard[ Keyboard::Key_ENTER ] ){
				return true;
			}
			break;
		}
	}
	debug( 1 ) << "Focus is " << lineEdit->isFocused() << endl;
	return false;
}

void ChatClient::drawInputBox( int x, int y, const Bitmap & work ){
	const Font & font = Font::getFont(Filesystem::find(Global::DEFAULT_FONT), 20, 20 );

	work.drawingMode( Bitmap::MODE_TRANS );
	Bitmap::transBlender( 0, 0, 0, 128 );
	work.rectangleFill( x, y, x + messages.getWidth(), y + font.getHeight() + 1, Bitmap::makeColor( 0, 0, 0 ) );
	work.drawingMode( Bitmap::MODE_SOLID );
	int color = Bitmap::makeColor( 255, 255, 255 );
	if ( focus == INPUT_BOX ){
		color = Bitmap::makeColor( 255, 255, 0 );
	}
	work.rectangle( x, y, x + messages.getWidth(), y + font.getHeight(), color );
	Bitmap input_box( work, x + 1, y, messages.getWidth(), font.getHeight() );
	// font.printf( x + 1, y, Bitmap::makeColor( 255, 255, 255 ), work, input, 0 );
	font.printf( 0, 0, Bitmap::makeColor( 255, 255, 255 ), input_box, input, 0 );

}

void ChatClient::drawBuddies( const Bitmap & area, int x, int y, const Font & font ){
	Bitmap buddyList( area, x, y, GFX_X - x - 5, 200 );
	buddyList.drawingMode( Bitmap::MODE_TRANS );
	Bitmap::transBlender( 0, 0, 0, 128 );
	buddyList.rectangleFill( 0, 0, buddyList.getWidth(), buddyList.getHeight(), Bitmap::makeColor( 0, 0, 0 ) );
	buddyList.drawingMode( Bitmap::MODE_SOLID );
	buddyList.rectangle( 0, 0, buddyList.getWidth() -1, buddyList.getHeight() - 1, Bitmap::makeColor( 255, 255, 255 ) );
	int fy = 1;
	for ( vector< Buddy >::iterator it = buddies.begin(); it != buddies.end(); it++ ){
		Buddy & buddy = *it;
		const string & name = buddy.name;
		font.printf( 1, fy, Bitmap::makeColor( 255, 255, 255 ), buddyList, name, 0 );
		fy += font.getHeight();
	}
}
	
void ChatClient::draw( const Bitmap & work ){
	int start_x = 20;
	int start_y = 20;
	const Font & font = Font::getFont(Filesystem::find(Global::DEFAULT_FONT), 20, 20 );
	background->Blit( work );
	messages.draw( start_x, start_y, work, font );
	// drawInputBox( start_x, start_y + messages.getHeight() + 5, work );
	lineEdit->render(work);
	drawBuddies( work, start_x + messages.getWidth() + 10, start_y, font );

	int color = Bitmap::makeColor( 255, 255, 255 );
	if ( focus == QUIT ){
		color = Bitmap::makeColor( 255, 255, 0 );
	}
	font.printf( start_x, start_y + messages.getHeight() + 5 + font.getHeight() + 20, color, work, "Back", 0 );

	need_update = false;
}
			
bool ChatClient::isFinished(){
	bool b;
	pthread_mutex_lock( &lock );
	b = finished;
	pthread_mutex_unlock( &lock );
	return b;
}

void ChatClient::finish(){
	pthread_mutex_lock( &lock );
	finished = true;
	pthread_mutex_unlock( &lock );
}
	
void ChatClient::killInputThread(){
	debug( 0 ) << "Killing input socket" << endl;
	Network::close( getSocket() );
	debug( 0 ) << "Waiting for input thread to die" << endl;
	pthread_join( inputThread, NULL );
	debug( 0 ) << "Input thread killed" << endl;
}

void ChatClient::run(){
	Global::speed_counter = 0;
	Bitmap work( GFX_X, GFX_Y );
	Keyboard keyboard;

	keyboard.setAllDelay( 200 );
	keyboard.setDelay( Keyboard::Key_TAB, 200 );
	keyboard.setDelay( Keyboard::Key_ESC, 0 );

	bool done = false;
	bool kill = false;
	while ( ! done ){
		int think = Global::speed_counter;
		while ( think > 0 ){
			keyboard.poll();
			if ( logic( keyboard ) ){
				kill = true;
				done = true;
				break;
			}
		
                        while ( ! toSend.empty() ){
                            if ( ! sendMessage( toSend.front() ) ){
                                popup( work, keyboard, "Could not send message" );
                            }
                            toSend.pop();
                        }

			think -= 1;
			Global::speed_counter = 0;
			done = isFinished();
			if ( keyboard[ Keyboard::Key_ESC ] ){
				kill = true;
				done = true;
				break;
			}
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

	if ( kill ){
		killInputThread();
	} else {
		Network::Message message;
		message << OK_TO_START;
		message.send( getSocket() );
	}
}

ChatClient::~ChatClient(){
	delete background;
	delete lineEdit;
}

