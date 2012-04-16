#ifndef mugen_widgets_h
#define mugen_widgets_h

#include "util.h"
#include "util/bitmap.h"
#include "util/input/text-input.h"

#include <string>

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
        
        virtual inline void setText(const std::string & text){
            this->input.setText(text);
        }
        
        virtual inline void setBodyColor(const Graphics::Color & color){
            this->body = color;
        }
        
        virtual inline void setBorderColor(const Graphics::Color & color){
            this->border = color;
        }
        
        virtual inline void setAlpha(const Graphics::Color & color){
            this->alpha = color;
        }
        
        virtual inline void setWidth(int width){
            this->width = width;
        }
        
    protected:
        Graphics::Color body;
        Graphics::Color border;
        int alpha;
        int width;
        TextInput input;
    };
}
}
#endif