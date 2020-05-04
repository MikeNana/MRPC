#ifndef GTEST_SAMPLE3_H
#define GTEST_SAMPLE3_H

#include <stddef.h>
//a simple queue implemented as a single-linked list for practice test fixture
template<typename E> 
class Queue;

template<typename E>
class QueueNode
{
friend class Queue<E>;
private:
    E element_;
    QueueNode* next_;
    //called by Queue without implicit way
    explicit QueueNode(const E& an_element):element_(an_element), next_(nullptr){}
    //disable the default assignment operator and c'tor
    const QueueNode& operator = (const QueueNode&);
    QueueNode (const QueueNode&);
public: 
    //get the element in the node
    const E& element() const { return element_;}
    //get the next node in the queue
    QueueNode* next() {return next_;}
    const QueueNode* next() const {return next_;}
};


template<typename E>
class Queue
{
private:
    //we disallow copying a queue
    Queue(const Queue&);
    Queue& operator = (const Queue&);
    
    QueueNode<E>* first_;   //the first node in the queue
    QueueNode<E>* last_;    //the last node in the queue
    size_t size_;           //the number of elements of in the queue
public:
    //create a empty queue
    Queue():first_(nullptr), last_(nullptr), size_(0){}
    //D'tor
    ~Queue(){Clear();}
    //clear the queue
    void Clear()
    {
        //1.delete every node in the queue
        if(size_ > 0)
        {
            QueueNode<E>* node_ = first_;
            QueueNode<E>* next = node_->next();
            for(;;)
            {
                delete node_;
                node_ = next;
                if(node_ == nullptr)
                    break;
                next = node_->next();
            }
        }
        //2.reset the member variables
        first_ = last_ = nullptr;
        size_ = 0;
    }
    size_t Size() const { return size_; }
    //return the first node in the queue
    QueueNode<E>* Head(){ return first_; }
    const QueueNode<E>* Head() const { return first_; }
    //return the last node in the queue
    QueueNode<E>* Tail(){ return last_; }
    const QueueNode<E>* Tail() const { return last_; }
    //Add a new node to the end of the queue
    void Enqueue(const E& element)
    {
        auto node = new QueueNode<E>(element);

        if(size_ == 0)
        {
            first_ = last_ = node;
            size_ = 1;
        }
        else
        {
            last_->next_ = node;
            last_ = node;
            ++size_;
        }
        return;
    }
    E* Dequeue()
    {
        if(size_ == 0)
        {
            return nullptr;
        }
        const QueueNode<E>* old_head = first_;
        first_ = first_->next_;
        --size_;
        if(size_ == 0)
        {
            last_ = nullptr;
        }
        E* element = new E(old_head->element());
        delete old_head;
        return element;
    }
    //applies a function on each element of the queue, and returns the result in a new queue
    template<typename F> 
    Queue* Map(F function) const
    {
        Queue* new_queue = new Queue();
        for(auto a = first_; a != nullptr; a = a->next_)
        {
            new_queue->Enqueue(function(a->element()));
        }
        return new_queue;
    }
};


#endif GTEST_SAMPLE3_H