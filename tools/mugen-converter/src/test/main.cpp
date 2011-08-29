#include <iostream>
#include <pthread.h>

#include <Python.h>

#include "character.h"

using namespace std;

static int error(const std::string & message){
    std::cout << message << std::endl;
    return -1;
}

static void lock(pthread_mutex_t & mutex){
    pthread_mutex_lock(&mutex);
}
static void unlock(pthread_mutex_t & mutex){
    pthread_mutex_unlock(&mutex);
}

pthread_mutex_t quitMutex = PTHREAD_MUTEX_INITIALIZER;
bool globalQuit = false;
pthread_mutex_t stateChangedMutex = PTHREAD_MUTEX_INITIALIZER;
bool stateChanged = false;
pthread_mutex_t stateMutex = PTHREAD_MUTEX_INITIALIZER;
int stateNumber = 0;
pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;
bool listStates = false;

void * handleCharacter (void * arg){
    const char * module = (const char *)arg;
    
    Py_Initialize();
        
    /* NOTE need to make sure we are trying to load in the same directory (is there a work around?) */
    PyRun_SimpleString("import sys"); 
    PyRun_SimpleString("sys.path.append('./')");
    
    try {
        Character * character = new Character(module);
        bool quit = false;
        while(!quit){
            character->act();
            lock(quitMutex);
            if (globalQuit){
                quit = true;
            }
            unlock(quitMutex);
            
            lock(stateChangedMutex);
            if (stateChanged){
                lock(stateMutex);
                character->changeState(stateNumber);
                unlock(stateMutex);
                stateChanged = false;
            }
            unlock(stateChangedMutex);
            
            lock(listMutex);
            if (listStates){
                character->listStates();
                listStates = false;
            }
            unlock(listMutex);
        }
        delete character;
    } catch (const PyException & ex){
        error("Problem with module! Reason: " + ex.getReason());
        unlock(stateMutex);
        unlock(stateChangedMutex);
    }
    
    Py_Finalize();
    
    lock(quitMutex);
    globalQuit = true;
    unlock(quitMutex);
}

int main(int argc, char ** argv){
    if (argc > 1){
        
        std::cout << "Type 'quit' or 'exit' to quit." << std::endl;
        std::cout << "To change to state zero type 'zero'." << std::endl; 
        std::cout << "To list all states type 'list'." << std::endl;
        
        pthread_t characterThread;
        int ret = pthread_create(&characterThread, NULL, handleCharacter, (void *)argv[1]);
            
        bool quit = false;
        while (!quit){
            std::string keys;
            std::cout << "Enter state number: ";
            std::cin >> keys;
            
            lock(quitMutex);
            if (globalQuit){
                globalQuit = false;
                
                lock(stateChangedMutex);
                stateChanged = false;
                unlock(stateChangedMutex);
                
                lock(listMutex);
                listStates = false;
                unlock(listMutex);
                
                std::cout << "Character crashed or had a problem! Restarting..." << std::endl;
                
                ret = pthread_create(&characterThread, NULL, handleCharacter, (void *)argv[1]);
                keys.clear();
                std::cin.clear();
            }
            unlock(quitMutex);
            
            if (ret == -1){
                quit = true;
            }
            
            if (keys == "quit" || keys == "exit" || keys == "q"){
                quit = true;
                lock(quitMutex);
                globalQuit = true;
                unlock(quitMutex);
                std::cout << "Shutting Down." << std::endl;
            } else if (keys == "help" || keys == "?"){
                std::cout << "'help', '?' to print help" << std::endl;
                std::cout << "'quit', 'exit', 'q' to exit" << std::endl;
                std::cout << "'list', to print available states" << std::endl;
                std::cout << "'zero', to change to state zero" << std::endl;
            } else if (keys == "zero"){
                lock(stateChangedMutex);
                stateChanged = true;
                lock(stateMutex);
                stateNumber = 0;
                unlock(stateMutex);
                unlock(stateChangedMutex);
            } else if (keys == "list"){
                lock(listMutex);
                listStates = true;
                unlock(listMutex);
            } else {
                int number = atoi(keys.c_str());
                if (number != 0){
                    lock(stateChangedMutex);
                    stateChanged = true;
                    lock(stateMutex);
                    stateNumber = number;
                    unlock(stateMutex);
                    unlock(stateChangedMutex);
                }
            }
            std::cin.clear();
        }
        
        pthread_join(characterThread, NULL);
        
        return 0;
    }
    
    std::cout << "Usage: ./test character_module_name" << std::endl;
    
    return 0;
    
}
