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
#include <vector>

#ifndef __GNUC__

#define  __attribute__(x)  /*NOTHING*/
    
#endif

namespace ws {

/*     struct ThreadLoadData
    template class LockFreeQueue<ThreadLoadData>; */

    struct ThreadLoadData{
        uint32_t Throughput;        // 千兆网卡每秒最大吞吐量也就是125MB左右，况且accept线程每0.1秒更新一次负载状况，uint完全够存；ps乘以12也不会越界，但是更高核的机器就有可能越界了
        uint16_t ThreadId;          // 所属线程下标，accept线程分配；
        uint16_t ConnectionNumber;  // 该线程目前存在的长连接数（长连接很难做负载均衡），个人认为涉及到epoll间转移fd，目前有了一种五次握手的想法

        ThreadLoadData(uint32_t thoughtout, uint16_t id, uint16_t connection = 0)
            : Throughput(thoughtout), ThreadId(id), ConnectionNumber(connection) {}

        ThreadLoadData(const ThreadLoadData& item)
            : Throughput(item.Throughput), ThreadId(item.ThreadId), ConnectionNumber(item.ConnectionNumber) {}
        
        ThreadLoadData(){}  // 无锁队列不支持移动，后面有时间尝试修改下无锁队列的实现
    };

    template class LockFreeQueue<ThreadLoadData>;   // 模板定义，为了多个文件只生成一份实例，节省空间; 放在文件头的话会因为生成代码时没有ThreadLoadData的定义而编译失败

    class LoadBalance : public Nocopy {
        public:
            explicit LoadBalance(ws::LockFreeQueue<ThreadLoadData>& que)
             : LoadBalanceQue(que), KeepData(false) {
                auto ThreadSum = std::max<int>(1, std::thread::hardware_concurrency() - 1); 
                PerThreadThroughput.resize(ThreadSum);
                CurrentWeight.resize(ThreadSum, 0);

                PerThreadConnectNumber.resize(ThreadSum);
            }

            bool __attribute__((pure)) GetFlag() const noexcept { return KeepData;}
            void SetFlag(bool flag) noexcept { KeepData = flag;}

            size_t Distribution();
            size_t ExtractDataDromLockFreeQueue();

            LockFreeQueue<ThreadLoadData>& ReturnQue() const & noexcept{
                return LoadBalanceQue;
            }

        private:
            void Resize(ThreadLoadData& item);
            LockFreeQueue<ThreadLoadData>& LoadBalanceQue;

            // 基于吞吐量的负载均衡算法参考Nginx的负载均衡算法，需要下面三个值；
            std::vector<uint32_t> PerThreadThroughput;
            std::vector<long> CurrentWeight;    // 因为计算时会出现负数
            long TotalWeight;


            std::vector<uint16_t> PerThreadConnectNumber;
            bool KeepData;  // 是否保留前面计算的数据
    };
}

#endif