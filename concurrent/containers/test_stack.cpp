#include <iostream>
#include <thread>
#include <atomic>
#include "stack.hpp"

using namespace std;

ConcurrentStack<int> stack;

void push_thread()
{
    for (int i = 0; i < 100000; ++i)
    {
        stack.push(i);
    }
}

void pop_thread()
{
    for (int i = 0; i < 100000; ++i)
    {
        stack.pop();
    }
}

int main()
{
    std::cout << "Hello tester!\n";

    // Create two threads to push elements onto the stack
    thread t1(push_thread);
    thread t2(push_thread);

    // Create two threads to pop elements off the stack
    thread t3(pop_thread);
    thread t4(pop_thread);

    // Wait for all threads to finish
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    // Check if the stack is empty
    if (stack.empty())
    {
        cout << "Test passed" << endl;
    }
    else
    {
        cout << "Test failed" << endl;
    }

    std::cout << "Goodbye, tester!\n";

    return 0;
}