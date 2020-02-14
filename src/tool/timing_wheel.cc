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

void TimerWheel::TW_Add(int fd, Fun para, int ticks){
    if(ticks < 0) throw std::invalid_argument("'Timer_Wheel::TW_Add' : error parameter."); 
    int ex = currenttime + ticks;
    _TW_Add_(fd, ex, para);
}

void TimerWheel::_TW_Add_(int fd, int ex, Fun& para){
    if(mp.find(fd) != mp.end()) throw std::invalid_argument("'TimerWheel::_TW_Add_ error parameter.'");
    uint32_t ex_ = static_cast<uint32_t>(ex);
    uint32_t idx = ex_ - currenttime;
    auto ptr = std::make_shared<timernode>(FIRST_INDEX(ex_),fd, ex_, para);
    if(idx < TVR_SIZE){
        ptr->Set_Wheel(0);
        tvroot[FIRST_INDEX(ex_)].emplace_back(std::move(ptr));
        std::cout <<  fd << " : " << tvroot[FIRST_INDEX(ex_)].size() << std::endl; 
        mp[fd] = --(tvroot[FIRST_INDEX(ex_)].end());
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
    if(index == 0){
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