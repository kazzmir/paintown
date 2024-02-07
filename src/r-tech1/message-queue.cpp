#include <queue>
#include <string>
#include <vector>
#include "r-tech1/message-queue.h"
#include "r-tech1/thread.h"

using std::vector;
using std::string;
    
Util::Thread::Lock MessageQueue::messageLock;

MessageQueue::MessageQueue(){
    Util::Thread::initializeLock(&lock);
}

void MessageQueue::add(const std::string & str){
    Util::Thread::acquireLock(&lock);
    messages.push(str);
    Util::Thread::releaseLock(&lock);
}

bool MessageQueue::hasAny(){
    bool any = false;
    Util::Thread::acquireLock(&lock);
    any = messages.size() > 0;
    Util::Thread::releaseLock(&lock);
    return any;
}

std::string MessageQueue::get(){
    std::string str;
    Util::Thread::acquireLock(&lock);
    if (messages.size() > 0){
        str = messages.front();
        messages.pop();
    }
    Util::Thread::releaseLock(&lock);
    return str;
}

MessageQueue::~MessageQueue(){
    Util::Thread::destroyLock(&lock);
}

static vector<string> messageBuffer;
/* should support infinite queues eventually */
static MessageQueue * current = NULL;
void MessageQueue::registerInfo(MessageQueue * queue){
    Util::Thread::acquireLock(&messageLock);
    current = queue;
    if (current != NULL){
        /* push all saved messages immediately to the queue */
        for (vector<string>::iterator it = messageBuffer.begin(); it != messageBuffer.end(); it++){
            current->add(*it);
        }
        messageBuffer.clear();
    }
    Util::Thread::releaseLock(&messageLock);
}

/* clear any buffered messages */
void MessageQueue::clearInfo(){
    Util::Thread::acquireLock(&messageLock);
    messageBuffer.clear();
    Util::Thread::releaseLock(&messageLock);
}

void MessageQueue::unregisterInfo(MessageQueue * queue){
    Util::Thread::acquireLock(&messageLock);
    if (current == queue){
        current = NULL;
    }
    Util::Thread::releaseLock(&messageLock);
}

void MessageQueue::info(const std::string & str){
    Util::Thread::acquireLock(&messageLock);
    if (current != NULL){
        current->add(str);
    } else {
        /* save it for later until a message-queue is registered */
        messageBuffer.push_back(str);
    }
    Util::Thread::releaseLock(&messageLock);
}
