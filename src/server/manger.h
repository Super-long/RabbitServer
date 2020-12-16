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

#ifndef MANGER_H_
#define MANGER_H_

#include "../base/copyable.h"
#include "../net/socket.h"
#include "../net/epoll.h"
#include "member.h"
#include "../net/epoll_event.h"
#include "../tool/timing_wheel.h"

#include <unordered_map>
#include <memory>

namespace ws{
    class Manger : public Copyable{
        using Fun = std::function<int(int)>; 
        public:
            explicit Manger(Epoll& _epoll) : 
            _Epoll_(_epoll), Timer_Wheel_(std::make_unique<TimerWheel>()){}

            int Opera_Member(std::unique_ptr<Member>&, EpollEventType&);
            int Opera_Member(int, EpollEventType&&);
            int Opera_Member(std::unique_ptr<Member>&, EpollEventType&&);
            int Opera_Member(std::unique_ptr<Member>&&, EpollEventType&);
            
            // 在使用的时候是信号驱动，当然可以使用signalfd，把信号驱动修改为事件驱动；
            void InsertTimeWheel(int fd);

            void Reading(int fd, long time = -1);
            void Writing(int fd, long time = -1);
            int JudgeToClose(int fd);
            int Update(int fd);
            int UpdateWrite(int fd);

            int Remove(int fd);
            bool Exist(int fd) const & {if(Fd_To_Member.find(fd) != Fd_To_Member.end()) return true; return false;}

        private:
            std::unique_ptr<TimerWheel> Timer_Wheel_;
            Epoll& _Epoll_;
            std::unordered_map<int, std::unique_ptr<Member>> Fd_To_Member;  // 相当于slab
    };
}

#endif