#include <string>
#include "r-tech1/language-string.h"
#include "r-tech1/configuration.h"

using namespace std;
    
LanguageString::LanguageString(){
}

LanguageString::LanguageString(const std::string & stuff){
    add(stuff);
}

LanguageString::LanguageString(const char * stuff){
    add(stuff);
}

LanguageString::LanguageString(const std::string & stuff, const std::string & language){
    add(stuff, language);
}

LanguageString::LanguageString(const LanguageString & language):
std::string(){
    languages = language.languages;
}
    
LanguageString & LanguageString::operator=(const LanguageString & obj){
    this->languages = obj.languages;
    return *this;
}
    
void LanguageString::add(const std::string & stuff, const std::string & language){
    languages[language] = stuff;
}

void LanguageString::add(const std::string & stuff){
    add(stuff, defaultLanguage());
}
    
const string LanguageString::currentLanguage() const {
    return Configuration::getLanguage();
}
    
std::string LanguageString::get() const {
    map<std::string, std::string>::const_iterator find = languages.find(currentLanguage());
    if (find != languages.end()){
        return find->second;
    }
    find = languages.find(defaultLanguage());
    if (find != languages.end()){
        return find->second;
    }
    return "";

    /*
    if (languages[currentLanguage()] != ""){
        return languages[currentLanguage()];
    }
    return languages[defaultLanguage()];
    */
}
