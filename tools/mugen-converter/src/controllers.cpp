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
    
    // TODO Go through current parameters and create new classes based on 'type' and add to definition
    for (std::vector<StateParameterMap>::iterator i = controllerParameters.begin(); i != controllerParameters.end(); ++i){
        std::vector<Ast::AttributeSimple *> * type = (*i).find("type");
        if (type != NULL){
            std::stringstream line;
            line << "# Found controller type '" << type->back()->valueAsString() << "' on line " << type->back()->getLine();
            definition.addContent(Content(1, line.str()));
        }
    }
    // FIXME Remove this line when the state function has been populated
    definition.addContent(Content(1, "pass"));
    
}
    
StateController::StateController(const StateParameterMap & parameters):
parameters(parameters){
}

StateController::~StateController(){
}
