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