#include "util/graphics/bitmap.h"
#include <deque>
#include "util/input/text-input.h"
#include "chat-widget.h"
#include "paintown-engine/game/world.h"
#include "util/input/input-manager.h"
#include "util/font.h"
#include "factory/font_render.h"
#include "util/resource.h"
#include "util/file-system.h"
#include <string>

using namespace std;

static void do_finish_chat_input(void * arg){
    ChatWidget * widget = (ChatWidget *) arg;
    widget->endChatLine();
}

ChatWidget::ChatWidget(World & world, Paintown::Object::networkid_t id):
world(world),
removeChatTimer(0),
enable_chat(false),
id(id){
    input.set(Keyboard::Key_T, 0, false, Talk);
    chatInput.setBlockingKeys();
    chatInput.addBlockingHandle(Keyboard::Key_ENTER, do_finish_chat_input, this);
}

ChatWidget::~ChatWidget(){
}

void ChatWidget::act(){
    vector<InputMap<Inputs>::InputEvent> events = InputManager::getEvents(input, InputSource());

    for (vector<InputMap<Inputs>::InputEvent>::iterator it = events.begin(); it != events.end(); it++){
        const InputMap<Inputs>::InputEvent & event = *it;
        if (!event.enabled){
            continue;
        }

        /* This prevents 't' from being pressed in the chat thing immediately */
        if (event[Talk]){
            enable_chat = true;
        } else {
            if (enable_chat){
                chatInput.enable();
                enable_chat = false;
            }
        }
    }
    
    for (deque<Message>::iterator it = chatMessages.begin(); it != chatMessages.end();){
        Message & message = *it;
        message.life -= 1;
        if (message.life <= 0){
            it = chatMessages.erase(it);
        } else {
            it++;
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
        receiveMessage(string("You: ") + message);
    }
}

void ChatWidget::receiveMessage(string message){
    chatMessages.push_back(Message("", message, 500));
    while (chatMessages.size() > 20){
        chatMessages.pop_front();
    }
    Resource::getSound(Filesystem::RelativePath("menu/sounds/talk.wav"))->play();
}

void ChatWidget::drawChat(Graphics::Bitmap * work, int start){
    const Font & font = Font::getFont(Global::DEFAULT_FONT, 18, 18);
    FontRender * render = FontRender::getInstance();

    // int y = work->getHeight() * 2 - 1 - font.getHeight() * 2 - 1;
    int y = start - font.getHeight();

    if (chatInput.isEnabled()){
        const Graphics::Color green = Graphics::makeColor(0, 255, 0);
        // render->addMessage(font, 1, y, green, -1, string("Say: ") + chatInput.getText());
        render->addMessage(font, 1, y, Graphics::makeColor(255, 255, 0), Graphics::MaskColor(), string("Say: "));
        render->addMessage(font, 1 + font.textLength("Say: "), y, green, Graphics::MaskColor(), chatInput.getText());
        y -= font.getHeight() + 1;
    }

    for (deque<Message>::reverse_iterator it = chatMessages.rbegin(); it < chatMessages.rend(); it++){
        const Message & message = *it;
        int trans = message.life;
        if (trans < 255){
            render->addMessage(font, 1, y, Graphics::makeColor(255, 255, 255), Graphics::MaskColor(), trans, message.text);
        } else {
            render->addMessage(font, 1, y, Graphics::makeColor(255, 255, 255), Graphics::MaskColor(), message.text);
        }
        y -= font.getHeight() + 1;
    }
}
