#include "util/init.h"
#include "util/loading.h"
#include "util/parameter.h"
#include "util/graphics/bitmap.h"
#include "util/font.h"
#include "util/input/input-manager.h"
#include "util/message-queue.h"
#include <stdlib.h>
#include "util/debug.h"

static void test(){
    class TestLoader: public Loader::LoadingContext {
    public:
        TestLoader():
        LoadingContext(){
        }

        virtual ~TestLoader(){
        }

        virtual void load(){
            for (int i = 0; i < 10; i++){
                MessageQueue::info("test");
                Global::debug(0) << i << std::endl;
                Util::rest(1000);
            }
        }
    };

    Loader::Info info("testing testing testing", Filesystem::AbsolutePath("data/menu/paintown.png"));
    TestLoader loader;
    Loader::loadScreen(loader, info, Loader::Default);
}

int main(int argc, char ** argv){
    Global::InitConditions conditions;
    Global::init(conditions);
    Global::setDebug(0);

    InputManager input;
    Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, Graphics::getScreenBuffer());
    Util::Parameter<Util::ReferenceCount<Graphics::ShaderManager> > defaultShaderManager(Graphics::shaderManager, Util::ReferenceCount<Graphics::ShaderManager>(new Graphics::ShaderManager()));
    Util::Parameter<Util::ReferenceCount<Path::RelativePath> > defaultFont(Font::defaultFont, Util::ReferenceCount<Path::RelativePath>(new Path::RelativePath("fonts/arial.ttf")));

    try{
        test();
    } catch (const Exception::Base & fail){
        Global::debug(0) << fail.getTrace() << std::endl;
        return 1;
    }

    return 0;
}
