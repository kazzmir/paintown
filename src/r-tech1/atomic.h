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
        atomic(const T & );
        atomic(const atomic<T> & );
        ~atomic();
        T load() const noexcept;
        void store(const T & ) noexcept;
        operator T() noexcept;
        atomic<T> & operator=(const T & ) noexcept;
        atomic<T> & operator=( atomic<T> & ) noexcept;
        atomic<T> & operator+(const T & ) noexcept;
        atomic<T> & operator-(const T & ) noexcept;
        atomic<T> & operator+=(const T & ) noexcept;
        atomic<T> & operator-=(const T & ) noexcept;
        atomic<T> & operator&=(const T & ) noexcept;
        atomic<T> & operator|=(const T & ) noexcept;
        atomic<T> & operator^=(const T & ) noexcept;
        bool operator!=( const atomic<T> & ) noexcept;
        bool operator==( const atomic<T> & ) noexcept;
        bool operator>( const atomic<T> & ) noexcept;
        bool operator<( const atomic<T> & ) noexcept;
        bool operator!=(const T & ) noexcept;
        bool operator==(const T & ) noexcept;
        bool operator>(const T & ) noexcept;
        bool operator<(const T & ) noexcept;
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