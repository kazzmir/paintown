package coroutine

import (
    "fmt"
)

// TODO: support return values of an arbitrary type T
// Allow main function to cancel the coroutine?

type Coroutine struct {
    yieldFrom chan struct{}
    yieldTo chan struct{}
    errorOut *error
    stopped bool
}

type YieldFunc func() error

type AcceptYieldFunc func(yield YieldFunc) error

var CoroutineFinished = fmt.Errorf("coroutine finished")
var CoroutineCancelled = fmt.Errorf("coroutine cancelled")

func MakeCoroutine(user AcceptYieldFunc) *Coroutine {
    yieldTo := make(chan struct{})
    yieldFrom := make(chan struct{})

    coroutineError := CoroutineFinished

    coroutine := &Coroutine{
        yieldFrom: yieldFrom,
        yieldTo: yieldTo,
        errorOut: &coroutineError,
    }

    go func(){
        defer func(){
            coroutine.stopped = true
            close(yieldTo)
        }()
        <-yieldFrom
        err := user(func() error {
            yieldTo <- struct{}{}
            _, ok := <-yieldFrom

            if !ok {
                return CoroutineCancelled
            }

            return nil
        })
        if err != nil {
            coroutineError = err
        }
    }()

    return coroutine
}

func (coro *Coroutine) Stop() {
    if !coro.stopped {
        coro.stopped = true
        close(coro.yieldFrom)
    }
}

/* nil return means the coroutine is still running.
 * CoroutineFinished means the coroutine has finished.
 * any other non-nil error is an error from the user's function
 */
func (coro *Coroutine) Run() error {
    if coro.stopped {
        // wait for yieldTo to be closed
        <-coro.yieldTo
        return CoroutineCancelled
    }
    coro.yieldFrom <- struct{}{}
    _, ok := <-coro.yieldTo
    if !ok {
        close(coro.yieldFrom)
        return *coro.errorOut
    }
    return nil
}
