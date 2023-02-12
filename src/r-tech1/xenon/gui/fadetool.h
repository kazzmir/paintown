#ifndef gui_fadetool_h
#define gui_fadetool_h

#include "r-tech1/graphics/bitmap.h"

/*! Fade utility */

namespace Graphics{
class Bitmap;
}
class Token;

namespace Gui{

class FadeTool{
    public:
        FadeTool();
        virtual ~FadeTool();

        enum State{
            FadeIn,
            FadeOut,
            NoFade,
            EndFade,
        };
        
        //! Read info from Token
        void parseDefaults(const Token *);
        
        // Fade state
        void setState(const State &);
        
        // update
        void act();
        
        // Fade to whatever state it is at
        virtual void draw(const Graphics::Bitmap &);
        
        inline const State & getState() const { 
            return currentState; 
        }

        inline void setFadeInTime(const int time){ 
            fadeInTime = time; 
        }

        inline void setFadeOutTime(const int time){ 
            fadeOutTime = time; 
        }

        inline int getFadeInTime() const { 
            return fadeInTime; 
        }

        inline int getFadeOutTime() const { 
            return fadeOutTime; 
        }

        inline void setFadeInColor(Graphics::Color c){ 
            fadeInColor = c; 
        }

        inline Graphics::Color getFadeInColor() const { 
            return fadeInColor; 
        }

        inline void setFadeOutColor(Graphics::Color c ){ 
            fadeOutColor = c; 
        }

        inline Graphics::Color getFadeOutColor() const { 
            return fadeOutColor; 
        }

    protected:
        
        //! Read fade from Token
        void readFade(const Token *, const State &);

        State currentState;
        State lastState;
        int fadeTime;
        int fadeInTime;
        int fadeOutTime;
        Graphics::Color fadeInColor;
        Graphics::Color fadeOutColor;
};
}
#endif
