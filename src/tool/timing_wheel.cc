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

#include "timing_wheel.h"

namespace ws{

#define TVR_BITS 8
#define TVR_SIZE (1 << TVR_BITS)
#define TVN_BITS 6
#define TVN_SIZE (1 << TVN_BITS)
#define TVR_MASK (TVR_SIZE - 1)
#define TVN_MASK (TVN_SIZE - 1)

#define FIRST_INDEX(v) ((v) & TVR_MASK)
#define NTH_INDEX(v, n) (((v) >> (TVR_BITS + (n) * TVN_BITS)) & TVN_MASK)

void TimerWheel::TW_Add(int fd, Fun fun, int ticks){
    if(ticks < 0) throw std::invalid_argument("'Timer_Wheel::TW_Add' : error parameter."); 
    int ex = currenttime + ticks;
    _TW_Add_(fd, ex, fun);
}

void TimerWheel::_TW_Add_(int fd, int ex, Fun& para){
    if(mp.find(fd) != mp.end()) throw std::invalid_argument("'TimerWheel::_TW_Add_ error parameter.'");
    uint32_t ex_ = static_cast<uint32_t>(ex);
    uint32_t idx = ex_ - currenttime; //用期望时间减去现在的时间就是要放入下标
    auto ptr = std::make_shared<timernode>(FIRST_INDEX(ex_),fd, ex_, para);
    if(idx < TVR_SIZE){
        ptr->Set_Wheel(0);
        tvroot[FIRST_INDEX(ex_)].emplace_back(std::move(ptr));
        //std::cout <<  fd << " : " << tvroot[FIRST_INDEX(ex_)].size() << std::endl; 
        mp[fd] = --(tvroot[FIRST_INDEX(ex_)].end()); //保存响应的迭代器 用作更新
    } else {
        uint64_t sz;
        for(int i = 0; i < 4; ++i){
            sz = (1ULL << (TVR_BITS + (i+1) * TVN_BITS));
            if(idx < sz){
                idx = NTH_INDEX(ex, i);
                tv[i][idx].emplace_back(std::move(ptr));
                mp[fd] = --(tv[i][idx].end());;
                ptr->Set_Wheel(i + 1);
                break;
            }
        }
    }
}

void TimerWheel::TW_Tick(){
    ++currenttime;
    uint32_t currtick = currenttime;
    int index = (currtick & TVR_MASK);
    if(index == 0){ //从更高级别的轮盘中取出事件
        int i = 0;
        int idx = 0;
        do{
            idx  = NTH_INDEX(currenttime, i);
            for(auto x : tv[i][idx]){
                TW_Add(x->Return_fd(), x->Return_Fun(), static_cast<int>(currenttime - x->Return_expire()));
            }
            tv[i][idx].erase(tv[i][idx].begin(), tv[i][idx].end());
        }while (idx == 0 && ++i < 4);
    }
    std::cout << "tvroot[index].size() : " << tvroot[index].size() << std::endl;
    for(auto x : tvroot[index]){
        int fd = x->Return_fd();
        x->Return_Fun()(fd);
    }
    tvroot[index].erase(tvroot[index].begin(),tvroot[index].end());
}

void TimerWheel::TW_Update(int fd){
    Fun& para = (*mp[fd])->Return_Fun();
    TW_Add(fd,para); 
    int solt = (*mp[fd])->Return_solt();
    int wheel = (*mp[fd])->Return_wheel();
    if(!solt){
        tvroot[wheel].erase(mp[fd]);
    }else{
        tv[solt - 1][wheel].erase(mp[fd]); 
    }
}

#undef TVR_BITS 
#undef TVR_SIZE
#undef TVN_BITS
#undef TVN_SIZE
#undef TVR_MASK
#undef TVN_MASK
#undef FIRST_INDEX
#undef NTH_INDEX

}