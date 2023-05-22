#include <iostream>
#include <thread>
#include <atomic>
#include "stack.hpp"

#include "containers.hpp"

using namespace std;

#define NUMBER 100000

ConcurrentStack<int> stack;

void push_thread()
{
    for (int i = 0; i < NUMBER; ++i)
    {
        stack.push(i);
    }
}

void pop_thread()
{
    for (int i = 0; i < NUMBER; ++i)
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

    // Create Container
    Container container;
    bool value = true;

    container.set_finished(&value);
    printf("finished: %d\n", container.get_finished());

    int verifiedPath = 34;
    container.set_verified_path(&verifiedPath);
    printf("verifiedPath: %d\n", container.get_verified_path());

    int threadStatus[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    container.set_thread_status_table(threadStatus);
    printf("threadStatus: %d\n", container.get_thread_status(3));
    container.print_thread_status();

    int status[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int i;
    for (i = 0; i < 10; i++)
    {
        container.set_thread_status(i, &status[i]);
    }
    printf("threadStatus: %d\n", container.get_thread_status(3));
    container.print_thread_status();

    return 0;
}