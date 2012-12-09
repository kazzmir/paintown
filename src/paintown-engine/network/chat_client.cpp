#ifdef HAVE_NETWORKING

#include "util/bitmap.h"
#include "util/stretch-bitmap.h"
#include "../network/network.h"
#include "util/trans-bitmap.h"
#include "util/events.h"
#include "chat_client.h"
#include "chat.h"
#include "util/bitmap.h"
#include "util/init.h"
#include "globals.h"
#include "util/input/keyboard.h"
#include "util/input/input-source.h"
#include "util/font.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/gui/lineedit.h"
#include "util/resource.h"
#include "util/input/input-manager.h"
#include "util/input/input-map.h"
#include "util/sound/sound.h"

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
            debug(1) << "Waiting to receive a message" << endl;
            Network::Message message(client->getSocket());
            int x;
            ChatType kind;
            message >> x;
            kind = (ChatType) x;
            debug( 1 ) << "Received message type " << kind << endl;
            switch ( kind ){
                case HELLO : {
                    debug(0) << "Client should not see a HELLO message!" << endl;
                    break;
                }
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

void ChatClient::enter_pressed(void * self){
    ChatClient * chat = (ChatClient*) self;
    if (chat->lineEdit->getText().size() > 0){
        chat->addMessage("You: " + chat->lineEdit->getText(), 0);
        chat->toSend.push(chat->lineEdit->getText());
        chat->lineEdit->clearText();
        chat->needUpdate();
    }
}

ChatClient::ChatClient( Network::Socket socket, const string & name ):
need_update(true),
messages( 400, 300 ),
socket(socket),
focus(INPUT_BOX),
finished(false),
enterPressed( false ){
    background = new Graphics::Bitmap(Global::titleScreen().path());
    Util::Thread::initializeLock(&lock);

    Util::Thread::createThread(&inputThread, NULL, (Util::Thread::ThreadFunction) serverInput, this);

    try{
        Network::Message nameMessage;
        nameMessage.path = name;
        nameMessage << HELLO;
        nameMessage << Global::MagicId;
        nameMessage << Global::getVersion();
        nameMessage.send(socket);
    } catch (const Network::NetworkException & n){
        debug( 0 ) << "Could not send username: " << n.getMessage() << endl;
    }

    lineEdit = new Gui::LineEdit();
    lineEdit->location.setPosition(Gui::AbsolutePoint(20, 20 + messages.getHeight() + 5));
    lineEdit->location.setDimensions(400, 30);
    lineEdit->transforms.setRadius(5);

    lineEdit->colors.body = Graphics::makeColor(0, 0, 0);
    lineEdit->colors.bodyAlpha = 128;
    lineEdit->colors.border = Graphics::makeColor(255, 255, 0);
    lineEdit->setHorizontalAlign(Gui::LineEdit::T_Left);
    lineEdit->setTextColor(Graphics::makeColor(255, 255, 255));

    lineEdit->setText("Hi!");
    // lineEdit->setFont(Menu::getFont());
    lineEdit->setFont(& Font::getFont(Global::DEFAULT_FONT, 20, 20));
    lineEdit->hookKey(Keyboard::Key_ENTER, enter_pressed, this);
    lineEdit->hookKey(Keyboard::Key_TAB, next_focus, this);
    lineEdit->setFocused(true);

    editCounter = 0;
}

bool ChatClient::needToDraw(){
    return need_update;
}

Focus ChatClient::nextFocus(Focus f){
    switch (f){
        case INPUT_BOX: return QUIT;
        case QUIT: return INPUT_BOX;
        default: return INPUT_BOX;
    }
}
	
void ChatClient::addBuddy( int id, const std::string & s ){
    Buddy b;
    b.id = id;
    b.name = s;
    Util::Thread::acquireLock( &lock );
    buddies.push_back(b);
    needUpdate();
    Util::Thread::releaseLock( &lock );
    Resource::getSound(Filesystem::RelativePath("menu/sounds/chip-in.wav"))->play();
}
	
void ChatClient::changeName( int id, const std::string & s ){
    Util::Thread::acquireLock( &lock );
    for ( vector< Buddy >::iterator it = buddies.begin(); it != buddies.end(); it++ ){
        Buddy & buddy = *it;
        if (buddy.id == id){
            buddy.name = s;
        }
    }
    needUpdate();
    Util::Thread::releaseLock( &lock );
}

void ChatClient::removeBuddy( int id ){
    Util::Thread::acquireLock( &lock );
    for ( vector< Buddy >::iterator it = buddies.begin(); it != buddies.end(); ){
        const Buddy & b = *it;
        if ( b.id == id ){
            it = buddies.erase( it );
        } else {
            it ++;
        }
    }
    needUpdate();
    Util::Thread::releaseLock( &lock );
    Resource::getSound(Filesystem::RelativePath("menu/sounds/chip-out.wav"))->play();
}

void ChatClient::addMessage( const string & s, unsigned int id ){
    Util::Thread::acquireLock( &lock );
    messages.addMessage( s );
    needUpdate();
    Util::Thread::releaseLock( &lock );
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

void ChatClient::popup(const Graphics::Bitmap & work, const std::string & str ){
    const Font & font = Font::getFont(Global::DEFAULT_FONT, 20, 20 );
    int length = font.textLength( str.c_str() ) + 20;
    int height = font.getHeight() * 2;
    Graphics::Bitmap area( work, work.getWidth() / 2 - length / 2, work.getHeight() / 2, length, height );
    area.translucent().rectangleFill( 0, 0, area.getWidth(), area.getHeight(), Graphics::makeColor( 64, 0, 0 ) );
    Graphics::Color color = Graphics::makeColor( 255, 255, 255 );
    area.rectangle( 0, 0, area.getWidth() - 1, area.getHeight() - 1, color );
    font.printf( 10, area.getHeight() / 3, Graphics::makeColor( 255, 255, 255 ), area, str, 0 );
    work.BlitToScreen();

    /*
    key.wait();
    key.readKey();
    */
}

#if 0
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

    const Font & font = Font::getFont(Global::DEFAULT_FONT, 20, 20 );
    lineEdit->act(font);

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
#endif

void ChatClient::needUpdate(){
	need_update = true;
}

void ChatClient::next_focus(void * self){
    ChatClient * chat = (ChatClient*) self;
    chat->focus = chat->nextFocus(chat->focus);
    chat->lineEdit->setFocused(chat->focus == INPUT_BOX);
    if (chat->focus == INPUT_BOX){
        chat->lineEdit->colors.border = Graphics::makeColor(255,255,0);
    } else {
        chat->lineEdit->colors.border = Graphics::makeColor(255,255,255);
    }
    chat->needUpdate();
}
	
bool ChatClient::logic(){
    const Font & font = Font::getFont(Global::DEFAULT_FONT, 20, 20);
    lineEdit->act(font);
    if (lineEdit->didChanged(editCounter)){
        needUpdate();
    }

    return false;

    /*
	if ( keyboard[ Keyboard::Key_TAB ] ){
		focus = nextFocus( focus );
                debug(1) << "Focus is " << focus << endl;
		needUpdate();
	}

	lineEdit->setFocused(false);
	switch ( focus ){
		case INPUT_BOX : {
			lineEdit->setFocused(true);
			lineEdit->colors.border = Bitmap::makeColor(255,255,0);
			handleInput( keyboard );
			return false;
		}
		case QUIT : {
			lineEdit->colors.border = Bitmap::makeColor(255,255,255);
			if ( keyboard[ Keyboard::Key_ENTER ] ){
				return true;
			}
			break;
		}
	}
	return false;
        */
}

void ChatClient::drawInputBox( int x, int y, const Graphics::Bitmap & work ){
    const Font & font = Font::getFont(Global::DEFAULT_FONT, 20, 20 );

    Graphics::Bitmap::transBlender( 0, 0, 0, 128 );
    work.translucent().rectangleFill( x, y, x + messages.getWidth(), y + font.getHeight() + 1, Graphics::makeColor( 0, 0, 0 ) );
    Graphics::Color color = Graphics::makeColor( 255, 255, 255 );
    if ( focus == INPUT_BOX ){
        color = Graphics::makeColor( 255, 255, 0 );
    }
    work.rectangle( x, y, x + messages.getWidth(), y + font.getHeight(), color );
    Graphics::Bitmap input_box( work, x + 1, y, messages.getWidth(), font.getHeight() );
    // font.printf( x + 1, y, Bitmap::makeColor( 255, 255, 255 ), work, input, 0 );
    font.printf( 0, 0, Graphics::makeColor( 255, 255, 255 ), input_box, input, 0 );

}

void ChatClient::drawBuddies( const Graphics::Bitmap & area, int x, int y, const Font & font ){
    Graphics::Bitmap buddyList( area, x, y, area.getWidth() - x - 5, 200 );
    // buddyList.drawingMode( Graphics::Bitmap::MODE_TRANS );
    Graphics::Bitmap::transBlender( 0, 0, 0, 128 );
    buddyList.translucent().rectangleFill( 0, 0, buddyList.getWidth(), buddyList.getHeight(), Graphics::makeColor( 0, 0, 0 ) );
    // buddyList.drawingMode( Bitmap::MODE_SOLID );
    buddyList.rectangle( 0, 0, buddyList.getWidth() -1, buddyList.getHeight() - 1, Graphics::makeColor( 255, 255, 255 ) );
    int fy = 1;
    for ( vector< Buddy >::iterator it = buddies.begin(); it != buddies.end(); it++ ){
        Buddy & buddy = *it;
        const string & name = buddy.name;
        font.printf( 1, fy, Graphics::makeColor( 255, 255, 255 ), buddyList, name, 0 );
        fy += font.getHeight();
    }
}
	
void ChatClient::draw( const Graphics::Bitmap & work ){
    int start_x = 20;
    int start_y = 20;
    const Font & font = Font::getFont(Global::DEFAULT_FONT, 20, 20 );
    background->Blit( work );
    messages.draw( start_x, start_y, work, font );
    // drawInputBox( start_x, start_y + messages.getHeight() + 5, work );
    lineEdit->render(work);
    drawBuddies( work, start_x + messages.getWidth() + 10, start_y, font );

    Graphics::Color color = Graphics::makeColor( 255, 255, 255 );
    if (focus == QUIT){
        color = Graphics::makeColor( 255, 255, 0 );
    }
    font.printf( start_x, start_y + messages.getHeight() + 5 + font.getHeight() + 20, color, work, "Back", 0 );

    need_update = false;
}
			
bool ChatClient::isFinished(){
    bool b;
    Util::Thread::acquireLock( &lock );
    b = finished;
    Util::Thread::releaseLock( &lock );
    return b;
}

void ChatClient::finish(){
    Util::Thread::acquireLock( &lock );
    finished = true;
    Util::Thread::releaseLock( &lock );
}

void ChatClient::killInputThread(){
    debug(0) << "Killing input socket" << endl;
    Network::close(getSocket());
    debug(0) << "Waiting for input thread to die" << endl;
    Util::Thread::joinThread(inputThread);
    debug(0) << "Input thread killed" << endl;
}

static void set_to_true(void * b){
    bool * what = (bool*) b;
    *what = true;
}

void ChatClient::run(){
    // Global::speed_counter2 = 0;

    class Logic: public Util::Logic {
    public:
        Logic(bool & kill, ChatClient & client):
        is_done(false),
        kill(kill),
        client(client),
        forceQuit(false){
            input.set(Keyboard::Key_TAB, 0, true, 0);
            input.set(Keyboard::Key_ENTER, 0, true, 1);
            input.set(Keyboard::Key_ESC, 0, true, 2);
            client.lineEdit->hookKey(Keyboard::Key_ESC, set_to_true, &forceQuit);
        }

        InputMap<int> input;
        bool is_done;
        bool & kill;
        ChatClient & client;
        bool forceQuit;

        double ticks(double system){
            return system;
        }

        bool done(){
            return is_done;
        }

        void run(){
            vector<InputMap<int>::InputEvent> events = InputManager::getEvents(input, InputSource());

            bool hasInputFocus = client.focus == INPUT_BOX;
            bool next = false;
            bool select = false;
            bool quit = false;
            for (vector<InputMap<int>::InputEvent>::iterator it = events.begin(); it != events.end(); it++){
                const InputMap<int>::InputEvent & event = *it;
                if (!event.enabled){
                    continue;
                }
                next = next || event[0];
                select = select || event[1];
                quit = quit || event[2];
            }

            if (client.logic() || forceQuit || quit){
                kill = true;
                is_done = true;
                return;
            }

            if (next && !hasInputFocus){
                next_focus(&client);
            }

            if (quit || (select && client.focus == QUIT)){
                Global::debug(0) << "Quit!" << endl;
                kill = true;
                is_done = true;
            }

            while (! client.toSend.empty()){
                if (! client.sendMessage(client.toSend.front())){
                    popup(*Graphics::screenParameter.current(), "Could not send message" );
                }
                client.toSend.pop();
            }

            is_done = is_done || client.isFinished();
        }
    };

    class Draw: public Util::Draw {
    public:
        Draw(ChatClient & client):
        client(client){
        }

        ChatClient & client;

        void draw(const Graphics::Bitmap & buffer){
            if (client.needToDraw()){
                Graphics::StretchedBitmap work(640, 480, buffer);
                work.start();
                work.clear();
                client.draw(work);
                work.finish();
                buffer.BlitToScreen();
            }
        }
    };

    bool kill = false;
    Logic logic(kill, *this);
    Draw draw(*this);

    Util::standardLoop(logic, draw);

#if 0
    InputMap<int> input;
    input.set(Keyboard::Key_TAB, 0, true, 0);
    input.set(Keyboard::Key_ENTER, 0, true, 1);
    input.set(Keyboard::Key_ESC, 0, true, 2);

    bool forceQuit = false;
    bool done = false;
    lineEdit->hookKey(Keyboard::Key_ESC, set_to_true, &forceQuit);
    while (! done){
        int think = Global::speed_counter2;
        Global::speed_counter2 = 0;
        while (think > 0){
            InputManager::poll();
            vector<InputMap<int>::InputEvent> events = InputManager::getEvents(input);

            bool hasInputFocus = focus == INPUT_BOX;
            bool next = false;
            bool select = false;
            bool quit = false;
            for (vector<InputMap<int>::InputEvent>::iterator it = events.begin(); it != events.end(); it++){
                const InputMap<int>::InputEvent & event = *it;
                if (!event.enabled){
                    continue;
                }
                next = next || event[0];
                select = select || event[1];
                quit = quit || event[2];
            }

            if (logic() || forceQuit || quit){
                kill = true;
                done = true;
                break;
            }

            if (next && !hasInputFocus){
                next_focus(this);
            }

            if (quit || (select && focus == QUIT)){
                Global::debug(0) << "Quit!" << endl;
                kill = true;
                done = true;
            }

            while (! toSend.empty()){
                if (! sendMessage( toSend.front() )){
                    popup(work, "Could not send message" );
                }
                toSend.pop();
            }

            think -= 1;
            done = done || isFinished();

            /*
            if ( keyboard[ Keyboard::Key_ESC ] ){
                kill = true;
                done = true;
                break;
            }
            */
        }

        if (needToDraw()){
            draw(work);
            work.BlitToScreen();
            work.clear();
        }

        while (Global::speed_counter2 == 0){
            Util::rest(1);
        }
    }
#endif

    lineEdit->setFocused(false);

    if (kill){
        killInputThread();
    } else {
        /* when OK_TO_START is sent there are guaranteed to be no other
         * packets in front of it to confuse the server
         */
        Network::Message message;
        message << OK_TO_START;
        message.send(getSocket());
    }
}

ChatClient::~ChatClient(){
    delete background;
    delete lineEdit;
}

#endif
