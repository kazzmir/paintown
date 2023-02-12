#ifndef util_gui_cutscene_h
#define util_gui_cutscene_h

#include "fadetool.h"
#include "r-tech1/pointer.h"
#include "animation.h"
#include "r-tech1/file-system.h"

#include <string>
#include <vector>

class Token;
namespace Graphics{
class Bitmap;
}

namespace Gui{

/*! Cut scenes or story boards */
class Scene{
public:
    Scene(const Token *);
    virtual ~Scene();

    virtual void forward(int tickCount = 1);
    virtual void reverse(int tickCount = 1);
    virtual void act();
    virtual void render(const Graphics::Bitmap &);

    virtual void addAnimation(const Util::ReferenceCount<Gui::Animation> &);
    
    virtual void reset();
    virtual void setToEnd();
    
    virtual bool done() const;

    virtual inline void setEnd(int ticks){
        this->endTicks = ticks;
    }
    
    virtual inline Gui::AnimationManager & getManager(){
        return this->backgrounds;
    }

protected:
    void parseScene(const Token * token);

protected:
    int ticks;
    int endTicks;

    Gui::AnimationManager backgrounds;

    Gui::FadeTool fader;
};
    
class CutScene{
public:
    CutScene();
    CutScene(const Filesystem::AbsolutePath & path);
    CutScene(const Token *);
    virtual ~CutScene();

    virtual void setName(const std::string &);
    virtual inline const std::string & getName() const {
        return this->name;
    }
    virtual void setResolution(int w, int h);
    virtual void setScene(unsigned int scene);
    virtual inline int getScene() const{
        return this->current;
    }
    
    /*! Return current scene */
    virtual Util::ReferenceCount<Scene> getCurrent();

    /* play all scenes in order */
    virtual void playAll();

    /* plays the current scene which may consists of several animations */
    virtual void playScene();

    /* play a specific scene */
    virtual void playScene(unsigned scene);

    /* advance to the next scene */
    virtual void next();
    virtual bool hasMore();

protected:
    virtual void load(const Token * token);

protected:
    std::string name;
    int width;
    int height;
    std::vector< Util::ReferenceCount<Scene> > scenes;
    unsigned int current;
};
}
#endif
