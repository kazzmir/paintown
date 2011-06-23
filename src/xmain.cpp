#include "main.h"

#ifdef USE_ALLEGRO
#include <allegro.h>

int main( int argc, char ** argv ){
    return paintown_main(argc, argv);
}
END_OF_MAIN()
#endif

#ifdef USE_ALLEGRO5
int main( int argc, char ** argv ){
    return paintown_main(argc, argv);
}
#endif

#ifdef MINPSPW
#include <pspkernel.h>
// Define only on windows because main is redefined as SDL_main (from SDL_psp_main.c comments)
#ifdef INCLUDE_SCE_MODULE_INFO
PSP_MODULE_INFO("Paintown", 0, 1, 1);
#endif 
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
PSP_HEAP_SIZE_MAX();
#endif

#ifdef NACL
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
namespace pp{
    class PaintownInstance : public pp::Instance {
    public:
        explicit PaintownInstance(PP_Instance instance) : pp::Instance(instance) {}
        virtual ~PaintownInstance() {}

        virtual void HandleMessage(const pp::Var& var_message){
            /* do something here */
        }
    };

    class PaintownModule : public pp::Module {
    public:
        PaintownModule() : pp::Module() {}
        virtual ~PaintownModule() {}

        virtual pp::Instance* CreateInstance(PP_Instance instance) {
            return new PaintownInstance(instance);
        }
    };

    Module * CreateModule(){
        return new PaintownModule();
    }
}
#endif

#ifdef USE_SDL
#ifdef USE_SDL_MAIN
extern "C" int SDL_main(int argc, char ** argv){
    return paintown_main(argc, argv);
}
#else
int main( int argc, char ** argv ){
    return paintown_main(argc, argv);
}
#endif
#endif
