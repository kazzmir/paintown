#pragma once

#include <atomic>

/* context, similar to go context */
class Context{
public:
    Context();
    Context(const Context* parent);

    bool isDone() const;
    void cancel();

    const Context* parent;
    std::atomic<bool> done;
};
