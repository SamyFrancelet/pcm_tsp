#ifndef CONCURRENT_STACK_HPP
#define CONCURRENT_STACK_HPP

#include <memory>
#include "atomic.hpp"

template <typename T>
class ConcurrentStack
{
public:
    struct Node
    {
        T value;
        Node *next;

        Node(const T value) : value(value), next(nullptr) {}
    };

    atomic_stamped<Node> top;

public:
    ConcurrentStack() : top(nullptr, 0) {}

    void push(const T value)
    {
        Node* new_node = new Node(value);
        uint64_t stamp = 0;
        while (true)
        {
            Node* current_top = top.get(stamp);
            new_node->next = current_top;
            if (top.cas(current_top, new_node, stamp, stamp + 1))
            {
                break;
            }
        }
    }


    T pop()
    {
        uint64_t stamp = 0;
        while (true)
        {
            Node* current_top = top.get(stamp);

            if (current_top == nullptr)
            {
                //printf("Stack is empty\n");
                continue;
            }
            Node* new_top = current_top->next;
            T value = current_top->value;
            if (top.cas(current_top, new_top, stamp, stamp + 1))
            {
                delete current_top;
                return value;
            }
        }
    }

    bool empty()
    {
        uint64_t stamp = 0;
        Node* current_top = top.get(stamp);
        return current_top == nullptr;
    }
};

#endif