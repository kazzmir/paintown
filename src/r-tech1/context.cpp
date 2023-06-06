#include "context.h"

namespace Util {

Context::Context(): parent(nullptr), done(false) {}

Context::Context(const Context* parent):
parent(parent),
done(false) {}

bool Context::isDone() const {
    return done.load() || (parent != nullptr && parent->isDone());
}

AutoCancel Context::autoCancel() {
    return AutoCancel(*this);
}

void Context::cancel() {
    done.store(true);
}

AutoCancel::AutoCancel(Context & context):
context(context) {
}

AutoCancel::~AutoCancel() {
    context.cancel();
}

}
