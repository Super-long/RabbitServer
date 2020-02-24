#include "client.h"
#include "../net/socket.h"
#include "../net/epoll_event.h"
#include "../net/epoll_event_result.h"
#include "../base/config.h"
#include "../tool/userbuffer.h"

#include <assert.h>
#include <sys/timerfd.h>

#include <iostream>

namespace{
    int Suit_TimingWheel_oneparameter = 0;
}

namespace ws{

void  //在Delay秒延迟后触发回调
Client::ResetEventfd(int Delay){ //这里绑定的参数有问题
//    TimerWheel_->TW_Add(eventfd_.fd(), std::bind(&Connection::Connect,Connection_.get(),std::placeholders::_1), Delay);
    TimerWheel_->TW_Add(++Suit_TimingWheel_oneparameter, std::bind(&Connection::Connect,Connection_.get(),std::placeholders::_1), Delay);
  
    struct itimerspec newValue;
    memset(&newValue, 0 , sizeof newValue);
    struct itimerspec oldValue;
    memset(&oldValue, 0 , sizeof oldValue);
    struct timespec DelayTime;
    memset(&DelayTime, 0 , sizeof DelayTime);

    DelayTime.tv_sec = static_cast<time_t>(Delay);
    DelayTime.tv_nsec = static_cast<long>(0);
    newValue.it_value = std::move(DelayTime);

    //第二个参数为零表示相对定时器 TFD_TIMER_ABSTIME为绝对定时器
    int ret = ::timerfd_settime(eventfd_.fd(), 0, &newValue, &oldValue);
    if(ret == -1) 
        std::cerr << "Client::ResetEventfd.timerfd_settime failture.\n";
}

void 
Client::SetFd_inSockers(int fd){
    std::unique_ptr<Socket> ptr(new Socket(fd));
    Epoll_->Add(*ptr, EpollCanRead());
    //Sockers_.insert(std::make_pair(fd, std::move(ptr)));
    Sockers_[fd] = std::move(ptr);
}

//服务端未开启而客户端进行非阻塞connect时 epoll中会收到EPOLLRDHUP事件
void 
Client::Remove(int fd){
    if(Sockers_.find(fd) == Sockers_.end()) 
        throw std::logic_error("Client::Remove What happend?");

    Epoll_->Remove(*Sockers_[fd], EpollTypeBase());
    Sockers_.erase(fd);
}

/**
 * TODO:
 * Connect的参数是一个设计上的问题,因为Timerwheel中的参数为int(int)
 * 而且在manger.h中以及使用,用于应用层删除不活跃连接
 * 这里用于应用层的重连 但参数对不上,只好出次下册,
 * 解决方法为把Timewheel写成泛型 最近心力交瘁,先写完主要功能再说吧
*/
void 
Client::Connect(){
    Connection_->Connect(0); 
}

void
Client::Run(){
    Epoll_->Add(eventfd_, EpollCanRead());
    EpollEvent_Result Event_Reault(Y_Dragon::EventResult_Number());

    while(true){
        std::cout << "up epoll_wait\n";
        Epoll_->Epoll_Wait(Event_Reault);
        std::cout << "down epoll_wait\n";        
        for(int i = 0; i < Event_Reault.size(); ++i){
            auto & item = Event_Reault[i];
            int id = item.Return_fd();
            if(id == eventfd_.fd()){
                TimerWheel_->TW_Tick();
                //eventfd_.Read();
                Epoll_->Modify(eventfd_, EpollCanRead());
            }else if(item.check(EETRDHUP)){ //断开连接
                std::cout << "删除\n";                
                Remove(id);
            }else if(item.check(EETCOULDREAD)){ //可读
                std::cout << "可读\n";
                std::shared_ptr<UserBuffer> Buffer_(new UserBuffer(8096));
                Sockers_[id]->Read(Buffer_);
                std::string Content(Buffer_->ReadPtr(), Buffer_->Readable());
                std::cout << Content << std::endl;
            }else if(item.check(EETCOULDWRITE)){
                std::cout << "可写\n";
                Connection_->HandleWrite(id, std::bind(&Client::SetFd_inSockers, this, std::placeholders::_1));
            }
        }
    }
}

}