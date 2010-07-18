#include "util.h"

using namespace std;

namespace Bor{

extern const void * parse(const char * in, int length, bool stats = false);

string doParse(const char * input, int length){
    string * output = (string *) parse(input, length);
    string final = *output;
    delete output;
    return final;
}

}
