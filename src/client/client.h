#ifndef CLIENT_H_
#define CLIENT_H_

#include "connection.h"
#include "eventfdWrapper.h"
#include "../tool/timing_wheel.h"
#include "channel.h"

#include <unordered_map>
#include <sys/eventfd.h>

namespace ws{

class Client{
private:
std::shared_ptr<TimerWheel> TimerWheel_;
std::shared_ptr<Epoll> Epoll_;
std::unique_ptr<Connection> Connection_;
std::unordered_map<int, std::unique_ptr<Socket>> Sockers_;//存放已有的sockfd
EventFdWrapper eventfd_;
ClientChannel Channel_;

void SetFd_inSockers(int fd);
void ResetEventfd(int Delay);
void Remove(int fd);
void RunAndPop() {Channel_.RunAndPop();}

public:
Client() : Epoll_(new Epoll), //这里因为这是一个特化的客户端,只供连接特定的服务器,所以IP端口固定
        Connection_(new Connection(Epoll_)),
        TimerWheel_(new TimerWheel),
        Channel_(Epoll_),
        eventfd_(){
            Connection_->SetTetryCallBack_(std::bind(&Client::ResetEventfd, this ,std::placeholders::_1));
        }

//在epoll中遇到可写事件的话执行connection.handlewrite,需要一个回调,把fd设置为sockfd_
//并向Connection注册一个recry中使用的回调 void(int)
void Run();

void Connect();

void Push(const std::function<void()>& fun){
    Channel_.push(std::move(fun));
}

int TestInterface(){ //返回已连接服务器的fd 测试接口 测试完后删除
    return Sockers_.begin()->first;
}

};

}

#endif //CLIENT_H_