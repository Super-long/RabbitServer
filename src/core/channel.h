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
#include "../base/Y_Dragon.h"
#include "../base/havefd.h"
#include "manger.h"

namespace ws{

class channel : public Nocopy, public Havefd{
private:
    std::queue<int> ptr_que;
    Manger _Manger_;
    Epoll _Epoll_;
    int eventfd;
public:
    explicit channel(int fd) : eventfd(fd), _Epoll_(), _Manger_(_Epoll_){}

    int fd() const override {
        return eventfd;
    }

    std::queue<int>* return_ptr(){
        return &ptr_que;
    }

    friend void looping(std::promise<std::queue<int>*>& pro, int eventfd);
};

class channel_helper : public Nocopy{
private:
    std::vector<std::thread> pool;
    std::vector<std::future<std::queue<int>*> > vec;
    std::vector<std::queue<int>*> store_;
    std::vector<int> eventfd_;
    int RoundRobin = 0;
    static const uint64_t tool; //no constexper.//https://www.ojit.com/article/112265
    const unsigned int ThreadNumber = std::thread::hardware_concurrency() - 1;
public:
    channel_helper() = default;

    void loop(); 
 
    void Distribution(int fd);
};

}

#endif //CHANNEL_H_