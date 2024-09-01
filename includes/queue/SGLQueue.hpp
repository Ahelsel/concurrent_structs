#ifndef SGLQUEUE_H
#define SGLQUEUE_H

#include <mutex>
#include <optional>

#define RELAXED std::memory_order_relaxed
#define RELEASE std::memory_order_release
#define ACQUIRE std::memory_order_acquire
#define ACQ_REL std::memory_order_acq_rel
#define SEQ_CST std::memory_order_seq_cst

template <typename T>
class SGLQueue {
    private:
        struct Node {
            T data;
            Node* next;
            Node(T val) : data(val), next(nullptr) {}
        };
        Node* front;
        Node* back;
        std::mutex mtx;

    public:
        SGLQueue() : front(nullptr), back(nullptr) {}

        ~SGLQueue() {
            while (front != nullptr) {
                Node* temp = front;
                front = front->next;
                delete temp;
            }
        }

        void push(T val) {
            mtx.lock();
            Node* node = new Node(val);
            if (back == nullptr)
                front = back = node;
            else {
                back->next = node;
                back = node;
            }
            mtx.unlock();
        }

        std::optional<T> pop() {
            mtx.lock();
            if (front == nullptr) {
                mtx.unlock();
                return std::nullopt;
            }
            Node* temp = front;
            T val = temp->data;
            front = front->next;
            if (front == nullptr)
                back = nullptr;
            delete temp;
            mtx.unlock();
            return val;
        }
};

#endif // SGLQUEUE_H