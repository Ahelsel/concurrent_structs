#ifndef TSTACK_H
#define TSTACK_H

#include <atomic>
#include <optional>
#include <sched.h>

#define RELAXED std::memory_order_relaxed
#define RELEASE std::memory_order_release
#define ACQUIRE std::memory_order_acquire
#define ACQ_REL std::memory_order_acq_rel
#define SEQ_CST std::memory_order_seq_cst

template <typename T>
class TStack {
    private:
        struct Node {
            T val;
            std::atomic<Node*> down;
            Node(T value) : val(value), down(nullptr) {}
        };
        std::atomic<Node*> top;
        
    public:
        TStack() : top(nullptr) {}
        ~TStack() { while (pop()); }

        void push(T val) {
            Node *t, *n; 
            n = new Node(val);
            do {
                t = top.load(ACQUIRE);
                n->down.store(t);
            } while (!top.compare_exchange_strong(t, n, ACQ_REL));
        }

        std::optional<T> pop() {
            Node *t, *n;
            int v;
            do {
                t = top.load(ACQUIRE);
                if (t == NULL) {
                    return std::nullopt;
                }
                n = t->down.load();
                v = t->val;
            } while (!top.compare_exchange_strong(t,n,ACQ_REL));
            return v;
        }
};

#endif  // TSTACK_H