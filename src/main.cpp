#include <ctime>
#include <iostream>
#include <thread>
#include <vector>

#define NUM_THREADS 5
#define NUM_ITERATIONS 100

// global thread vector
std::vector<std::thread> threads(NUM_THREADS);

template <typename T>
void put_tid(int tid, int iterations, T primitive) {
    for (int i = 0; i < iterations; i++) {
        primitive->put(tid);
    }
}


int main() {

    std::cout << "Program Starting..." << std::endl;

    /* create structs here */ 

    for (unsigned int i = 0; i < NUM_THREADS; i++) {
        // i + 1 == tid
        // put_tid tells the thread to add its own tid into the shared struct
        // NUM_ITERATIONS is how many times it will put its own tid
        // threads[i] = std::thread(put_tid, i + 1, NUM_ITERATIONS, /* STRUCT GOES HERE */);
    }

    for (auto& th : threads) {
        th.join();
    }

    /* add print statements for testing if needed */

    for (int i = 0; i < NUM_THREADS * NUM_ITERATIONS; i++) {
        
    }

    /* add print statements for testing if needed */

}



