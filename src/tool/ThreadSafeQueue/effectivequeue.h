#include "../../base/nocopy.h"
#include <queue>
#include <mutex>
#include <atomic>

#ifndef EFFECTIVEQUEUE_H_
#define EFFECTIVEQUEUE_H_

// https://downdemo.gitbook.io/cpp-concurrency-in-action-2ed/5.-ji-yu-suo-de-bing-fa-shu-ju-jie-gou-de-she-ji-designing-lockbased-concurrent-data-structure/shi-yong-xi-li-du-finegrained-suo-he-tiao-jian-bian-liang-shi-xian-threadsafe-queue

namespace ws{
    
    template<typename Type>
    class EffectiveQueue : public Nocopy{
        struct node {
            std::shared_ptr<Type> val;
            std::shared_ptr<node> next;
        };
        std::shared_ptr<node> head;
        node* tail;
        std::mutex hm; // head mutex
        std::mutex tm; // tail mutex
        std::atomic<int> count;

        node* get_tail() {
            std::lock_guard<std::mutex> l(tm);
            return tail;
        }

        std::shared_ptr<node> pop_head(){
            std::lock_guard<std::mutex> l(hm);
            if (head.get() == get_tail()) return nullptr;
            std::shared_ptr<node> oldHead = std::move(head);
            head = std::move(oldHead->next);
            return oldHead;
        }
        
    public:
        EffectiveQueue() : head(new node), tail(head.get()) {}
        // push有两种可能产生异常的情况：一是给mutex上锁，但数据在上锁成功后才会修改
        // 二是构建智能指针对象时可能抛出异常，但智能指针本身是异常安全的，异常时会释放
        void push(Type x){ // 因此push是异常安全的
            std::shared_ptr<Type> newVal(std::make_shared<Type>(std::move(x)));
            std::shared_ptr<node> p(new node);
            node* const newTail = p.get();
            {
                std::lock_guard<std::mutex> l(tm); // 此处加锁
                tail->val = newVal;
                tail->next = std::move(p);
                tail = newTail;
            }
            count++;
        }
        // 同理try_pop也是异常安全的
        std::shared_ptr<Type> pop(){
            std::shared_ptr<node> oldHead = pop_head();
            if(oldHead){
                count--;
                return oldHead->val;
            } else {
                return std::shared_ptr<Type>();
            }
        }

        decltype(auto) size() const & noexcept {
            return count.load();
        }
    };
}

#endif