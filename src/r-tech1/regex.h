#ifndef _rtech1_regex_h
#define _rtech1_regex_h

#include <string>

namespace Util{

    /* Use a new type so raw strings are not confused for regexes */
    class Regex{
    public:
        explicit Regex(const std::string & input);

        const std::string & get() const;
    public:
        std::string data;
    };

    bool matchRegex(const std::string & str, const Regex & regex);
    std::string captureRegex(const std::string & str, const Regex & regex, int capture);
}

#endif
