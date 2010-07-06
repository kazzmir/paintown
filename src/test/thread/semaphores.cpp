#include <iostream>
#include <vector>
#include "util/thread.h"

using namespace std;

int bag_of_cheetos;

static void * do_calculationinate(void * arg){
    Util::Thread::Semaphore * semaphore = (Util::Thread::Semaphore*) arg;
    Util::Thread::semaphoreDecrease(semaphore);
    bag_of_cheetos += 1;
    Util::Thread::semaphoreIncrease(semaphore);
    return NULL;
}

int main(){
    /* start with no chips */
    bag_of_cheetos = 0;
    const int max_threads = 5;
    vector<Util::Thread::Id> threads;
    Util::Thread::Semaphore george_washington;
    Util::Thread::initializeSemaphore(&george_washington, 1);
    for (int i = 0; i < max_threads; i++){
        Util::Thread::Id moses;
        Util::Thread::createThread(&moses, NULL, (Util::Thread::ThreadFunction) do_calculationinate, (void*) &george_washington);
        threads.push_back(moses);
    }

    for (vector<Util::Thread::Id>::iterator it = threads.begin(); it != threads.end(); it++){
        Util::Thread::Id penguins = *it;
        Util::Thread::joinThread(penguins);
    }

    if (bag_of_cheetos != max_threads){
        cout << "Expected " << max_threads << " cheetos but only got " << bag_of_cheetos << endl;
    } else {
        cout << "Passed! Expected " << max_threads << " and got " << bag_of_cheetos << endl;
    }
}
