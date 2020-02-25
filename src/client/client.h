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

#ifndef CLIENT_H_
#define CLIENT_H_

#include "connection.h"
#include "eventfdWrapper.h"
#include "../tool/timing_wheel.h"
#include "channel.h"

#include <thread>
#include <unordered_map> 
#include <sys/eventfd.h>

namespace ws{

/**
 * 预计给外界留的接口为传入传出一个字符串
*/
class Client{
private:
std::shared_ptr<TimerWheel> TimerWheel_;
std::shared_ptr<Epoll> Epoll_;
std::unique_ptr<Connection> Connection_;
std::unordered_map<int, std::unique_ptr<Socket>> Sockers_;//存放已有的sockfd
EventFdWrapper eventfd_;
ClientChannel Channel_;
std::thread Eventloop_;

void SetFd_inSockers(int fd);
void ResetEventfd(int Delay);
void Remove(int fd);
void RunAndPop() {Channel_.RunAndPop();}

//在epoll中遇到可写事件的话执行connection.handlewrite,需要一个回调,把fd设置为sockfd_
//并向Connection注册一个recry中使用的回调 void(int)
void Run();

public:
Client() : Epoll_(new Epoll), //这里因为这是一个特化的客户端,只供连接特定的服务器,所以IP端口固定
        Connection_(new Connection(Epoll_)),
        TimerWheel_(new TimerWheel),
        Channel_(Epoll_),
        eventfd_(){
            Connection_->SetTetryCallBack_(std::bind(&Client::ResetEventfd, this ,std::placeholders::_1));
        }

void Connect();

void Push(const std::function<void()>& fun){
    Channel_.push(std::move(fun));
}

void Start(){
    Eventloop_ = std::thread(&Client::Run, this);
}

~Client(){
    Eventloop_.join();
}

//start 和 connect之后调用
int SendToServer(const std::string& Content){ //string_view
std::cout << "length : " << Content.length() << std::endl;
    Push(std::bind(::write, Sockers_.begin()->first, Content.c_str(), Content.length()));
/*     Push([&]{
        int ret = ::write(Sockers_.begin()->first, Content.c_str(), Content.length());
    }); */
}

int test(){
    return Sockers_.begin()->first;
}

};

}

#endif //CLIENT_H_