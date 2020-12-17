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

#include "loadbalance.h"
#include <algorithm>
#include <limits.h>
#include <numeric>
#include <iostream>

namespace ws{

    extern template class LockFreeQueue<ThreadLoadData>;

    /**
     * @notes: 基于吞吐量和长连接数判断下次应该把套接字分配给哪一个线程，显然这样做到了机制与策略相分离，我们可以修改Distribution而做到上层无感知；
     * @notes: PerThreadConnectNumber优先级大于PerThreadThroughput；就loadbalaancetest.cc中来看算法基本保持稳定；
    */
    size_t LoadBalance::Distribution(){
        // step1: 基于线程长连接数进行判断
        // nullptr

        // step2: 基于吞吐量进行判断
        for (size_t i = 0; i < CurrentWeight.size(); i++){
            CurrentWeight[i] += PerThreadThroughput[i];
        }
        size_t index = std::max_element(CurrentWeight.begin(), CurrentWeight.end()) - CurrentWeight.begin();
        CurrentWeight[index] -= TotalWeight;

        return index;
    }

    /**
     * @notes: 从无锁队列中把数据取出来放到容器中;
    */
    size_t LoadBalance::ExtractDataDromLockFreeQueue(){
        if(!KeepData){
            std::fill(PerThreadThroughput.begin(), PerThreadThroughput.end(), 0);
            std::fill(PerThreadConnectNumber.begin(), PerThreadConnectNumber.end(), 0);
        }

        auto len = LoadBalanceQue.size();   // 队列中至少len个项可读，如果多个线程读就要考虑取出的值为空了，单线程直接取就好；
        for (size_t i = 0; i < len; i++){
            ThreadLoadData item;
            LoadBalanceQue.pop(item);   // 结构体开销比较小，直接拷贝即可
            if(!KeepData){
                PerThreadConnectNumber[item.ThreadId] += item.ConnectionNumber; // 暂时不用
                PerThreadThroughput[item.ThreadId] += item.Throughput;
            } else {    // 数据保持的话要考虑数字越界
                Resize(item);
            }
        }

        // 用于基于吞吐量的负载均衡
        std::fill(CurrentWeight.begin(), CurrentWeight.end(), 0);

        TotalWeight = std::accumulate(PerThreadThroughput.begin(), PerThreadThroughput.end(), 0);   // 记录下来，少计算几次，32位先64位转，安全

        // 用于把吞吐量和线程的映射改变，因为本来这种负载均衡算法的初始值是基于机器的异构性的，我们需要反过来；
        std::vector<std::pair<uint32_t, size_t>> temp;
        for (size_t i = 0; i < PerThreadThroughput.size(); i++){
            temp.emplace_back(std::make_pair(PerThreadThroughput[i], i));
        }

        sort(temp.begin(), temp.end());

        for (size_t i = 0; i < PerThreadThroughput.size()/2; i++){
            std::swap(PerThreadThroughput[temp[i].second], PerThreadThroughput[temp[PerThreadThroughput.size() - i - 1].second]);
        }

/*         for (size_t i = 0; i < PerThreadThroughput.size(); i++){
            std::cout << i << " : " << PerThreadThroughput[i] << std::endl;
        } */

        return len; // 返回无锁队列长度，暂时没什么用处
    }

    /**
     * @notes: 用于在KeepData为true的时候两个容器内数据越界；
    */
    void LoadBalance::Resize(ThreadLoadData& item){ // 如果有一个特别大的数字传入就gg了，但是实际因为网卡的限制不会有那么大的数据
        if(USHRT_MAX - item.ConnectionNumber < PerThreadConnectNumber[item.ThreadId]){
            std::for_each(PerThreadConnectNumber.begin(), PerThreadConnectNumber.end(), [](uint16_t& data){ data /= 2;});
        }
        PerThreadConnectNumber[item.ThreadId] += item.ConnectionNumber; 

        if(UINT_MAX - item.Throughput < PerThreadThroughput[item.ThreadId]){
            std::for_each(PerThreadThroughput.begin(), PerThreadThroughput.end(), [](uint32_t& data){ data /= 2;});
        }

        PerThreadThroughput[item.ThreadId] += item.Throughput;
    }

}