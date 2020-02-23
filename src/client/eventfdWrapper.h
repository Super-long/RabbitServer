#ifndef EVENTFDWRAPPER_H_
#define EVENTFDWRAPPER_H_

#include "../base/havefd.h"

#include <sys/eventfd.h>
#include <iostream>
#include <unistd.h>

namespace ws{

class EventFdWrapper : public Havefd{
private:
int Eventfd_;
public:
    EventFdWrapper() : Eventfd_(::eventfd(0,EFD_CLOEXEC | EFD_NONBLOCK)){}

    int fd() const override {return Eventfd_;}

    void Read(){
        uint64_t howmany;
        ssize_t n = ::read(Eventfd_, &howmany, sizeof howmany);
        if (n != sizeof howmany){
            std::cerr << "error in EventFdWrapper Read.\n";
        }
    }
};

}

#endif //EVENTFDWRAPPER_H_