#ifdef NACL

/* talks to chrome via the nacl stuff */

#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/var.h>
// #include <ppapi/cpp/dev/scriptable_object_deprecated.h>
#include <SDL/SDL.h>
#include <SDL/SDL_nacl.h>
#include <string>
#include "../main.h"
#include "network-system.h"
#include "../funcs.h"

namespace nacl{
    /*
    class PaintownScript: public pp::deprecated::ScriptableObject {
    public:
        virtual bool HasMethod(const pp::Var& method, pp::Var* exception);
        virtual pp::Var Call(const pp::Var& method, const std::vector<pp::Var>& args,pp::Var* exception);
    };
    */

    class PaintownInstance : public pp::Instance {
    public:
        explicit PaintownInstance(PP_Instance instance, pp::Core * core):
            pp::Instance(instance),
            core(core){
            the_instance = this;
            RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE);
            RequestFilteringInputEvents(PP_INPUTEVENT_CLASS_KEYBOARD);
        }
        virtual ~PaintownInstance() {}
        static PaintownInstance * the_instance;
        pp::Core * core;

        static int launch(void * arg){
            int argc = 1;
            char * argv[1] = {"paintown"};
            Global::debug(0) << "Run paintown main" << std::endl;
            paintown_main(argc, argv);
            return 0;
        }

        bool HandleInputEvent(const pp::InputEvent & event){
            SDL_NACL_PushEvent(event);
            return true;
        }

        /* set up the viewport and run the game as usual */
        void run(){
            Util::ReferenceCount<Storage::System> system(new Nacl::NetworkSystem(the_instance, core));
            Storage::setInstance(system);
            SDL_NACL_SetInstance(pp_instance(), 640, 480);
            int ok = SDL_Init(SDL_INIT_VIDEO |
                              SDL_INIT_AUDIO |
                              SDL_INIT_TIMER |
                              SDL_INIT_JOYSTICK |
                              SDL_INIT_NOPARACHUTE);
            Global::debug(0) << "SDL Init: " << ok << std::endl;

            Util::Thread::Id thread;
            Util::Thread::createThread(&thread, NULL, (Util::Thread::ThreadFunction) launch, NULL);
            Global::debug(0) << "Running thread " << thread << std::endl;
        }

        /*
        virtual pp::Var GetInstanceObject(){
            PaintownScript * object = new PaintownScript();
            return pp::Var(this, object);
        }
        */

        /* when the browser does paintownModule.postMessage('run') this will
         * get executed.
         */
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

    /*
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
    */

    class PaintownModule : public pp::Module {
    public:
        PaintownModule() : pp::Module() {}
        virtual ~PaintownModule() {}

        virtual pp::Instance* CreateInstance(PP_Instance instance) {
            return new PaintownInstance(instance, core());
        }
    };
}

namespace pp{
    Module * CreateModule(){
        return new nacl::PaintownModule();
    }
}

#endif
