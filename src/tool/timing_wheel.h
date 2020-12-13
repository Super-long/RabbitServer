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

#ifndef TIMEING_WHEEL_H_
#define TIMEING_WHEEL_H_

#include <list>
#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <functional>

namespace ws{

    class TimerWheel{
        using Fun = std::function<int(int)>;
        class timernode{
            public:
                explicit timernode(uint16_t solt , int fd_, uint32_t ex, Fun& para) :
                    time_solt(solt), fd(fd_), expire(ex), f(para){}
                int Return_fd() const noexcept {return fd;}
                uint32_t Return_expire() const noexcept {return expire;}  
                int Return_solt() const noexcept {return static_cast<int>(time_solt);}
                int Return_wheel() const noexcept {return static_cast<int>(time_wheel);}
                Fun& Return_Fun() noexcept {return f;} 
                void Set_Wheel(uint8_t sw) noexcept {time_wheel = sw;}
            private:
                uint16_t time_solt;
                uint8_t time_wheel;
                int fd;
                Fun f;
                uint32_t expire;
        };
        using itr = std::list<std::shared_ptr<timernode>>::iterator;
        using TVN_ = std::list<std::shared_ptr<timernode>>[256];
        using TVR_ = std::list<std::shared_ptr<timernode>>[64];
        public:
            //using Fun = std::function<int(int)>;

            TimerWheel() : currenttime(0){} 

            void TW_Add(int fd, Fun fun, int ticks = 2);
            void TW_Tick();
            void TW_Update(int fd); 

        private:
            std::unordered_map<int, itr> mp;
            void _TW_Add_(int fd, int ex, Fun&); 
            uint32_t currenttime;
            TVN_ tvroot;
            TVR_ tv[4];
    };

}

#endif 