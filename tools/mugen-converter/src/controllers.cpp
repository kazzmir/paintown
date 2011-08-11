#include "controllers.h"

#include "ast/all.h"

using namespace Mugen;

StateParameterMap::StateParameterMap(){
}

StateParameterMap::StateParameterMap(const StateParameterMap & copy){
    this->parameters = copy.parameters;
}

StateParameterMap::~StateParameterMap(){
}

const StateParameterMap & StateParameterMap::operator=(const StateParameterMap & copy){
    this->parameters = copy.parameters;
}

void StateParameterMap::add(const std::string & param, Ast::AttributeSimple * simple){
    parameters[param].push_back(simple);
}

std::vector<Ast::AttributeSimple *> * StateParameterMap::find(const std::string & param){
    std::map<std::string, std::vector<Ast::AttributeSimple *> >::iterator found = parameters.find(param);
    if (found != parameters.end()){
        return &(*found).second;
    }
    return NULL;
}
    
StateControllerStore::StateControllerStore(){
}
StateControllerStore::~StateControllerStore(){
}

void StateControllerStore::newController(){
    controllerParameters.push_back(StateParameterMap());
}

StateParameterMap & StateControllerStore::getCurrentController(){
    return controllerParameters.back();
}

void StateControllerStore::addToDefinition(PythonDefinition & definition){
    
}
    
StateController::StateController(const StateParameterMap & parameters):
parameters(parameters){
}

StateController::~StateController(){
}
