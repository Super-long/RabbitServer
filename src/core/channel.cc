#include "channel.h"
#include "member.h"

namespace ws{

void looping(std::promise<std::queue<int>*>& pro, int eventfd){
    try{
        channel rea(eventfd); //非异常安全
        pro.set_value(rea.return_ptr());
        rea._Epoll_.Add(rea, EpollCanRead());
        EpollEvent_Result Event_Reault(Y_Dragon::EventResult_Number());
 
        while(true){
            rea._Epoll_.Epoll_Wait(Event_Reault);
            for(int i = 0; i < Event_Reault.size(); ++i){
                auto & item = Event_Reault[i];
                int id = item.Return_fd();

                if(id == eventfd){
                    uint64_t Temp = 0;
                    read(eventfd, &Temp, sizeof(Temp));
                    while(Temp--){
                        assert(!rea.return_ptr()->empty());
                        rea._Manger_.Opera_Member(std::make_unique<Member>(rea.return_ptr()->front()),EpollCanRead());
                        rea.return_ptr()->pop();
                    }
                    rea._Epoll_.Modify(rea, EpollCanRead());
                }else if(item.check(EETRDHUP)){
                    rea._Manger_.Remove(id);
                }else if(item.check(EETCOULDREAD)){
                    rea._Manger_.Reading(id);
                    rea._Manger_.JudgeToClose(id); //修改
                }else if(item.check(EETCOULDWRITE)){
                    rea._Manger_.Writing(id);
                    rea._Manger_.JudgeToClose(id);
                }
                //TODO : Using Time wheel
            }
        }
    }catch(...){
        std::cerr << "error in : " << std::this_thread::get_id() << std::endl;//log_fatal
    }
}

const uint64_t channel_helper::tool = 1;

void
channel_helper::loop(){
    for(unsigned int i = 0; i < ThreadNumber; i++){
        std::promise<std::queue<int>*> Temp;
        vec.push_back(Temp.get_future());
        int fd = eventfd(0,EFD_CLOEXEC | EFD_NONBLOCK);
        pool.push_back(std::thread(looping, std::ref(Temp), fd));
        store_.push_back(vec[i].get());
        eventfd_.push_back(fd);
    }
}

void channel_helper::Distribution(int fd){
        store_[RoundRobin]->push(fd);
        write(eventfd_[RoundRobin], &channel_helper::tool, sizeof(tool));
        RoundRobin = (RoundRobin+1)%ThreadNumber;
}

}