#ifndef ELIMSGLSTACK_H
#define ELIMSGLSTACK_H

#pragma once

#include <atomic>
#include <cstdlib>
#include <mutex>
#include <optional>
#include <sched.h>

#define RELAXED std::memory_order_relaxed
#define RELEASE std::memory_order_release
#define ACQUIRE std::memory_order_acquire
#define ACQ_REL std::memory_order_acq_rel
#define SEQ_CST std::memory_order_seq_cst

template <typename T>
class EliminationSGLStack {
    private:
        struct Node {
            T data;
            std::atomic<Node*> down;
            std::atomic<bool> marked;
            Node(T val) : data(val), down(nullptr), marked(false) {}
        };
        std::mutex mtx;
        std::atomic<Node*> top;
        T val;

        static constexpr size_t elim_arr_size = 10;
        std::atomic<Node*> elim_arr[elim_arr_size];

    public:
        EliminationSGLStack() : top(nullptr) {}
        ~EliminationSGLStack() { while (pop()); }

        void push(T val) {
            Node* n = new Node(val);
            bool elim_success = false;

            //try pushing normally
            if (mtx.try_lock()) {
                Node* t = top.load();
                n->down.store(t);
                if (top.compare_exchange_strong(t, n)) {
                    mtx.unlock();
                    return;
                }
                mtx.unlock();
            }

            // try elimination
            int slot = rand() % elim_arr_size;
            Node* t = elim_arr[slot].load();
            if (t == nullptr && elim_arr[slot].compare_exchange_strong(t, n))
                elim_success = true;

            // if elim fails, do it normally
            if (!elim_success) {
                mtx.lock();
                Node* t;
                do {
                    t = top.load();
                    n->down.store(t);
                } while (!top.compare_exchange_strong(t, n));
                mtx.unlock();
            }
        }

        std::optional<T> pop() {
            Node* t, *n;
            T val;
            
            // try elim arr first
            int slot = rand() % elim_arr_size;
            t = elim_arr[slot].load();
            if (t != nullptr && !t->marked) {
                if (elim_arr[slot].compare_exchange_strong(t, nullptr)) {
                    val = t->data;
                    delete t;
                    return val;
                }
            }

            // else, regular pop from stack
            mtx.lock();
            do {
                t = top.load();
                if (t == nullptr) {
                    mtx.unlock();
                    return std::nullopt;
                }
                n = t->down.load();
                val = t->data;
            } while (!top.compare_exchange_strong(t, n));
            mtx.unlock();

            t->marked = true;
            return val;
        }
};

#endif // ELIMSGLSTACK_H