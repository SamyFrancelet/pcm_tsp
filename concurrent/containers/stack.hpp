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
                //Print error
                exit(-1);
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

/*template <typename T>
class ConcurrentStack
{
private:
    struct Node
    {
        T value;
        Node *next;

        Node(const T value) : value(value), next(nullptr) {}
    };

    atomic_stamped<Node *> top;

public:
    ConcurrentStack() : top(nullptr, 0) {}

    void push(const T value)
    {
        Node new_node = new Node(value);
        uint64_t stamp = 0;
        while (true)
        {
            Node current_top = top.get(stamp);
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
            Node current_top = top.get(stamp);

            if (current_top == nullptr)
            {
                return nullptr;
            }
            Node new_top = current_top->next;
            T value = current_top->value;
            if (top.cas(current_top, new_top, stamp, stamp + 1))
            {
                return value;
            }
        }
    }

    bool empty() const
    {
        uint64_t stamp;
        return !top.get(&stamp);
    }
};
*/

/*#include <iostream>
#include <cstdlib>
#include <memory>
#include "atomic.hpp"

template <typename T>
class Stack{
private:
    class Slot
    {
        boolean full = false;
        volatile T value = null;
    } Slot[] stack_;
    int size_;
    atomic_stamped top_ = new atomic_stamped(0); // array index
public:
    Stack(int size)
    {
        size_ = size;
        stack_ = (Slot[]) new Object[size];
        for (int i = 0; i < capacity; i++)
        {
            stack_[i] = new Slot();
        }
    }
public
    void push(T value) throws FullException
    {
        while (true)
        {
            int i = top.getAndIncrement();
            if (i > capacity - 1)
            { // is stack full?
                throw new FullException();
            }
            else if (i > 0)
            { // i in range, slot reserved
                stack[i].value = value;
                stack[i].full = true; // push fulfilled
                return;
            }
        }
    }
public
    T pop() throws EmptyException
    {
        while (true)
        {
            int i = top.getAndDecrement();
            if (i < 0)
            { // is stack empty?
                throw new EmptyException();
            }
            else if (i < capacity - 1)
            {
                while (!stack[i].full)
                {
                };
                T value = stack[i].value;
                stack[i].full = false;
                return value; // pop fulfilled
            }
        }
    }
}*/

/*
using namespace std;

// Define the default capacity of the stack
#define SIZE 10

// A class to represent a stack
template <typename T>
class Stack
{
private:
    struct Node
    {
        T value;
        std::shared_ptr<Node> next;
        Node(const T &value) : value(value) {}
    };
    std::shared_ptr<Node> head;

public:
    // Constructor to initialize the stack
    Stack(int size = SIZE)
    {
        head = nullptr;
    }

    // Destructor to free memory allocated to the stack
    ~Stack()
    {
    }

    // Utility function to add an element `x` to the stack
    void push(const T &value)
    {
        cout << "Inserting " << endl;
        std::shared_ptr<Node> new_node = std::make_shared<Node>(value);
        std::shared_ptr<Node> old_head;
        do
        {
            old_head = head;
            new_node->next = old_head;
        } while (!atomic_stamped::cas<std::shared_ptr<Node>>(old_head, head, old_head->next, head->next));
    }

    // Utility function to pop a top element from the stack
    int pop()
    {
        // check for stack underflow
        if (empty())
        {
            cout << "Underflow\nProgram Terminated\n";
            exit(EXIT_FAILURE);
        }

        cout << "Removing " << endl;
        std::shared_ptr<Node> old_head;
        do
        {
            old_head = head;
            if (!old_head)
            {
                return nullptr;
            }
        } while (!atomic_stamped::cas<std::shared_ptr<Node>>(old_head, head, old_head->next, head->next));
        return std::make_shared<T>(old_head->value);
    }

    bool empty() const
    {
        std::shared_ptr<Node> current_head = std::atomic_load(&head);
        return !current_head;
    }
};
*/