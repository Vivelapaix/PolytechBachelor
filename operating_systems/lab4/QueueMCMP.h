#ifndef QUEUEMCMP_H
#define QUEUEMCMP_H

#include <stdlib.h>
#include "Queue.h"

template<typename Elem>
class QueueMCMP : public Queue<Elem>
{
public:
    QueueMCMP();
    ~QueueMCMP();
    
    void  enqueue(const Elem *item);
    Elem* dequeue(void);
    bool  empty(void);

private:
    volatile Node<Elem> *head;
    Node<Elem> *tail;
    
    volatile int lockDequeue;

    volatile int lockEnqueue;
};

template<typename Elem>
QueueMCMP<Elem>::QueueMCMP() {
    head = tail = (Node<Elem> *)malloc(sizeof(Node<Elem>));
    head->isTail = true;
    lockDequeue = 0;
    lockEnqueue = 0;
}

template<typename Elem>
QueueMCMP<Elem>::~QueueMCMP() {
    while (!head->isTail) {
        volatile Node<Elem> *next = head->next;
        delete head;
        head = next;
    }
    delete head;
}

template<typename Elem>
bool QueueMCMP<Elem>::empty() {
    return head->isTail;
}

template<typename Elem>
Elem* QueueMCMP<Elem>::dequeue() {
    Elem *item = ElemNull;
    volatile Node<Elem> *node = NodeNull;
    
    while (__sync_lock_test_and_set(&lockDequeue, 1)) while (lockDequeue) pthread_yield();
    if (!head->isTail) {
        node = __sync_val_compare_and_swap(&head, head, head->next); 
        item = (Elem *)node->item;
    }
    __sync_lock_release(&lockDequeue);
    
    delete node; 
    return item;
}

template<typename Elem>
void QueueMCMP<Elem>::enqueue(const Elem *item) {
    Node<Elem> *node = (Node<Elem> *)malloc(sizeof(Node<Elem>));
    node->isTail = true;
    
    while (__sync_lock_test_and_set(&lockEnqueue, 1)) while (lockEnqueue) pthread_yield();
    tail->next = node;
    tail->item = item;
    tail->isTail = false;
    tail = node;
    __sync_lock_release(&lockEnqueue);
}

#endif /* QUEUEMCMP_H */

