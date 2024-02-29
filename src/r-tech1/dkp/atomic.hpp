#ifdef DEVKITPRO

//#include "r-tech1/atomic.h"


template<typename T>
Atomic::atomic<T>::atomic(const T & arg):
value(arg){

}

template<typename T>
Atomic::atomic<T>::atomic(const atomic<T> & copy):
value(copy.arg){

}

template<typename T>
Atomic::atomic<T>::~atomic(){

}

template<typename T>
T Atomic::atomic<T>::load() const noexcept {
    return this->value;
}

template<typename T>
void Atomic::atomic<T>::store(const T & arg) noexcept {
    value = arg;
}

template<typename T>
Atomic::atomic<T>::operator T() noexcept {
    return value;
}

template<typename T>
atomic<T> & Atomic::atomic<T>::operator=(const T & arg) noexcept {
    value = arg;
    return *this;
}

template<typename T>
atomic<T> & Atomic::atomic<T>::operator=( atomic<T> & copy) noexcept {
    this->value = copy.value;
    return *this;
}

template<typename T>
atomic<T> & Atomic::atomic<T>::operator+(const T & arg) noexcept {
    return *this;
}

template<typename T>
atomic<T> & Atomic::atomic<T>::operator-(const T & arg) noexcept {
    return *this;
}

template<typename T>
atomic<T> & Atomic::atomic<T>::operator+=(const T & arg) noexcept {
    this->value += arg;
    return *this;
}

template<typename T>
atomic<T> & Atomic::atomic<T>::operator-=(const T & arg) noexcept {
    this->value -= arg;
    return *this;
}

template<typename T>
atomic<T> & Atomic::atomic<T>::operator&=(const T & arg) noexcept {
    this->value &= arg;
    return *this;
}

template<typename T>
atomic<T> & Atomic::atomic<T>::operator|=(const T & arg) noexcept {
    this->value |= arg;
    return *this;
}

template<typename T>
atomic<T> & Atomic::atomic<T>::operator^=(const T & arg) noexcept {
    this->value ^= arg;
    return *this;
}

template<typename T>
bool Atomic::atomic<T>::operator!=( const atomic<T> & rhs ) noexcept { return value != rhs.value; }

template<typename T>
bool Atomic::atomic<T>::operator==( const atomic<T> & rhs ) noexcept { return value == rhs.value; }

template<typename T>
bool Atomic::atomic<T>::operator>( const atomic<T> & rhs ) noexcept { return value > rhs.value; }

template<typename T>
bool Atomic::atomic<T>::operator<( const atomic<T> & rhs ) noexcept { return value < rhs.value; }

template<typename T>
bool Atomic::atomic<T>::operator!=(const T & arg) noexcept { return value != arg; }

template<typename T>
bool Atomic::atomic<T>::operator==(const T & arg) noexcept { return value == arg; }

template<typename T>
bool Atomic::atomic<T>::operator>(const T & arg) noexcept { return value > arg; }

template<typename T>
bool Atomic::atomic<T>::operator<(const T & arg) noexcept { return value < arg; }

#endif