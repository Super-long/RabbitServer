#include "../../base/nocopy.h"
#include <atomic>
#include <string>
#include <iostream>
#include <list>
#include <stdio.h>
#ifndef LOCKFREEQUEUE_H_
#define LOCKFREEQUEUE_H_

// https://coolshell.cn/articles/8239.html

// ABA问题不会影响代码，因为就算队列尾部在A线程被切换以后出现入队再出队的情况也没关系；

namespace ws{

template <typename T>
class LockFreeQueue : public Nocopy
{
  // 用链表实现LockFreeQueue  Node定义节点的类型
private:
  struct Node 
  {
    T data_;
    Node *next_;
    Node() : data_(), next_(nullptr) {}
    Node(T &data) : data_(data), next_(nullptr) {}
    Node(const T &data) : data_(data), next_(nullptr) {}
  };

private:
  Node *head_, *tail_;
  std::atomic<int> count;

public:
  LockFreeQueue() : head_(new Node()), tail_(head_), count(0) {}
  ~LockFreeQueue() 
  {
    Node *tmp;
    while (head_ != nullptr) 
    {
      tmp = head_;
      head_ = head_->next_;
      delete tmp;
    }
    tmp = nullptr;
    tail_ = nullptr;
  }

  bool pop(T &data) 
  {
    Node *old_head, *old_tail, *first_node;
    while(true) 
    {
      // 取出头指针，尾指针，和第一个元素的指针
      old_head = head_;
      old_tail = tail_; // 前面两步的顺序非常重要，一定要在获取old_tail之前获取old_head
                        // 保证old_head_不落后于old_tail
      first_node = old_head->next_;

      // 下面的工作全是在确保上面三个指针的“一致性”
      // 1. 保证head_的一致性
      /* 
         判断head_指针是否已经移动
         如果head已经被改变我们需要重新再去获取它
      */
      if (old_head != head_) 
      {
        continue;
      }

      // 2. 保证尾指针落后于头指针 (尾指针与全局尾指针部分一致性保持)
      // 空队列或者全局尾指针落后
      // 落后是指其他线程的已经更新，而当前线程没有更新
      if (old_head == old_tail) 
      {
        //这里是队列为空，返回false
        if (first_node == nullptr) 
        {
          return false;
        }
        // 证明全局尾指针没有被更新，尝试更新一下 “主动操作”
        //尾指针落后了，所以我们要更新尾指针，然后在循环一次
        ::__sync_bool_compare_and_swap(&tail_, old_tail, first_node);
        continue;
      } 
      else // 前面的操作都是在确保全局尾指针在全局头指针之后，只有这样才能安全的删除数据
      {
        // 在CAS前先取出数据，防止其他线程dequeue造成数据的缺失
        data = first_node->data_;
        // 在取出数据后，如果head指针没有落后，移动 old_head 指针成功则退出
        if (::__sync_bool_compare_and_swap(&head_, old_head, first_node)) 
        {
          break;
        }
      }
    }
    delete old_head;
    count--;
    return true;
  }

  size_t size() const & noexcept{return count.load();}

  void push(const T &data) 
  {
    Node *enqueue_node = new Node(data);
    Node *old_tail, *old_tail_next;

    while(true) 
    {
      //先取一下尾指针和尾指针的next
      old_tail = tail_;
      old_tail_next = old_tail->next_;

      //如果尾指针已经被移动了，则重新开始
      if (old_tail != tail_) 
      {
        continue;
      }

      // 判断尾指针是否指向最后一个节点
      if (old_tail_next == nullptr) 
      {
        if (::__sync_bool_compare_and_swap(&(old_tail->next_), old_tail_next,
                                           enqueue_node)) 
        {
          break;
        }
      } 
      else 
      {
        // 全局尾指针不是指向最后一个节点，就把全局尾指针向后移动

        // 全局尾指针不是指向最后一个节点，发生在其他线程已经完成节点添加操作，
        // 但是并没有更新最后一个节点，此时，当前线程的(tail_和old_tail是相等的，)
        // 可以更新全局尾指针为old_tail_next，如果其他线程不更新全局尾指针，
        // 那么当前线程会不断移动，直到  old_tail_next == nullptr 为true

        // 为什么这里不直接Continue 原因是:
        // 如果其他线程添加了节点，但是并没有更新
        // 全局尾节点，就会导致所有的线程原地循环等待，所以每一个线程必须要有一些
        // “主动的操作” 去获取尾节点，这种思想在 dequeue的时候也有体现
        ::__sync_bool_compare_and_swap(&(tail_), old_tail, old_tail_next);
        continue;
      }
    }
    // 重置尾节点, 也有可能已经被别的线程重置，那么当前线程就不用管了
    /*
      不用管是因为我们已经将节点成功添加，每一次向队列中加入数据的时候都要保证将tail指针移动到最尾端
    */
    ::__sync_bool_compare_and_swap(&tail_, old_tail, enqueue_node);
    count++;
  }
};
}

#endif