#include <iostream>
#include <pthread.h>

#include <Python.h>

#include "character.h"

using namespace std;

static int error(const std::string & message){
    std::cout << message << std::endl;
    return -1;
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
    
    try {
        Character * character = new Character(module);
        bool quit = false;
        while(!quit){
            character->act();
            pthread_mutex_lock(&quitMutex);
            if (globalQuit){
                quit = true;
            }
            pthread_mutex_unlock(&quitMutex);
            
            pthread_mutex_lock(&stateChangedMutex);
            if (stateChanged){
                pthread_mutex_lock(&stateMutex);
                character->changeState(stateNumber);
                pthread_mutex_unlock(&stateMutex);
                stateChanged = false;
            }
            pthread_mutex_unlock(&stateChangedMutex);
            
            pthread_mutex_lock(&listMutex);
            if (listStates){
                character->listStates();
                listStates = false;
            }
            pthread_mutex_unlock(&listMutex);
        }
        delete character;
    } catch (const PyException & ex){
        error("Problem with module! Reason: " + ex.getReason());
    }
    
    pthread_mutex_lock(&quitMutex);
    globalQuit = true;
    pthread_mutex_unlock(&quitMutex);
}

int main(int argc, char ** argv){
    if (argc > 1){
        
        Py_Initialize();
        
        /* NOTE need to make sure we are trying to load in the same directory (is there a work around?) */
        PyRun_SimpleString("import sys"); 
        PyRun_SimpleString("sys.path.append('./')");
        
        
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
            if (ret == -1){
                quit = true;
            }
            pthread_mutex_lock(&quitMutex);
            if (globalQuit){
                std::cout << "Killed!" << std::endl;
                quit = true;
            }
            pthread_mutex_unlock(&quitMutex);
            if (keys == "quit" || keys == "exit" || keys == "q"){
                quit = true;
                pthread_mutex_lock(&quitMutex);
                globalQuit = true;
                pthread_mutex_unlock(&quitMutex);
                std::cout << "Shutting Down." << std::endl;
            } else if (keys == "zero"){
                pthread_mutex_lock(&stateChangedMutex);
                stateChanged = true;
                pthread_mutex_lock(&stateMutex);
                stateNumber = 0;
                pthread_mutex_unlock(&stateMutex);
                pthread_mutex_unlock(&stateChangedMutex);
            } else if (keys == "list"){
                pthread_mutex_lock(&listMutex);
                listStates = true;
                pthread_mutex_unlock(&listMutex);
            } else {
                int number = atoi(keys.c_str());
                if (number != 0){
                    pthread_mutex_lock(&stateChangedMutex);
                    stateChanged = true;
                    pthread_mutex_lock(&stateMutex);
                    stateNumber = number;
                    pthread_mutex_unlock(&stateMutex);
                    pthread_mutex_unlock(&stateChangedMutex);
                }
            }
            std::cin.clear();
        }
        
        pthread_join(characterThread, NULL);
        
        Py_Finalize();
        
        return 0;
    }
    
    std::cout << "Usage: ./test character_module_name" << std::endl;
    
    return 0;
    
}
