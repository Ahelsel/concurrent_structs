/*



    THIS IS A WORK IN PROGRESS



*/
#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <cstdlib>
#include <string>

class DynamicArray {
    private:
        int* arr = (int*)malloc(sizeof(int) * 100);
        int size = 0;
        int max_size = 100; // start at 100 int size

    public:
        void put(int input) {
            if (size == max_size) {
                max_size *= 2;
                arr = (int*)realloc(arr, max_size * sizeof(int)); // double arr size on realloc
            }
    
            arr[size] = input;
            size += 1;
        }

        int get() {
            return arr[size - 1];
            size -= 1;
            // todo: shrink array after its gets to 1/4 maximum size?
            // todo: keep minimum size 100
        }

        // print the contents of the array
        std::string toString() {
            std::string ret = "";
            for (int i = 0; i < size; i++) {
                ret.append(std::to_string(arr[i]));
                ret.append(" ");
            }
            return ret;    
        }

        int getMaxSize() {
            return max_size;
        }
};

#endif // DYNARRAY_H