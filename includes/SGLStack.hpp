#ifndef SGLSTACK_H
#define SGLSTACK_H

#include <mutex>
#include <optional>
#include <sched.h>

#define RELAXED std::memory_order_relaxed
#define RELEASE std::memory_order_release
#define ACQUIRE std::memory_order_acquire
#define ACQ_REL std::memory_order_acq_rel
#define SEQ_CST std::memory_order_seq_cst

template <typename T> 
class SGLStack {
    private:
        struct Node {
            T data;
            Node* down;
            Node(T val) : data(val), down(nullptr) {}
        };
        std::mutex mtx;
        Node* top;

    public:
        SGLStack() : top(nullptr) {}

        ~SGLStack() {
            Node* curr = top;
            while (curr != nullptr) {
                Node* temp = curr;
                curr = curr->down;
                delete temp;
            }
        }

        void push(T val) {
            mtx.lock();
            Node* node = new Node(val);
            node->down = top;
            top = node;
            mtx.unlock();
        }

        std::optional<T> pop() {
            mtx.lock();
            if (top == nullptr) {
                mtx.unlock();
                return std::nullopt;
            }
            Node* temp = top;
            top = top->down;
            T val = temp->data;
            delete temp;
            mtx.unlock();
            return val;
        }
};

#endif // SGLSTACK_H
