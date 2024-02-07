#include "r-tech1/argument.h"

#include "r-tech1/exceptions/shutdown_exception.h"
#include "r-tech1/exceptions/exception.h"
#include "r-tech1/exceptions/load_exception.h"
#include "r-tech1/ftalleg.h"
#include "r-tech1/menu/menu-exception.h"
#include "r-tech1/token_exception.h"
#include "r-tech1/system.h"

#include "r-tech1/file-system.h"

#include <string.h>

using std::vector;
using std::string;

Argument::Action::~Action(){
}

bool Argument::Parameter::isArg(const string & what) const {
    for (string check: keywords()){
        if (strcasecmp(check.c_str(), what.c_str()) == 0){
            return true;
        }
    }
    return false;
}

Argument::Parameter::~Parameter(){
}

Argument::Handler::Handler(){
}

Argument::Handler::~Handler(){
}

void Argument::Handler::add(Util::ReferenceCount<Parameter> parameter){
    parameters.push_back(parameter);
}
    
void Argument::Handler::add(const std::vector<Util::ReferenceCount<Parameter> > & parameters){
    for (ParameterRefs::const_iterator i = parameters.begin(); i != parameters.end(); ++i){
        add(*i);
    }
}

void Argument::Handler::setDefaultAction(Util::ReferenceCount<Action> action){
    defaultAction = action;
}

void Argument::Handler::runActions(int argc, char ** argv){
    vector<string> stringArgs;
    for (int q = 1; q < argc; q++){
        stringArgs.push_back(argv[q]);
    }
    
    std::vector<Util::ReferenceCount<Action> > actions;
    
    /* Sort of a hack but if we are already at the end of the argument list (because some
     * argument already reached the end) then we don't increase the argument iterator
     */
    for (vector<string>::iterator it = stringArgs.begin(); it != stringArgs.end(); (it != stringArgs.end()) ? it++ : it){
        for (ParameterRefs::iterator arg = parameters.begin(); arg != parameters.end(); arg++){
            Util::ReferenceCount<Parameter> parameter = *arg;
            if (parameter->isArg(*it)){
                it = parameter->parse(it, stringArgs.end(), actions);

                /* Only parse one argument */
                break;
            }
        }
    }
    
    /* If there are no actions then add default */
    if (actions.size() == 0){
        actions.push_back(defaultAction);
    }
    
    bool allow_quit = true;
    /* Run em */
    while (true){
        bool normal_quit = false;
        try{
            for (ActionRefs::const_iterator it = actions.begin(); it != actions.end(); it++){
                Util::ReferenceCount<Action> action = *it;
                action->act();
            }
            normal_quit = true;
        } catch (const Filesystem::Exception & ex){
            Global::debug(0) << "There was a problem loading an argument. Error was:\n  " << ex.getTrace() << std::endl;
        } catch (const TokenException & ex){
            Global::debug(0) << "There was a problem with the token. Error was:\n  " << ex.getTrace() << std::endl;
            return;
        } catch (const LoadException & ex){
            Global::debug(0) << "There was a problem loading an argument. Error was:\n  " << ex.getTrace() << std::endl;
            return;
        } catch (const Exception::Return & ex){
        } catch (const ShutdownException & shutdown){
            Global::debug(1) << "Forced a shutdown. Cya!" << std::endl;
        } catch (const ReloadMenuException & ex){
            Global::debug(1) << "Menu Reload Requested. Restarting...." << std::endl;
            continue;
        } catch (const ftalleg::Exception & ex){
            Global::debug(0) << "Freetype exception caught. Error was:\n" << ex.getReason() << std::endl;
        } catch (const Exception::Base & base){
            Global::debug(0) << "Base exception: " << base.getTrace() << std::endl;
        } catch (const std::bad_alloc & fail){
            Global::debug(0) << "Failed to allocate memory. Usage is " << System::memoryUsage() << std::endl;
        } 
        // Do not catch all, end user should
        /*catch (...){
            Global::debug(0) << "Uncaught exception!" << std::endl;
        }*/

        if (allow_quit && normal_quit){
            break;
        } else if (normal_quit && !allow_quit){
        } else if (!normal_quit){
            break;
        }
    }
}
