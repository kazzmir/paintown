#include "controllers.h"

#include "ast/all.h"

using namespace Mugen;

Content getController(const std::string & type){
    if (type == "AfterImage"){
    } else if (type == "AfterImageTime"){
    } else if (type == "AllPalFX"){
    } else if (type == "AngleAdd"){
    } else if (type == "AngleDraw"){
    } else if (type == "AngleMul"){
    } else if (type == "AngleSet"){
    } else if (type == "AppendToClipboard"){
    } else if (type == "AssertSpecial"){
    } else if (type == "AttackDist"){
    } else if (type == "AttackMulSet"){
    } else if (type == "BGPalFX"){
    } else if (type == "BindToParent"){
    } else if (type == "BindToRoot"){
    } else if (type == "BindToTarget"){
    } else if (type == "ChangeAnim"){
    } else if (type == "ChangeAnim2"){
    } else if (type == "ChangeState"){
    } else if (type == "ClearClipboard"){
    } else if (type == "CtrlSet"){
    } else if (type == "DefenceMulSet"){
    } else if (type == "DestroySelf"){
    } else if (type == "DisplayToClipboard"){
    } else if (type == "EnvColor"){
    } else if (type == "EnvShake"){
    } else if (type == "Explod"){
    } else if (type == "ExplodBindTime"){
    } else if (type == "FallEnvShake"){
    } else if (type == "ForceFeedback"){
    } else if (type == "GameMakeAnim"){
    } else if (type == "Gravity"){
    } else if (type == "Helper"){
    } else if (type == "HitAdd"){
    } else if (type == "HitBy"){
    } else if (type == "HitDef"){
    } else if (type == "HitFallDamage"){
    } else if (type == "HitFallSet"){
    } else if (type == "HitFallVel"){
    } else if (type == "HitOverride"){
    } else if (type == "HitVelSet"){
    } else if (type == "LifeAdd"){
    } else if (type == "LifeSet"){
    } else if (type == "MakeDust"){
    } else if (type == "ModifyExplod"){
    } else if (type == "MoveHitReset"){
    } else if (type == "NotHitBy"){
    } else if (type == "Null"){
    } else if (type == "Offset"){
    } else if (type == "PalFX"){
    } else if (type == "ParentVarAdd"){
    } else if (type == "ParentVarSet"){
    } else if (type == "Pause"){
    } else if (type == "PlaySnd"){
    } else if (type == "PlayerPush"){
    } else if (type == "PosAdd"){
    } else if (type == "PosFreeze"){
    } else if (type == "PosSet"){
    } else if (type == "PowerAdd"){
    } else if (type == "PowerSet"){
    } else if (type == "Projectile"){
    } else if (type == "RemapPal"){
    } else if (type == "RemoveExplod"){
    } else if (type == "ReversalDef"){
    } else if (type == "ScreenBound"){
    } else if (type == "SelfState"){
    } else if (type == "SndPan"){
    } else if (type == "SprPriority"){
    } else if (type == "StateTypeSet"){
    } else if (type == "StopSnd"){
    } else if (type == "SuperPause"){
    } else if (type == "TargetBind"){
    } else if (type == "TargetDrop"){
    } else if (type == "TargetFacing"){
    } else if (type == "TargetLifeAdd"){
    } else if (type == "TargetPowerAdd"){
    } else if (type == "TargetState"){
    } else if (type == "TargetVelAdd"){
    } else if (type == "TargetVelSet"){
    } else if (type == "Trans"){
    } else if (type == "Turn"){
    } else if (type == "VarAdd"){
    } else if (type == "VarRandom"){
    } else if (type == "VarRangeSet"){
    } else if (type == "VarSet"){
    } else if (type == "VelAdd"){
    } else if (type == "VelMul"){
    } else if (type == "VelSet"){
    } else if (type == "VictoryQuote"){
    } else if (type == "Width"){
    } else {
        std::cout << "Unhandled Controller Type: " << type << std::endl;
    }
    
    // Set to pass if state controller can't be determined
    return Content(1, "pass");
}

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
            const std::string & ctrl = type->back()->valueAsString();
            line << "# Found controller type '" << ctrl << "' on line " << type->back()->getLine();
            definition.addContent(Content(1, line.str()));
            definition.addContent(getController(ctrl));
        }
    }
}
    
StateController::StateController(const StateParameterMap & parameters):
parameters(parameters){
}

StateController::~StateController(){
}
