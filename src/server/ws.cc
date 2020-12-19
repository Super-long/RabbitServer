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

#include "../base/config.h"
#include "../tool/ThreadSafeQueue/lockfreequeue.h"
#include "../tool/loadbalance.h"
#include "ws.h"

#include <signal.h>
#include <assert.h> 
#include <sys/time.h>

#include <iostream>

namespace ws{

    extern template class LockFreeQueue<ThreadLoadData>;

    /*
     * @notes: gettimeofday的缺点是精度比较低，但是速度快，因为它不是一个系统调用，且微秒已经可以满足我们的需要,所以没有必要用cpp的接口去写这个；
     * 《[如何精确测量一段代码的执行时间](https://www.0xffffff.org/2015/12/06/37-How-to-benchmark-code-execution-times/)》
     */
    int64_t Get_Current_Time(){
        timeval now;
        int ret = gettimeofday(&now, nullptr);
        assert(ret != 1);
        return now.tv_sec*1000 + now.tv_usec / 1000;
    }

    Web_Server::Web_Server() : _Epoll_(), _Manger_(_Epoll_), _Server_(Y_Dragon::MyPort()){}
 
    void Web_Server::Running(){
        try{
            signal(SIGPIPE, SIG_IGN);

            _Server_.Set_AddrRUseA();
            _Server_.Base_Setting();
            _Server_.Server_BindAndListen();
            _Server_.Server_DeferAccept();

            _Epoll_.Add(_Server_, EpollOnlyRead());
            _Epoll_.Add(_Timer_, EpollOnlyRead());  // 处理连接，又是单线程，只注册一个可读事件即可
            _Timer_.SetTimer();

            LockFreeQueue<ThreadLoadData> que;
            LoadBalance LB(que);                    // 负载均衡器

            EpollEvent_Result Event_Reault(Y_Dragon::EventResult_Number());
            channel_helper Channel_(LB);
            Channel_.loop();

            while(true){
                //constexpr int Second = 20;
                _Epoll_.Epoll_Wait(Event_Reault);
                // https://man7.org/linux/man-pages/man2/epoll_wait.2.html
                // FIX：初始没有考虑epoll_wait返回-1，但是很隐晦，在性能分析的时候需要用到信号，而在接收到信号的时候epoll_wait会被中断返回-1，即errno==EINTR
                if(Event_Reault.size() == -1) continue; // 把这句话删掉跑性能分析就会出现段错误；

                for(int i = 0; i < Event_Reault.size(); ++i){
                    auto& item = Event_Reault[i];
                    int id = item.Return_fd();
                    if(id == _Server_.fd()){ //这里放入事件循环
                        _Server_.Server_Accept([&](int fd){Channel_.Distribution(fd);});
                    } else if(id == _Timer_.fd()) { // 从全局无锁队列中取值进行负载均衡
                        // 每次唤醒后必须阅读timerfd, 如果不这样做，那么将立即再次被唤醒，直到超时为止；被这个点浪费了四十分钟
                        // 事实上不管是默认的间隔超时还是重新使用timerfd_settime都会至少执行一次系统调用。后者还可以使得每次超时时间可变
                        uint64_t exp;
                        ssize_t size = read(id, &exp, sizeof(uint64_t));
                        if(size != sizeof(uint64_t)) {
                            std::cerr << "ERROR : read error. (ws.cc)\n";
                        }
                        LB.ExtractDataDromLockFreeQueue();
                    }
                }
            }
        } catch (std::exception& err){
            std::cout << err.what() << std::endl;
        }
    }
}