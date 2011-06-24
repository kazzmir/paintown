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
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/var.h>
#include <ppapi/cpp/dev/scriptable_object_deprecated.h>
#include <SDL/SDL_nacl.h>
#include <string>

namespace nacl{
    class PaintownScript: public pp::deprecated::ScriptableObject {
    public:
        virtual bool HasMethod(const pp::Var& method, pp::Var* exception);
        virtual pp::Var Call(const pp::Var& method, const std::vector<pp::Var>& args,pp::Var* exception);
    };

    class PaintownInstance : public pp::Instance {
    public:
        explicit PaintownInstance(PP_Instance instance) : pp::Instance(instance) {
            the_instance = this;
        }
        virtual ~PaintownInstance() {}
        static PaintownInstance * the_instance;

        void run(){
            SDL_NACL_SetInstance(pp_instance(), 640, 480);
            int argc = 1;
            char * argv[1] = {"paintown"};
            paintown_main(argc, argv);
        }

        virtual pp::Var GetInstanceObject(){
            PaintownScript * object = new PaintownScript();
            return pp::Var(this, object);
        }

        virtual void HandleMessage(const pp::Var& var_message){
            if (!var_message.is_string()){
                return;
            }

            std::string message = var_message.AsString();
            if (message == "run"){
                run();
            }
        }
    };
    PaintownInstance * PaintownInstance::the_instance;

    bool PaintownScript::HasMethod(const pp::Var& method, pp::Var* exception){
        if (!method.is_string()){
            return false;
        }

        std::string name = method.AsString();
        if (name == "run"){
            return true;
        }
        return false;
    }

    pp::Var PaintownScript::Call(const pp::Var& method, const std::vector<pp::Var>& args,pp::Var* exception){
        if (!method.is_string()){
            return false;
        }

        std::string name = method.AsString();
        if (name == "run"){
            PaintownInstance::the_instance->run();
        }

        return pp::Var();
    }

    class PaintownModule : public pp::Module {
    public:
        PaintownModule() : pp::Module() {}
        virtual ~PaintownModule() {}

        virtual pp::Instance* CreateInstance(PP_Instance instance) {
            return new PaintownInstance(instance);
        }
    };
}

namespace pp{
    Module * CreateModule(){
        return new nacl::PaintownModule();
    }
}
#endif

#ifndef NACL
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
#endif
