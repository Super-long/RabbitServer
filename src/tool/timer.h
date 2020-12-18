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

#ifndef TIMER_H_
#define TIMER_H_

#include "../base/havefd.h"
#include "../base/nocopy.h"
#include <sys/timerfd.h>
#include <unistd.h>

namespace ws{
    class Timer : public Havefd, public Nocopy{
        public:
            Timer() : timeFd(::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK)){}
            int fd() const & noexcept override {return timeFd;}

            // 间隔时间与首次超时时间，单位为纳秒; 默认为200ms以后启动，100ms间隔触发一次；
            int SetTimer(long intervalNanoseconds = 100000000l, long firstNanoseconds = 200000000l){
                struct itimerspec its;

                constexpr long base = 1000000000;

                its.it_interval.tv_sec = intervalNanoseconds / base;     // 后面触发的间隔时间
                its.it_interval.tv_nsec = intervalNanoseconds % base;

                its.it_value.tv_sec = firstNanoseconds / base;// 首次超时时间
                its.it_value.tv_nsec = firstNanoseconds % base;
                // https://linux.die.net/man/2/timerfd_settime
                if (::timerfd_settime(timeFd, 0, &its, nullptr) < 0)
                    return -1;
                return 0;   // timerfd_settime成功时返回0，等于-1的时候失败；
            }

            ~Timer(){
                ::close(timeFd);
            }
        private:
            int timeFd;

    };
}

#endif