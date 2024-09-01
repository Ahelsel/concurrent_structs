#ifndef MSQUEUE_H
#define MSQUEUE_H

#include <atomic>
#include <optional>

#define RELAXED std::memory_order_relaxed
#define RELEASE std::memory_order_release
#define ACQUIRE std::memory_order_acquire
#define ACQ_REL std::memory_order_acq_rel
#define SEQ_CST std::memory_order_seq_cst

template <typename T>
class MSQueue {
    private:
        struct Node {
            T val;
            std::atomic<Node*> next;
            Node(T value) : val(value), next(nullptr) {}
        };

        std::atomic<Node*> head;
        std::atomic<Node*> tail;

        void delete_nodes(Node* front) {
            while (front) {
                Node* tmp = front;
                front = front->next.load(RELAXED);
                delete tmp;
            }
        }

    public:
        MSQueue() {
            Node* dummy = new Node(T());
            head.store(dummy, RELAXED);
            tail.store(dummy, RELAXED);
        }

        ~MSQueue() {
            Node* front = head.load(RELAXED);
            delete_nodes(front);
        }

        void push(T val) {
            Node *t, *e, *n;
            n = new Node(val);
            while (true) {
                t = tail.load(); e = t->next.load();
                if (t == tail.load()) {
                    Node *null_node = nullptr;
                    if (e==NULL && t->next.compare_exchange_strong(null_node, n)) 
                        break;
                    else if (e != NULL) 
                        tail.compare_exchange_strong(t, e);
                }
            }
            tail.compare_exchange_strong(t, n);
        }

        std::optional<T> pop() {
            Node *t, *h, *n;
            while (true) {
                h=head.load(); t=tail.load(); n=h->next.load();
                if (h==head.load()) {
                    if (h==t) {
                        if (n==NULL) 
                            return std::nullopt;
                        else 
                            tail.compare_exchange_strong(t, n);
                    }
                    else {
                        int ret=n->val;
                        if (head.compare_exchange_strong(h, n)) 
                            return ret;
                    }
                }
            }
        }
};

#endif // MSQUEUE_H