#ifndef QUEUE_H
#define QUEUE_H

#define ElemNull (Elem *)0
#define NodeNull (Node<Elem> *)0

template<typename Elem>
struct Node
{
    volatile const Elem *item;
    volatile Node<Elem> *next;
    bool isTail;
};

template<typename Elem>
class Queue
{
public:
    virtual ~Queue() {}
    
    virtual void  enqueue(const Elem *item) = 0;
    virtual Elem* dequeue(void) = 0;
    virtual bool  empty(void) = 0;
};

#endif /* QUEUE_H */
