#ifndef FCQUEUE_H
#define FCQUEUE_H

#pragma once

#include <atomic>
#include <mutex>
#include <optional>
#include <vector>

#define RELAXED std::memory_order_relaxed
#define RELEASE std::memory_order_release
#define ACQUIRE std::memory_order_acquire
#define ACQ_REL std::memory_order_acq_rel
#define SEQ_CST std::memory_order_seq_cst

template <typename T>
class FlatCombiningQueue {
    private:
        struct Node {
            T data;
            std::atomic<Node*> next;
            Node(T val = T()) : data(val), next(nullptr) {}
        };

        struct Operation {
            enum Type {PUSH, POP} opType;
            std::atomic<bool> completed;
            std::optional<T> result;
            T value;

            Operation(Type type, T val = T()) : opType(type), value(val), completed(false) {}
        };

        std::atomic<Node*> head;
        std::atomic<Node*> tail;
        std::mutex mtx;
        std::vector<Operation*> ops;

        void combine() {
            for (auto& op : ops) {
                if (op->opType == Operation::PUSH) {
                    Node *t, *n = new Node(op->value);
                    while (true) {
                        t = tail.load();
                        Node* null_node = nullptr;
                        if (t->next.compare_exchange_strong(null_node, n)) {
                            tail.compare_exchange_strong(t, n);
                            break;
                        }
                    }
                } else if (op->opType == Operation::POP) {
                    Node *h, *n;
                    while (true) {
                        h = head.load();
                        n = h->next.load();
                        if (n == nullptr) {
                            op->result = std::nullopt;
                            break;
                        }
                        if (head.compare_exchange_strong(h, n)) {
                            op->result = n->data;
                            delete h;
                            break;
                        }
                    }
                }
                op->completed = true;
            }
        }

    public:
        FlatCombiningQueue() {
            Node* dummy = new Node();
            head.store(dummy, RELAXED);
            tail.store(dummy, RELAXED);
        }

        ~FlatCombiningQueue() {
            Node* front = head.load(RELAXED);
            while (front != nullptr) {
                Node* tmp = front;
                front = front->next.load(RELAXED);
                delete tmp;
            }
        }

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

#endif // FCQUEUE_H