#ifndef _paintown_a0739024d79cadca8f06a994adc0bdf9_h
#define _paintown_a0739024d79cadca8f06a994adc0bdf9_h

#include <deque>
#include "input/text-input.h"
#include "object/object.h"

class World;

class ChatWidget{
public:
    ChatWidget(World & world, Object::networkid_t id);
    virtual ~ChatWidget();

    virtual void act();
    void receiveMessage(std::string message);

    inline const std::deque<std::string> & getChatMessages() const {
        return chatMessages;
    }

    virtual void drawChat(Bitmap * work, int start);

public:
    void endChatLine();

private:
    enum Inputs{
        Talk,
    };

    World & world;
    std::deque<std::string> chatMessages;
    int removeChatTimer;
    bool enable_chat;
    TextInput chatInput;
    InputMap<Inputs> input;
    Object::networkid_t id;
};

#endif
