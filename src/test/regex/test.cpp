#include "../../util/regex.h"
#include <iostream>

using namespace std;

class Failure: public exception{
public:
    Failure(int num):
    num(num){
    }

    int num;
};

void test1(){
    using namespace Util;
    if (!matchRegex("a", "a")){
        throw Failure(1);
    }
    if (!matchRegex("aaaa", "a*")){
        throw Failure(1);
    }
    if (!matchRegex("2", "[0-9]")){
        throw Failure(1);
    }
    if (!matchRegex("asdofijwioefrjoaisfjioasdf", ".*")){
        throw Failure(1);
    }
    if (!matchRegex("tuna", "^tuna$")){
        throw Failure(1);
    }
    if (!matchRegex("football", "f(oot)ball")){
        throw Failure(1);
    }
    if (!matchRegex("az", "(1|2|3|a)(f|v|z)")){
        throw Failure(1);
    }
    if (matchRegex("nogga", "lopster")){
        throw Failure(1);
    }
}

void test2(){
    using namespace Util;
    if (captureRegex("foobar", "f(oo)bar", 0) != "oo"){
        throw Failure(2);
    }
}

int main(){
    try{
        test1();
        test2();
        cout << "All(2) tests passed!" << endl;
        return 0;
    } catch (const Failure & f){
        cout << "Test case " << f.num << " failed" << endl;
        return 1;
    }
}
