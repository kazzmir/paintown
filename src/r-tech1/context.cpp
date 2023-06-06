#include "context.h"

Context::Context(): parent(nullptr), done(false) {}

Context::Context(const Context* parent):
parent(parent),
done(false) {}

bool Context::isDone() const {
    return done.load() || (parent != nullptr && parent->isDone());
}

void Context::cancel() {
    done.store(true);
}
