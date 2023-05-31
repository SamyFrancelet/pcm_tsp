#ifndef C_OBJECT_HPP
#define C_OBJECT_HPP

#include "atomic.hpp"
#include <iostream>

template <typename T>
class CObject {
private:
    atomic_stamped<T> object;

public:
    CObject() : object(nullptr, 0) {}

    void set(T* value) {
        uint64_t stamp = 0;
        T* expected = object.get(stamp);
        while (!object.cas(expected, value, stamp, stamp + 1)) {
            expected = object.get(stamp);
        }
    }

    T* get() {
        uint64_t stamp = 0;
        return object.get(stamp);
    }
};

#endif