#ifndef EVENTFDWRAPPER_H_
#define EVENTFDWRAPPER_H_

#include "../base/havefd.h"

#include <sys/timerfd.h>
#include <sys/eventfd.h>
#include <iostream>
#include <unistd.h>

namespace ws{

class EventFdWrapper : public Havefd{
private:
int Eventfd_;
public:
    EventFdWrapper() : Eventfd_(::timerfd_create(CLOCK_MONOTONIC,
                                 TFD_NONBLOCK | TFD_CLOEXEC)){} //Used as timerfd.

    explicit EventFdWrapper(int fd) : Eventfd_(fd){} //Used ad evnetfd.

    int fd() const override {return Eventfd_;}

    uint64_t Read(){
        uint64_t howmany;
        ssize_t n = ::read(Eventfd_, &howmany, sizeof howmany);
        if (n != sizeof howmany){
            std::cerr << "error in EventFdWrapper Read.\n";
        }
        return howmany;
    }

    uint64_t Write(uint64_t para){
        ::write(Eventfd_, &para , sizeof para);
    }

    ~EventFdWrapper(){
        ::close(Eventfd_);
    }
};

}

#endif //EVENTFDWRAPPER_H_