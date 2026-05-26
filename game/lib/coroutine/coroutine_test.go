package coroutine

import (
    "testing"
    "slices"
    "fmt"
    // "sync"
)

func TestCoroutine1(testing *testing.T) {
    var data []int

    v1 := func(yield YieldFunc) error {
        // fmt.Printf("v1 run 1\n")
        data = append(data, 1)
        // fmt.Printf("v1 yield 1\n")
        yield()
        // fmt.Printf("v1 run 2\n")
        data = append(data, 10, 11)
        return nil
    }

    coro := MakeCoroutine(v1)

    // fmt.Printf("coroutine main run 1\n")
    coro.Run()
    // fmt.Printf("coroutine main run 2\n")
    if !slices.Equal(data, []int{1}) {
        testing.Error("data should be [1]")
    }
    data = append(data, 5)
    coro.Run()
    // fmt.Printf("coroutine main run 3\n")
    if !slices.Equal(data, []int{1, 5, 10, 11}) {
        testing.Error("data should be [1, 5, 10, 11]")
    }
}

func TestCoroutine2(testing *testing.T){
    z := 0
    v1 := func(yield YieldFunc) error {
        for z < 10 {
            z += 1
            yield()
        }

        return nil
    }

    coro := MakeCoroutine(v1)
    for {
        err := coro.Run()
        if err != nil {
            break
        }
    }

    if z != 10 {
        testing.Error("z should be 10")
    }
}

func TestCoroutineError(test *testing.T) {
    myError := fmt.Errorf("my error")
    v1 := func(yield YieldFunc) error {
        yield()
        return myError
    }

    coro := MakeCoroutine(v1)
    var err error
    for range 10 {
        err = coro.Run()
        if err != nil {
            break
        }
    }

    if err != myError {
        test.Error("error should have been myError")
    }
}

// Issue 1: Panic on repeated Run() after natural completion
// After coroutine finishes, Run() closes yieldFrom but doesn't set stopped=true.
// A subsequent Run() call will send on a closed channel, causing a panic.
func TestRunAfterNaturalCompletion(test *testing.T) {
    defer func() {
        if r := recover(); r != nil {
            test.Errorf("Run() panicked after coroutine finished: %v", r)
        }
    }()

    v1 := func(yield YieldFunc) error {
        return nil // finish immediately
    }

    coro := MakeCoroutine(v1)

    // First Run() - coroutine finishes
    err := coro.Run()
    if err != CoroutineFinished {
        test.Errorf("expected CoroutineFinished, got %v", err)
    }

    // Second Run() - should not panic, should return an error
    err = coro.Run()
    if err == nil {
        test.Error("expected error on Run() after completion")
    }
}

// Issue 2: Panic on Stop() after natural completion
// If coroutine finishes naturally and Run() closes yieldFrom,
// calling Stop() will try to close an already-closed channel.
func TestStopAfterNaturalCompletion(test *testing.T) {
    defer func() {
        if r := recover(); r != nil {
            test.Errorf("Stop() panicked after coroutine finished: %v", r)
        }
    }()

    v1 := func(yield YieldFunc) error {
        return nil
    }

    coro := MakeCoroutine(v1)

    // Run until completion
    err := coro.Run()
    if err != CoroutineFinished {
        test.Errorf("expected CoroutineFinished, got %v", err)
    }

    // Stop() after natural completion - should not panic
    coro.Stop()
}

// Issue 3: Yield return value should be checked for cancellation
// When Stop() is called, the yield function returns CoroutineCancelled.
// The user function should check this and exit gracefully.
func TestYieldReturnValueOnCancel(test *testing.T) {
    var yieldError error
    var iterations int

    v1 := func(yield YieldFunc) error {
        for range 100 {
            iterations++
            err := yield()
            if err != nil {
                yieldError = err
                return err // properly exit on cancellation
            }
        }
        return nil
    }

    coro := MakeCoroutine(v1)

    // Run a few iterations
    coro.Run()
    coro.Run()
    coro.Run()

    // Stop mid-execution
    coro.Stop()

    // The yield should have returned CoroutineCancelled
    // Note: This requires one more Run() to propagate the cancellation
    coro.Run()

    if yieldError != CoroutineCancelled {
        test.Errorf("expected yield to return CoroutineCancelled, got %v", yieldError)
    }

    if iterations != 3 {
        test.Errorf("coroutine should have stopped early due to cancellation: %v", iterations)
    }
}

// Issue 4: Race condition on stopped flag
// Concurrent calls to Run() and Stop() can race on the stopped field.
/*
func TestConcurrentRunStop(test *testing.T) {
    defer func() {
        if r := recover(); r != nil {
            test.Errorf("concurrent Run/Stop caused panic: %v", r)
        }
    }()

    v1 := func(yield YieldFunc) error {
        for i := 0; i < 1000; i++ {
            if err := yield(); err != nil {
                return err
            }
        }
        return nil
    }

    coro := MakeCoroutine(v1)

    var wg sync.WaitGroup
    wg.Add(2)

    // Goroutine 1: repeatedly call Run()
    go func() {
        defer wg.Done()
        for i := 0; i < 50; i++ {
            err := coro.Run()
            if err != nil {
                return
            }
        }
    }()

    // Goroutine 2: call Stop() after a short delay
    go func() {
        defer wg.Done()
        for i := 0; i < 10; i++ {
            coro.Run()
        }
        coro.Stop()
    }()

    wg.Wait()
}
*/

// Issue 5: Multiple concurrent Run() calls
// Calling Run() from multiple goroutines simultaneously causes races.
/*
func TestConcurrentRun(test *testing.T) {
    defer func() {
        if r := recover(); r != nil {
            test.Errorf("concurrent Run() caused panic: %v", r)
        }
    }()

    counter := 0
    v1 := func(yield YieldFunc) error {
        for i := 0; i < 100; i++ {
            counter++
            if err := yield(); err != nil {
                return err
            }
        }
        return nil
    }

    coro := MakeCoroutine(v1)

    var wg sync.WaitGroup

    // Launch multiple goroutines all calling Run()
    for i := 0; i < 5; i++ {
        wg.Add(1)
        go func() {
            defer wg.Done()
            for j := 0; j < 20; j++ {
                err := coro.Run()
                if err != nil {
                    return
                }
            }
        }()
    }

    wg.Wait()
}
*/

// Issue 6: Double Stop() call
// Calling Stop() twice should not panic.
func TestDoubleStop(test *testing.T) {
    defer func() {
        if r := recover(); r != nil {
            test.Errorf("double Stop() caused panic: %v", r)
        }
    }()

    v1 := func(yield YieldFunc) error {
        yield()
        return nil
    }

    coro := MakeCoroutine(v1)
    coro.Run()

    // First Stop()
    coro.Stop()

    // Second Stop() - should not panic
    coro.Stop()
}
