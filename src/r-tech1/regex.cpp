#include <string>
#include <map>
#include <string.h>
/* gnu/posix's regex header */
// #include <regex.h>
// #include "trex/trex.h"
#include "libs/pcre/pcre.h"
/* our regex header */
#include "r-tech1/regex.h"

using namespace std;

namespace Util{

static map<string, pcre*> cachedPatterns;

Regex::Regex(const string & data):
data(data){
}
        
const std::string & Regex::get() const {
    return data;
}
    
/* http://www.gnu.org/s/libc/manual/html_node/Regular-Expressions.html */
bool matchRegex(const string & str, const Regex & pattern){
    pcre * regex;
    const char * error;
    int errorOffset;
    int count;
    regex = cachedPatterns[pattern.get()];
    if (regex == NULL){
        regex = pcre_compile(pattern.get().c_str(), 0, &error, &errorOffset, NULL);
        if (regex == NULL){
            return false;
        }
        cachedPatterns[pattern.get()] = regex;
    }

    count = pcre_exec(regex, NULL, str.c_str(), str.size(), 0, 0, NULL, 0);
    // pcre_free(regex);
    return count >= 0;
}
    
string captureRegex(const string & str, const Regex & pattern, int capture){
    pcre * regex;
    const char * error;
    int errorOffset;
    int count;
    const int captureMax = 100;
    int captures[captureMax];
    regex = cachedPatterns[pattern.get()];
    if (regex == NULL){
        regex = pcre_compile(pattern.get().c_str(), 0, &error, &errorOffset, NULL);
        if (regex == NULL){
            return "";
        }
        cachedPatterns[pattern.get()] = regex;
    }

    count = pcre_exec(regex, NULL, str.c_str(), str.size(), 0, 0, captures, captureMax);
    // pcre_free(regex);
    if (count >= 0 && capture < count){
        int start = captures[(capture + 1) * 2];
        int end = captures[(capture + 1) * 2 + 1];
        return str.substr(start, end - start);
    }
    
    return "";
}

} // namespace
