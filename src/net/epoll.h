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

#ifndef WEBSERVER_EPOLL
#define WEBSERVER_EPOLL

#include "epoll_event_result.h"
#include "../base/nocopy.h"
#include "../base/havefd.h"

#include <sys/epoll.h>

#include <iostream>

namespace ws{
    // https://blog.csdn.net/qingzhuyuxian/article/details/108358074?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522160760737119724847186036%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=160760737119724847186036&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_v2~rank_v29-6-108358074.first_rank_v2_pc_rank_v29&utm_term=epoll_wait%E5%8F%82%E6%95%B0&spm=1018.2118.3001.4449
    class Epoll final : public Nocopy, public Havefd{
        public:
            Epoll() : epoll_fd_(epoll_create1(::EPOLL_CLOEXEC)) {}
            // 其实下面用右值有点蠢，因为确实没什么用；
            
            int Add(EpollEvent& para){
                return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD,para.Return_fd(),para.Return_Pointer());
            }
            int Add(EpollEvent&& para){
                return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD,para.Return_fd(),para.Return_Pointer());
            }
            int Add(const Havefd& Hf,EpollEventType ETT){
                return Add({Hf,ETT}); 
            }

            int Modify(EpollEvent& para){
                return epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, para.Return_fd(), para.Return_Pointer());
            }
            int Modify(EpollEvent&& para){
                return epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, para.Return_fd(), para.Return_Pointer());
            } 
            int Modify(const Havefd& Hf,EpollEventType ETT){
                return Modify({Hf,ETT});
            }
            
            int Remove(EpollEvent& para){
                //std::cout << "已断开一个连接 : " << epoll_fd_ << std::endl;
                return epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, para.Return_fd(), para.Return_Pointer());
            }
            int Remove(EpollEvent&& para){
                //std::cout << "已断开一个连接 : " << epoll_fd_ << std::endl;
                return epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, para.Return_fd(), para.Return_Pointer());
            }
            int Remove(const Havefd& Hf, EpollEventType ETT){
                return Remove({Hf, ETT}); 
            } 

            void Epoll_Wait(EpollEvent_Result& ETT){
                Epoll_Wait(ETT, -1);
            }

            void Epoll_Wait(EpollEvent_Result& ETT, int timeout){    // -1为阻塞；0为非阻塞
                int Available_Event_Number_ =
                    epoll_wait(epoll_fd_,reinterpret_cast<epoll_event*>(ETT.array.get()), ETT.All_length,timeout);
                ETT.Available_length = Available_Event_Number_;
                // 可能返回-1，但是没关系，在ws.cc中处理就可以了；
            }

            int fd() const & noexcept override {return epoll_fd_; }

        private: 
            int epoll_fd_;
    };
}

#endif