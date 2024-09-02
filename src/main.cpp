#include <ctime>
#include <iostream>
#include <thread>
#include <vector>
// Queues
#include "../includes/queue/FCQueue.hpp"
#include "../includes/queue/MSQueue.hpp"
#include "../includes/queue/SGLQueue.hpp"
// Stacks
#include "../includes/stack/ElimSGLStack.hpp"
#include "../includes/stack/ElimTStack.hpp"
#include "../includes/stack/FCStack.hpp"
#include "../includes/stack/SGLStack.hpp"
#include "../includes/stack/TStack.hpp"


#define NUM_THREADS 5
#define NUM_ITERATIONS 100

// global thread vector
std::vector<std::thread> threads(NUM_THREADS);

int main() {

    std::cout << "Program Starting..." << std::endl;

    TStack<int> stack;

    for (int i = 0; i < 10; i++) {
        stack.push(i);
    }

    for (int i = 0; i < 10; i++) {
        int val = stack.pop().value();
        std::cout << "Val: " << val << std::endl;
    }

    std::cout << "Finished successfully" << std::endl;


}