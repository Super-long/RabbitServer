#include "eventloop.h"
#include <iostream>

namespace ws{

thread_local EventLoop* EventLoopInThisThread = nullptr;

EventLoop::EventLoop() 
    : threadID(std::this_thread::get_id()), looping(false){
    if(EventLoopInThisThread){
        std::cout << "errno in eventloop.cc : \n";
        abort();
    } else {
        EventLoopInThisThread = this;
    }
}

EventLoop::~EventLoop(){
    assert(!looping);
    EventLoopInThisThread = nullptr;
}

void
EventLoop::loop(){
    assert(!looping);
    looping = true;

    //do something.

    looping = false;
}

}