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

#ifndef LOADBALANCE_H_
#define LOADBALANCE_H_

#include "../base/nocopy.h"
#include "ThreadSafeQueue/lockfreequeue.h"
#include <thread>

namespace ws {

    struct ThreadLoadData{
        uint32_t Throughput;    // 千兆网卡每秒最大吞吐量也就是125MB左右，况且accept线程每0.1秒更新一次负载状况，uint完全够存；
        short ThreadId;         // 所属线程下标，accept线程分配；
        short ConnectionNumber; // 该线程目前存在的长连接数（长连接很难做负载均衡），个人认为涉及到epoll间转移fd；
    };

    class LoadBalance : public Nocopy {
        public:
            explicit LoadBalance(ws::LockFreeQueue<ThreadLoadData>& que)
             : LoadBalance(que) {
                auto ThreadSum = std::max<int>(1, std::thread::hardware_concurrency() - 1); 
                PerThreadThroughput.resize(ThreadSum);
                PerThreadConnectNumber.resize(ThreadSum);
            }
        private:
            ws::LockFreeQueue<ThreadLoadData>& LoadBalanceQue;
            std::vector<uint32_t> PerThreadThroughput;
            std::vector<uint32_t> PerThreadConnectNumber;
    };
}

#endif