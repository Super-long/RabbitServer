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

#include "channel.h"
#include "member.h"
#include<sys/types.h>
#include<sys/sysinfo.h>
#include<sched.h>

namespace ws{

/**
 * @notes: 用作工作线程绑核
 * @param: 传入的参数应该小于cpu数，由调用方保证
 * @link：https://man7.org/linux/man-pages/man2/sched_setaffinity.2.html
*/
bool SetCPUaffinity(int param){  //arg  传递线程标号（自己定义）
    cpu_set_t mask;     // CPU核的集合
    int a = param;

    CPU_ZERO(&mask);    // 置空
    CPU_SET(a,&mask);   // 设置亲和力值,把cpu加到集合中 https://man7.org/linux/man-pages/man3/CPU_SET.3.html
    // 第一个参数为零的时候默认为调用线程
    if (sched_setaffinity(0, sizeof(mask), &mask) == -1){   // 设置线程CPU亲和力
        return false;
        // 看起来五种errno没有必要处理；
        // TODO 绑核失败意味着后面可能负载不均衡
    } else {
        return true;
    }
}

void looping(std::promise<std::queue<int>*>& pro, int eventfd, int index){
    channel rea(eventfd); //非异常安全
    pro.set_value(rea.return_ptr());
    rea._Epoll_.Add(rea, EpollCanRead());
    SetCPUaffinity(index);

    while(true){    // 这个线程不能退出，所以搞一个死循环
        try{
            EpollEvent_Result Event_Reault(Y_Dragon::EventResult_Number());
    
            while(true){
                rea._Epoll_.Epoll_Wait(Event_Reault);
                for(int i = 0; i < Event_Reault.size(); ++i){
                    auto& item = Event_Reault[i];
                    int id = item.Return_fd();

                    if(id == eventfd){
                        uint64_t Temp = 0;
                        read(eventfd, &Temp, sizeof(Temp));
                        // TODO 瓶颈导致这里只会每次插入1
                        while(Temp--){
                            assert(!rea.return_ptr()->empty());
                            // 从队列中取到的值就是fd，第二个参数是这个fd加入epoll应该触发的事件是什么，默认注册三个事件，加上读事件；
                            rea._Manger_.Opera_Member(rea.return_ptr()->front(), EpollCanRead());
                            rea.return_ptr()->pop();
                        }
                        rea._Epoll_.Modify(rea, EpollCanRead());
                    } else if (item.check(EETRDHUP)){
                        rea._Manger_.Remove(id);
                    } else if (item.check(EETCOULDREAD)){
                        rea._Manger_.Reading(id);
                        rea._Manger_.JudgeToClose(id);  // 服务端可能出现大量TIME_WAIT状态的套接字导致资源耗尽
                    } else if (item.check(EETCOULDWRITE)){
                        rea._Manger_.Writing(id);
                        rea._Manger_.JudgeToClose(id);
                    }
                    //TODO : Using Time wheel
                }
            }
        }catch(...){
            std::cerr << "Error in : " << std::this_thread::get_id() << std::endl;  //log_fatal
        }
    }
    // 此时线程结束,此时我们应该在分发函数中检查每个线程结构是否是有效的；
}

const uint64_t channel_helper::tool = 1;

// 先把工作线程创建好
void channel_helper::loop(){
    for(unsigned int i = 0; i < RealThreadNumber; i++){
        std::promise<std::queue<int>*> Temp;
        vec.push_back(Temp.get_future());
        int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        pool.push_back(std::thread(looping, std::ref(Temp), fd, i%ThreadNumber));
        pool.back().detach();
        store_.push_back(vec[i].get());
        eventfd_.push_back(fd);
    }
}

// 每到达一个连接就会使用eventfd通信一次
// TODO 瓶颈所在，可以在一个epoll_wait循环结束以后再执行分发；
// TODO 这里还可以根据每个线程的实际吞吐量执行更有效的负载均衡
void channel_helper::Distribution(int fd){
        store_[RoundRobin]->push(fd);
        write(eventfd_[RoundRobin], &channel_helper::tool, sizeof(tool));
        RoundRobin = (RoundRobin + 1) % RealThreadNumber;
}

}