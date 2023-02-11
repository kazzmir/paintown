#ifndef _rtech1_message_queue_h
#define _rtech1_message_queue_h

#include <queue>
#include <string>
#include "thread.h"

/* multithreaded message queue.
 * someone puts messages in, someone takes them out.
 * FIFO order
 */
class MessageQueue{
public:
    MessageQueue();

    /* push on */
    void add(const std::string & str);
    /* true if any messages are left */
    bool hasAny();
    /* get the first message */
    std::string get();

    virtual ~MessageQueue();

    static void registerInfo(MessageQueue *);
    static void unregisterInfo(MessageQueue *);
    static void info(const std::string & str);
    static void clearInfo();
    static Util::Thread::Lock messageLock;

private:
    std::queue<std::string> messages;
    Util::Thread::Lock lock;
};

#endif
