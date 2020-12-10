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

#include "connection.h"
#include "../net/epoll_event.h"
#include "../net/epoll_event_result.h"

#include <assert.h>
#include <errno.h>
#include <iostream>

namespace ws{ 

const int 
Connection::kMaxRetryDelayMs = 48;

const int
Connection::KInitRetryDelayMs = 1;

int
Connection::Connect(int padding){ //这里的参数仅仅是为了填坑
    // 每次需要重新创建一个套接字，因为可能出现自连接的情况，端口不能在重用了；
    socket_.Set(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP));
    int ret = ::connect(socket_.fd(), ServerAddress.Return_Pointer(), ServerAddress.Return_length());
    int SaveErrno = (ret == 0) ? 0 : errno;
    switch (SaveErrno){
        case 0:
        case EINPROGRESS: //正在连接
        case EINTR: //当阻塞于某个慢系统调用的一个进程捕获某个信号且相应信号处理函数返回时，该系统调用可能返回一个EINTR错误。
        case EISCONN: //连接成功
            Connecting(socket_);
            //retry(socket_.fd()); //Test retry.
            break;

        case EAGAIN: //临时端口(ephemeral port)不足  
        case EADDRINUSE: //监听的端口已经被使用
        case EADDRNOTAVAIL://配置的IP不对
        case ECONNREFUSED: //服务端在我们指定的端口没有进程等待与之连接
        case ENETUNREACH: //表示目标主机不可达 
            retry(socket_.fd());
            break;

        case EACCES: //没有权限
        case EPERM: //操作不被允许
        case EAFNOSUPPORT: //该系统不支持IPV6
        case EALREADY: //套接字非阻塞且进程已有待处理的连接
        case EBADF: //无效的文件描述符
        case EFAULT: //操作套接字时的一些参数无效
        case ENOTSOCK: //不是一个套接字
            ::close(socket_.fd());
            break;

        default:
            ::close(socket_.fd());
            // connectErrorCallback_();
            break;
    }
}

void 
Connection::Connecting(const Socket& socket_){
    SetConnectionState(kConnecting);
    ClientEpoll->Add(socket_, EpollCanWite()); //这里没办法加进去
}

struct sockaddr_in6 
Connection::getLocalAddr(int sockfd){
    struct sockaddr_in6 localaddr;
    memset(&localaddr, 0, sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    if (::getsockname(sockfd, static_cast<struct sockaddr*>(static_cast<void*>(&localaddr)), &addrlen) < 0){
        std::cerr << "sockets::getLocalAddr";
    }
    return localaddr;
}

struct sockaddr_in6 
Connection::getPeerAddr(int sockfd){
    struct sockaddr_in6 peeraddr;
    memset(&peeraddr, 0, sizeof peeraddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    if (::getpeername(sockfd, static_cast<struct sockaddr*>(static_cast<void*>(&peeraddr)), &addrlen) < 0)
    {
        std::cerr << "sockets::getPeerAddr";
    }
    return peeraddr;
}

int 
Connection::getSocketError(int sockfd){
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof optval);

    if(::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0){
        return errno;
    } else {
        return optval;
    }
}

// 一般而言在连接的时候客户端会让内核随机指定一个端口；自连接其实就是在本地目的IP与目的端口与本端IP还有本地端口相同；
// 明明这个端口已经用于listen，这个端口还是可以用于连接，所以可能出现自连接的情况；很诡异的一种情况
bool 
Connection::isSelfConnect(int sockfd){
    struct sockaddr_in6 localaddr = getLocalAddr(sockfd);
    struct sockaddr_in6 peeraddr = getPeerAddr(sockfd);
    if(localaddr.sin6_family == AF_INET){
        const struct sockaddr_in* laddr4 = reinterpret_cast<struct sockaddr_in*>(&localaddr);
        const struct sockaddr_in* raddr4 = reinterpret_cast<struct sockaddr_in*>(&peeraddr);
        return laddr4->sin_port == raddr4->sin_port
            && laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
    }else if(localaddr.sin6_family == AF_INET6){
        return localaddr.sin6_port == peeraddr.sin6_port
            && memcmp(&localaddr.sin6_addr, &peeraddr.sin6_addr, sizeof localaddr.sin6_addr) == 0;
    }else{
        return false;
    }
}

//正常的写过程 触发写事件时传入fd,判断正确后再加入client的map中
//所以写入是fd如果map中没有的话就是connecting的可写事件
void  
Connection::HandleWrite(int fd, const std::function<void(int)>& newConnectionCallback){
    if(states == kConnecting){
        int err = getSocketError(fd);
        if(err){ //连接错误 
            retry(fd);
            std::cerr << "Connection::HandleWrite error.\n";
        }else if(isSelfConnect(fd)){ //出现自连接，此时重新连接就可以了
            retry(fd);
            std::cerr << "Connection::HandleWrite error.\n";
        }else{
            std::cout << "Connect successful.\n";
            SetConnectionState(kConnected);
            std::cout << "fd : " << fd << std::endl;
            newConnectionCallback(fd); //TODO 应该是加入client的map中 且加入Epoll
            retryDelayMs_ = KInitRetryDelayMs; //为了复用
        }
    }else{
        //This does not happen.
        assert(states == kDisconnected);
    }
}

void 
Connection::retry(int fd){ 
    std::cerr << "start reconnect, Delay is : " << retryDelayMs_ << "S.\n";
    
    ::close(fd);
    socket_.Set(-1);
    
    SetConnectionState(kDisconnected);
    if(retryDelayMs_ == kMaxRetryDelayMs){
        std::cerr << "Reconnect failture, stop reconnect.\n";
        return;
    }
    
    RetryCallBack_(retryDelayMs_);
    retryDelayMs_ = std::min(retryDelayMs_*2, kMaxRetryDelayMs);
}

void
Connection::SetTetryCallBack_(const std::function<void(int)>& callback){
    RetryCallBack_ = callback;
}

}