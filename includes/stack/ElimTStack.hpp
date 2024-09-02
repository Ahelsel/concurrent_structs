#ifndef ELIMTSTACK_H
#define ELIMTSTACK_H

#pragma once

#include <atomic>
#include <cstdlib>
#include <optional>
#include <sched.h>

#define RELAXED std::memory_order_relaxed
#define RELEASE std::memory_order_release
#define ACQUIRE std::memory_order_acquire
#define ACQ_REL std::memory_order_acq_rel
#define SEQ_CST std::memory_order_seq_cst

template <typename T>
class Elimination_TStack {
    private:
        struct Node {
            T val;
            std::atomic<Node*> down;
            std::atomic<bool> marked;
            Node(T value) : val(value), down(nullptr), marked(false) {}
        };
        std::atomic<Node*> top;
        static constexpr size_t elim_arr_size = 10;
        std::atomic<Node*> elim_arr[elim_arr_size];

    public:
        Elimination_TStack() : top(nullptr) {}
        ~Elimination_TStack() { while (pop()); }

        void push(T val) {
            Node* n = new Node(val);
            Node* t;
            bool elim_success = false;
            
            // try pushing normally
            t = top.load();
            n->down.store(t);
            if (top.compare_exchange_strong(t, n)) {
                return;
            }
            
            // else, try elimination
            int slot = rand() % elim_arr_size;
            t = elim_arr[slot].load();
            if (t == nullptr && elim_arr[slot].compare_exchange_strong(t, n)) {
                elim_success = true;
            }

            // try push normally
            if (!elim_success) {
                do {
                    t = top.load();
                    n->down.store(t);
                } while (!top.compare_exchange_strong(t, n));
            }
        }

        std::optional<T> pop() {
            Node* t, *n;
            T val;

            // look in elim arr
            int slot = rand() % elim_arr_size;
            t = elim_arr[slot].load();
            if (t != nullptr && !t->marked) {
                if (elim_arr[slot].compare_exchange_strong(t, nullptr)) {
                    val = t->val;
                    delete t;
                    return val;
                }
            }

            // regular pop 
            do {
                t = top.load();
                if (t == nullptr) {
                    return std::nullopt;
                }
                n = t->down.load();
                val = t->val;
            } while (!top.compare_exchange_strong(t, n));

            // delete node 
            t->marked = true;
            return val;
        }
};

#endif // ELIMTSTACK_H