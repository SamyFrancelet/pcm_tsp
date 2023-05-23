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

    void set(T* value, int index) {
        uint64_t stamp = 0;
        T* expected = &object.get(stamp)[index];
        std::cout << "cas: " << *expected << " " << *value << std::endl;
        while (!object.cas(expected, value, stamp, stamp + 1)) {
            expected = &object.get(stamp)[index];
        }
    }

    T* get() {
        uint64_t stamp = 0;
        return object.get(stamp);
    }

    T* get(int index) {
        uint64_t stamp = 0;
        T* current = object.get(stamp);
        return &current[index];
    }

    void release() {
        uint64_t stamp = 0;
        T current = object.get(stamp);
        if (current != nullptr) {
            delete[] current;
            object.set(nullptr, stamp + 1);
        }
    }
};

#endif