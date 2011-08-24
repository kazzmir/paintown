#include "character.h"

#include "util/timedifference.h"

#include <stdio.h>
#include <stdlib.h>

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
module(NULL),
dict(NULL),
charClass(NULL),
character(NULL){
    PyObject * name; 
    name = PyString_FromString(str);
    TimeDifference time;
    time.startTime();
    module = PyImport_Import(name);
    time.endTime();
    std::cout << time.printTime("Loading Character Module took") << std::endl;
    if (module == NULL){
        throw PyException("Couldn't load module: " + std::string(str));
    }
    
    dict = PyModule_GetDict(module);
        
    charClass = PyDict_GetItemString(dict, str);
    
    if (PyCallable_Check(charClass)){
        character = PyObject_CallObject(charClass, NULL);
    } else {
        throw PyException("Couldn't verify class.");
    }
    
    std::cout << "Successfull loaded module: " << str << ".py" << std::endl;
    
    // Set initial state
    PyObject * check = PyObject_CallMethod(character, "changeState", "(is)", 5900, "None");
    if (check == NULL){
        throw PyException("Couldn't load initialize state 5900");
    }
    
    Py_DECREF(check);
    Py_DECREF(name);
}

Character::~Character(){
    Py_DECREF(character);
    Py_DECREF(module);
}

void Character::act(){
    // State -3
    
    // State -2 
    
    // State -1
    
    // Current State
    PyObject * check = PyObject_CallMethod(character, "act", "(s)", "world");
    if (check != NULL){
    } else if (check == NULL){
        throw PyException("Calling current state failed.");
    }
    Py_DECREF(check);
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