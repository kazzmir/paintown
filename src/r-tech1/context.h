#pragma once

#include <atomic>

namespace Util {

class Context;

class AutoCancel{
public:
    AutoCancel(Context & context);
    ~AutoCancel();

    Context & context;
};

/* context, similar to go context */
class Context{
public:
    Context();
    Context(const Context* parent);

    bool isDone() const;
    void cancel();

    AutoCancel autoCancel();

    const Context* parent;
    std::atomic<bool> done;
};

}
