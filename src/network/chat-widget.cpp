#include "util/bitmap.h"
#include <deque>
#include "input/text-input.h"
#include "chat-widget.h"
#include "world.h"
#include "input/input-manager.h"
#include "util/font.h"
#include "factory/font_render.h"
#include "util/file-system.h"
#include <string>

using namespace std;

static void do_finish_chat_input(void * arg){
    ChatWidget * widget = (ChatWidget *) arg;
    widget->endChatLine();
}

ChatWidget::ChatWidget(World & world, Object::networkid_t id):
world(world),
removeChatTimer(0),
enable_chat(false),
id(id){
    input.set(Keyboard::Key_T, 0, false, Talk);
    chatInput.addHandle(Keyboard::Key_ENTER, do_finish_chat_input, this);
}

ChatWidget::~ChatWidget(){
    chatInput.disable();
}

void ChatWidget::act(){
    if (removeChatTimer > 0){
        removeChatTimer -= 1;
        if (removeChatTimer == 0 && chatMessages.size() > 0){
            chatMessages.pop_front();
        }
    } else if (chatMessages.size() > 0){
        removeChatTimer = 175;
    }
    
    InputMap<Inputs>::Output inputState = InputManager::getMap(input);
    if (inputState[Talk]){
        enable_chat = true;
    } else {
        if (enable_chat){
            chatInput.enable();
            enable_chat = false;
        }
    }

    chatInput.doInput();
}

void ChatWidget::endChatLine(){
    string message = chatInput.getText();
    chatInput.disable();
    chatInput.clearInput();

    if (message != ""){
        Network::Message chat;
        /* server's id is always 0 */
        chat.id = 0;
        chat << World::CHAT;
        chat << id;
        chat << message;
        world.addMessage(chat);
        chatMessages.push_back(string("You: ") + message);
        while (chatMessages.size() > 10){
            chatMessages.pop_front();
        }
    }
}

void ChatWidget::receiveMessage(string message){
    chatMessages.push_back(message);
    while (chatMessages.size() > 10){
        chatMessages.pop_front();
    }
}

void ChatWidget::draw(Bitmap * work){
    const Font & font = Font::getFont(Filesystem::find(Global::DEFAULT_FONT), 18, 18);
    FontRender * render = FontRender::getInstance();

    int y = work->getHeight() * 2 - 1 - font.getHeight() * 2 - 1;
    const deque<string> messages = getChatMessages();
    for (deque<string>::const_reverse_iterator it = messages.rbegin(); it != messages.rend(); it++){
        string message = *it;
        render->addMessage(font, 1, y, Bitmap::makeColor(255, 255, 255), -1, message);
        y -= font.getHeight() + 1;
    }

    if (chatInput.isEnabled()){
        const int green = Bitmap::makeColor(0, 255, 0);
        render->addMessage(font, 1, work->getHeight() * 2 - font.getHeight() - 1, green, -1, string("Say: ") + chatInput.getText());
    }
}
