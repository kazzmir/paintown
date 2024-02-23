#ifndef __r_tech1_atomic
#define __r_tech1_atomic

// Overloaded for DevKitPro because it doesn't have libatomic.a
#if !defined(DEVKITPRO)
#include <atomic>
#endif

namespace Atomic {
#if !defined(DEVKITPRO)
    using namespace std;
#else
    // Forward declared
    template<typename T>
    class atomic{
    public:
        atomic(T arg);
        atomic(atomic<T> & copy);
        ~atomic();
        T load() const noexcept;
        void store(T arg) noexcept;
        operator T() noexcept;
        T operator=( T arg ) noexcept;
        T operator=( atomic & copy) noexcept;
        T operator+( T arg ) noexcept;
        T operator-( T arg ) noexcept;
        T operator+=( T arg ) noexcept;
        T operator-=( T arg ) noexcept;
        T operator&=( T arg ) noexcept;
        T operator|=( T arg ) noexcept;
        T operator^=( T arg ) noexcept;
        bool operator!=( T arg ) noexcept;
        bool operator==( T arg ) noexcept;
        bool operator>( T arg ) noexcept;
        bool operator<( T arg ) noexcept;
    protected:
        T value;
    };

    // Implementations
    #ifdef DEVKITPRO
        #include "dkp/atomic.hpp"
    #endif
#endif
}

#endif