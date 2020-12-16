#include "../../base/nocopy.h"
#include <queue>
#include <mutex>
#include <memory>

#ifndef BASEQUEUE_H_
#define BASEQUEUE_H_

namespace ws{

    // 所有线程安全队列的实现必须实现push,pop,size接口，但不希望引入一个虚函数，增加类的负担；
    template<typename Type>
    class BaseQueue : public Nocopy{
        public:

        // 当队列中不存在值的时候返回的智能指针为空；
        // 这个函数其实写的不好，算是性能换取语义的明确，因为智能指针是可以避免的；
        std::shared_ptr<Type> pop(){   // http://bitdewy.github.io/blog/2014/01/12/why-make-shared/
            std::lock_guard<std::mutex> guard(mu);
            if(que.empty()) return std::shared_ptr<Type>();
            std::shared_ptr<Type> res = que.front();
            que.pop();
            return res;
        }

        decltype(auto) size() const & {
            std::lock_guard<std::mutex> guard(mu);
            return que.size();
        }

        void push(Type item){
            std::shared_ptr<Type> data(std::make_shared<Type>(item));
            std::lock_guard<std::mutex> guard(mu);
            que.emplace(data);
        }

        private:
            std::queue<std::shared_ptr<Type>> que;
            mutable std::mutex mu;
    };

}

#endif