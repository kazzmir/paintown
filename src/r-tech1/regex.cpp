#include <string>
#include <map>
#include <string.h>
#include <regex>
/* our regex header */
#include "r-tech1/regex.h"

using namespace std;

namespace Util{

Regex::Regex(const string & data):
data(data){
}
        
const std::string & Regex::get() const {
    return data;
}

/* http://www.gnu.org/s/libc/manual/html_node/Regular-Expressions.html */
bool matchRegex(const string & str, const Regex & pattern){
    std::smatch match;
    if (std::regex_search(str, match, std::regex(pattern.get()))){
        return true;
    }

    return false;
}
    
string captureRegex(const string & str, const Regex & pattern, int capture){
    std::smatch match;
    if (std::regex_match(str, match, std::regex(pattern.get()))){
        if ((int) match.size() > capture+1){
            return match[capture+1];
        }
        return "";
    }

    return "";
}

} // namespace
