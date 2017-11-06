#ifndef QUEEUMCSP_H
#define QUEEUMCSP_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "Queue.h"

template<typename Elem>
class QueueMCSP : public Queue<Elem>
{
public:
    QueueMCSP();
    ~QueueMCSP();
    
    void  enqueue(const Elem *item);
    Elem* dequeue(void);
    bool  empty(void);

private:
    volatile Node<Elem> *head;
    Node<Elem> *tail;
    
    volatile long long currentTicket;
    volatile long long maxTicket;
    volatile int lock;
};

template<typename Elem>
QueueMCSP<Elem>::QueueMCSP() {
    head = tail = (Node<Elem> *)malloc(sizeof(Node<Elem>));
    head->isTail = true;
    currentTicket = maxTicket = 0;
    lock = 0;
}

template<typename Elem>
QueueMCSP<Elem>::~QueueMCSP() {
    while (!head->isTail) {
        volatile Node<Elem> *next = head->next;
        delete head;
        head = next;
    }
    delete head;
}

template<typename Elem>
bool QueueMCSP<Elem>::empty() {
    return head->isTail;
}

template<typename Elem>
Elem* QueueMCSP<Elem>::dequeue() {
    Elem *item = ElemNull;
    volatile Node<Elem> *node = NodeNull;
    
    long long ticket = __sync_fetch_and_add(&maxTicket, 1);
    while (currentTicket < ticket);
    while (__sync_lock_test_and_set(&lock, 1)) while (lock) pthread_yield();
    if (!head->isTail) {
        node = __sync_val_compare_and_swap(&head, head, head->next); 
        item = (Elem *)node->item;
    }
    __sync_lock_release(&lock);
    currentTicket++;
    
    delete node; 
    return item;
}

template<typename Elem>
void QueueMCSP<Elem>::enqueue(const Elem *item) {
    Node<Elem> *node = (Node<Elem> *)malloc(sizeof(Node<Elem>));
    node->isTail = true;
    
    tail->next = node;
    tail->item = item;
    tail->isTail = false;
    tail = node;
}

#endif /* QUEEUMCSP_H */