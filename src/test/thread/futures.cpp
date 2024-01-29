#include "r-tech1/thread.h"
#include <cassert>

#ifndef WINDOWS
#include "r-tech1/file-system.h"

/* FIXME: dont put these methods in this test file */
Filesystem::AbsolutePath Filesystem::configFile(){
    std::ostringstream str;
    /* what if HOME isn't set? */
    str << getenv("HOME") << "/.paintownrc";
    return Filesystem::AbsolutePath(str.str());
}

Filesystem::AbsolutePath Filesystem::userDirectory(){
    std::ostringstream str;
    char * home = getenv("HOME");
    if (home == NULL){
        str << "/tmp/paintown";
    } else {
        str << home << "/.paintown/";
    }
    return Filesystem::AbsolutePath(str.str());
}
#endif

class Future: public Util::Future<int> {
public:
    virtual void compute(){
        set(32);
    }
};

void test1(){
    Future f;
    f.start();
    assert(f.get() == 32);
}

void test2(){
    for (int i = 0; i < 100000; i++){
        Future f;
    }
}

int main(){
    test1();
    test2();
}
