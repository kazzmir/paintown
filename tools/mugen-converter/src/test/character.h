#ifndef mugen_converter_test_character
#define mugen_converter_test_character

#include <Python.h>

#include <iostream>
#include <string>
#include <map>
#include <exception>

class PyException : public std::exception{
    public:
        PyException(const std::string &);
        virtual ~PyException() throw();
        
        virtual const std::string & getReason() const throw();
        
    private:
        std::string reason;
    
    
};

class Character{
    public:
        Character(const char *);
        virtual ~Character();
        
        // Simplified for now
        enum AttributeType{
            String,
            Numerical,
        };
        
        virtual void act();
        
        virtual void addAttribute(const std::string &, const AttributeType &);
        
        virtual void callMethod(const std::string &);
        
        virtual const std::string & getStringValue(const std::string & attribute);
        
        virtual const double getNumericValue(const std::string & attribute);
        
        virtual int getCurrentStateNumber();
        
        virtual void changeState(int number);
        
        virtual inline const std::string & getName() const{
            return this->characterName;
        }
        
    protected:
        std::string moduleName;
        std::string characterName;
        PyObject * charModule, * charDict, * charClass, * character;
        PyObject * worldModule, * worldDict, * worldClass, * world;
        
        // Simplified for now, can change later
        std::map<std::string, std::string> stringValues;
        std::map<std::string, double> numericValues;
};

#endif