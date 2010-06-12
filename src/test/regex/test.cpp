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
    if (!Util::matchRegex("a", "a")){
        throw Failure(1);
    }
    if (!Util::matchRegex("aaaa", "a*")){
        throw Failure(1);
    }
    if (!Util::matchRegex("2", "[0-9]")){
        throw Failure(1);
    }
    if (!Util::matchRegex("asdofijwioefrjoaisfjioasdf", ".*")){
        throw Failure(1);
    }
    if (!Util::matchRegex("tuna", "^tuna$")){
        throw Failure(1);
    }
    if (!Util::matchRegex("football", "f(oot)ball")){
        throw Failure(1);
    }
    if (!Util::matchRegex("az", "(1|2|3|a)(f|v|z)")){
        throw Failure(1);
    }
    if (Util::matchRegex("nogga", "lopster")){
        throw Failure(1);
    }
}

int main(){
    try{
        test1();
        cout << "All(1) tests passed!" << endl;
        return 0;
    } catch (const Failure & f){
        cout << "Test case " << f.num << " failed" << endl;
        return 1;
    }
}
