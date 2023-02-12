#ifndef _rtech1_language_string_h
#define _rtech1_language_string_h

#include <string>
#include <map>

class LanguageString: public std::string {
public:
    LanguageString();
    LanguageString(const char * stuff);
    LanguageString(const std::string & stuff);
    LanguageString(const std::string & stuff, const std::string & language);
    LanguageString(const LanguageString & language);
    LanguageString & operator=(const LanguageString & obj);

    static const std::string defaultLanguage(){
        return "English";
    }

    const std::string currentLanguage() const;

    /* add a new language translation */
    void add(const std::string & stuff, const std::string & language);

    /* adds with the default language */
    void add(const std::string & stuff);

    std::string get() const;

    inline const std::map<std::string, std::string> & getLanguages() const {
        return languages;
    }

protected:
    std::map<std::string, std::string> languages;
};

#endif
