/**
 * Copyright lizhaolong(https://github.com/Super-long)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <queue>
#include <vector>
#include <thread>
#include <future>
#include <unistd.h>
#include <assert.h>
#include <sys/eventfd.h>

#include "../base/nocopy.h"
#include "../net/epoll.h"
#include "../base/config.h"
#include "../base/havefd.h"
#include "manger.h"

/**
 * 你可能相信你能看懂以下代码，但是其实绝对不可能，相信我。一旦你调试了，你绝对会后悔装聪明去尝试优化这段代码。最好的方式是关闭文件，去玩点儿你喜欢的东西吧。
 * 2020.12.12: 加点注释，给后来的朋友点面子；
*/

namespace ws{

// 其实就是每个工作线程的类
class channel : public Nocopy, public Havefd{
private:
    std::queue<int> ptr_que;
    Manger _Manger_;    // 负责管理每一个线程连接
    Epoll _Epoll_;
    int eventfd;
public:
    explicit channel(int fd) : eventfd(fd), _Epoll_(), _Manger_(_Epoll_) {}

    int fd() const & noexcept override {
        return eventfd;
    }

    std::queue<int>* return_ptr() noexcept {
        return &ptr_que;
    }

    friend void looping(std::promise<std::queue<int>*>& pro, int eventfd);

};

// channel_helper负责分发文件描述符
class channel_helper : public Nocopy{ 
private:
    std::vector<std::thread> pool;
    std::vector<std::future<std::queue<int>*> > vec;
    std::vector<std::queue<int>*> store_;
    std::vector<int> eventfd_;
    int RoundRobin = 0;         // TODO 可以根据每个线程的实际吞吐量去做一个负载均衡，不过这样需要一个线程安全的数据结构去做负载均衡；
    static const uint64_t tool; // no constexper.//https://www.ojit.com/article/112265
    const unsigned int ThreadNumber = std::max<int>(1, std::thread::hardware_concurrency() - 1);    // TODO 需要再看看一般的数据量到底是计算密集型还是IO密集型；
public:
    channel_helper() = default;

    void loop(); 

    void Distribution(int fd);
};

}

#endif //CHANNEL_H_