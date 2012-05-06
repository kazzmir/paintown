#ifndef mugen_widgets_h
#define mugen_widgets_h

#include "util.h"
#include "util/bitmap.h"
#include "util/input/text-input.h"

#include <string>
#include <deque>

namespace Mugen{
namespace Widgets{
    //! Draw a box with a border
    void drawBox(int radius, int x, int y, int width, int height, const Graphics::Color &, const Graphics::Color &, int alpha, const Graphics::Bitmap &);
    
    //! Input Box
    class InputBox{
    public:
        InputBox();
        virtual ~InputBox();
        
        virtual void act();
        virtual void draw(int x, int y, const FontSystem::Font &, const Graphics::Bitmap &);
        
        virtual void toggleEnabled();
        
        virtual void addHook(int key, void (*callback)(void *), void * arg);
        
        virtual inline const std::string getText() const {
            return this->input.getText();
        }
        
        virtual inline void setText(const std::string & text){
            this->input.setText(text);
        }
        
        virtual inline void clear(){
            this->input.clearInput();
        }
        
        virtual inline void setBodyColor(const Graphics::Color & color){
            this->body = color;
        }
        
        virtual inline void setBorderColor(const Graphics::Color & color){
            this->border = color;
        }
        
        virtual inline void setAlpha(int color){
            this->alpha = color;
        }
        
        virtual inline void setWidth(int width){
            this->width = width;
        }
        
    protected:
        void renderCursor(int x, int y, const FontSystem::Font &, const Graphics::Bitmap &);
        Graphics::Color body;
        Graphics::Color border;
        int alpha;
        int width;
        TextInput input;
        int cursorTime;
    };
    
    typedef void *(*EventCallback)(const std::string &);
    
    class ChatPanel{
    public:
        ChatPanel(int x, int y, int width, int height);
        virtual ~ChatPanel();
        
        virtual void act();
        virtual void draw(const Graphics::Bitmap &);
        
        virtual void addMessage(const std::string &);
        virtual void addMessage(const std::string &, const std::string &);
        
        virtual void notify(const std::string &);
        
        virtual void handleCommand(const std::vector<std::string> &);
        
        virtual inline void subscribe(EventCallback subscriber){
            this->callbacks.push_back(subscriber);
        }
        
        class Event{
        public:
            Event();
            virtual ~Event();
            virtual void sendMessage(const std::string &) = 0;
        };
        virtual inline void subscribe(Event * subscriber){
            this->subscribers.push_back(subscriber);
        }
        
        virtual inline InputBox & getInput(){
            return this->input;
        }
        
        virtual inline void setFont(const FontSystem::Font & font){
            this->font = font;
        }
        
        virtual inline void setClient(const std::string & name){
            this->client = name;
        }
        
        virtual inline const std::string & getClient() const {
            return this->client;
        }
        
    protected:
        FontSystem::Font font;
        InputBox input;
        std::deque<std::string> buffer;
        int x;
        int y;
        int width;
        int height;
        std::string client;
        
        // Message poller
        std::vector<EventCallback> callbacks;
        std::vector<Event *> subscribers;
    };
}
}
#endif
