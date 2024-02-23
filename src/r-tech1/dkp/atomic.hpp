#ifdef DEVKITPRO

//#include "r-tech1/atomic.h"


template<typename T>
Atomic::atomic<T>::atomic(T arg):
value(arg){

}

template<typename T>
Atomic::atomic<T>::atomic(atomic<T> & copy):
value(copy.arg){

}

template<typename T>
Atomic::atomic<T>::~atomic(){

}

template<typename T>
T Atomic::atomic<T>::load() const noexcept {
    return value;
}

template<typename T>
void Atomic::atomic<T>::store(T arg) noexcept {
    value = arg;
}

template<typename T>
Atomic::atomic<T>::operator T() noexcept {
    return value;
}

template<typename T>
T Atomic::atomic<T>::operator=( T arg ) noexcept {}

template<typename T>
T Atomic::atomic<T>::operator=( atomic & copy) noexcept {}

template<typename T>
T Atomic::atomic<T>::operator+( T arg ) noexcept {}

template<typename T>
T Atomic::atomic<T>::operator-( T arg ) noexcept {}

template<typename T>
T Atomic::atomic<T>::operator+=( T arg ) noexcept {}

template<typename T>
T Atomic::atomic<T>::operator-=( T arg ) noexcept {}

template<typename T>
T Atomic::atomic<T>::operator&=( T arg ) noexcept {}

template<typename T>
T Atomic::atomic<T>::operator|=( T arg ) noexcept {}

template<typename T>
T Atomic::atomic<T>::operator^=( T arg ) noexcept {}

template<typename T>
bool Atomic::atomic<T>::operator!=( T arg ) noexcept { return value != arg; }

template<typename T>
bool Atomic::atomic<T>::operator==( T arg ) noexcept { return value == arg; }

template<typename T>
bool Atomic::atomic<T>::operator>( T arg ) noexcept { return value > arg; }

template<typename T>
bool Atomic::atomic<T>::operator<( T arg ) noexcept { return value < arg; }

#endif