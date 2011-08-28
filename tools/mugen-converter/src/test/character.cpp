#include "character.h"

#include "util/timedifference.h"

#include <stdio.h>
#include <stdlib.h>
#include <sstream>

PyException::PyException(const std::string & what):
reason(what){
}
PyException::~PyException() throw(){
}

const std::string & PyException::getReason() const throw(){
    if (PyErr_Occurred() != NULL){
        PyErr_Print();
    }
    return reason;
}


Character::Character(const char * str):
moduleName(str),
charModule(NULL),
charDict(NULL),
charClass(NULL),
character(NULL){
    PyObject * name = PyString_FromString(str);
    TimeDifference time;
    time.startTime();
    charModule = PyImport_Import(name);
    time.endTime();
    std::cout << time.printTime("Loading Character Module took") << std::endl;
    if (charModule == NULL){
        throw PyException("Couldn't load module: " + std::string(str));
    }
    
    charDict = PyModule_GetDict(charModule);
        
    charClass = PyDict_GetItemString(charDict, str);
    
    if (PyCallable_Check(charClass)){
        character = PyObject_CallObject(charClass, NULL);
    } else {
        throw PyException("Couldn't verify " + std::string(str) + " class.");
    }
    
    std::cout << "Successfully loaded module: " << str << ".py" << std::endl;
    
    addAttribute("name", Character::String);
    characterName = getStringValue("name");
    
    std::cout << "Character Name: " << characterName << std::endl;
    
    Py_DECREF(name);
    
    // Load mugen.World
    name = PyString_FromString("mugen");
    time.startTime();
    worldModule = PyImport_Import(name);
    time.endTime();
    std::cout << time.printTime("Loading mugen.World took") << std::endl;
    if (worldModule == NULL){
        throw PyException("Couldn't load mugen.World.");
    }
    worldDict = PyModule_GetDict(worldModule);
    worldClass = PyDict_GetItemString(worldDict, "World");
    
    if (PyCallable_Check(worldClass)){
        world = PyObject_CallObject(worldClass, NULL);
    } else {
        throw PyException("Couldn't verify mugen.World class.");
    }
    
    std::cout << "Successfully loaded mugen.World" << std::endl;
    
    // Set initial state
    std::cout << "Setting initial state 5900" << std::endl;
    changeState(5900);
    
    Py_DECREF(name);
}

Character::~Character(){
    Py_DECREF(character);
    Py_DECREF(charModule);
    Py_DECREF(world);
    Py_DECREF(worldModule);
}

void Character::act(){
    // State -3
    
    // State -2 
    
    // State -1
    
    // Current State
    //PyObject * check = PyObject_CallMethod(character, "act", "(o)", world);
    PyObject * act = PyString_FromString("act");
    PyObject * check = PyObject_CallMethodObjArgs(character, act, world, NULL);
    if (check != NULL){
    } else if (check == NULL){
        throw PyException("Calling current state failed.");
    }
    Py_DECREF(check);
    Py_DECREF(act);
}
        
void Character::addAttribute(const std::string & attributeName, const AttributeType & type){
    switch (type){
        case String:{
            if (PyObject_HasAttrString(character, attributeName.c_str())){
                PyObject * tmp = PyObject_GetAttrString(character, attributeName.c_str());
                stringValues[attributeName] = PyString_AsString(tmp);
                Py_DECREF(tmp);
            }
            break;
        }
        case Numerical:{
            if (PyObject_HasAttrString(character, attributeName.c_str())){
                PyObject * tmp = PyObject_GetAttrString(character, attributeName.c_str());
                numericValues[attributeName] = atof(PyString_AsString(tmp));
                Py_DECREF(tmp);
            }
            break;
        }
        default:
            break;
    }
}

void Character::callMethod(const std::string & method){
    //PyObject_CallMethod(character, "someFunc", NULL);
}


const std::string & Character::getStringValue(const std::string & attribute){
    std::map<std::string, std::string>::iterator found = stringValues.find(attribute);
    if (found != stringValues.end()){
        return found->second;
    } else {
        throw PyException("Attribute '" + attribute + "' Not Found");
    }
}

const double Character::getNumericValue(const std::string & attribute){
    std::map<std::string, double>::iterator found = numericValues.find(attribute);
    if (found != numericValues.end()){
        return found->second;
    } else {
        throw PyException("Attribute '" + attribute + "' Not Found");
    }
}

int Character::getCurrentStateNumber(){
    PyObject * check = PyObject_CallMethod(character, "getStateNo", NULL);
    if (check == NULL){
         throw PyException("Couldn't get current state number.");
    }
    int number = PyLong_AsLong(check);
    Py_DECREF(check);
    return number;
}

void Character::changeState(int number){
    PyObject * check = PyObject_CallMethod(character, "changeState", "(is)", number, world);
    if (check == NULL){
        std::ostringstream num;
        num << number;
        throw PyException("Couldn't change to state " + num.str());
    }
    Py_DECREF(check);
}
