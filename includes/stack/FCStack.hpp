#ifndef FCSTACK_H
#define FCSTACK_H

#include <atomic>
#include <mutex>
#include <optional>
#include <sched.h>
#include <vector>

#define RELAXED std::memory_order_relaxed
#define RELEASE std::memory_order_release
#define ACQUIRE std::memory_order_acquire
#define ACQ_REL std::memory_order_acq_rel
#define SEQ_CST std::memory_order_seq_cst

template <typename T>
class FlatCombiningStack {
    private:
        struct Node {
            T data;
            std::atomic<Node*> down;
            Node(T val) : data(val), down(nullptr) {}
        };

        struct Operation {
            enum Type {PUSH,
                       POP} opType;
            std::atomic<bool> completed;
            std::optional<T> result;
            T value;

            Operation(Type type, T val = T()) : opType(type), value(val), completed(false) {}
        };

        std::mutex mtx;
        std::atomic<Node*> top;
        std::vector<Operation*> ops;

        void combine() {
            for (auto& op : ops) {
                if (op->opType == Operation::PUSH) {
                    Node* n = new Node(op->value);
                    n->down = top.load();
                    top = n;
                } else if (op->opType == Operation::POP) {
                    Node* t = top.load();
                    if (t == nullptr) {
                        op->result = std::nullopt;
                    } else {
                        top = t->down.load();
                        op->result = t->data;
                        delete t;
                    }
                }
                op->completed = true;
            }
        }

    public:
        FlatCombiningStack() : top(nullptr) {}
        ~FlatCombiningStack() { while (pop()); }

        void push(T val) {
            Operation op(Operation::PUSH, val);
            mtx.lock();
            ops.push_back(&op);
            if (ops.size() == 1) {
                combine();
                ops.clear();
            }
            mtx.unlock();
            while (!op.completed.load());
        }

        std::optional<T> pop() {
            Operation op(Operation::POP);
            mtx.lock();
            ops.push_back(&op);
            if (ops.size() == 1) {
                combine();
                ops.clear();
            }
            mtx.unlock();
            while (!op.completed.load());
            return op.result;
        }
};

#endif // FCSTACK_H