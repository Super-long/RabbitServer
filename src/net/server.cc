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

#include "server.h"

#include <assert.h>
#include <iostream>
#include <errno.h>

namespace{
    template <typename T>
    T max(T&& a,T&& b){
        return a > b ? a : b;
    }
}

namespace ws{
    std::unique_ptr<Socket> Server::Server_Accept(){
        return std::make_unique<Socket>(::accept4(fd(), nullptr, nullptr, SOCK_NONBLOCK));
    }

    void Server::Server_Accept(fun&& f){ 
/*         int ret = 0;
        ret = ::accept4(fd(), nullptr, nullptr, SOCK_NONBLOCK);
        if(ret != -1){
            f(ret);
            std::cout << "已接收一个新的连接 fd : " << ret << std::endl;
        }
        else if(errno == EMFILE){
            fileopen_helper prevent(FileOpen);
        } */
        // accpect返回-1的情况 http://manpages.org/accept4/2
        // 套接字被设置成非阻塞，需要一个while循环来接收
        while(1){
            int ret = 0;
            ret = ::accept4(fd(), nullptr, nullptr, SOCK_NONBLOCK); // 减少一次系统调用
            if(ret != -1){
                // 成功以后才会分发套接字；失败直接退出就可以了；
                f(ret);
                std::cout << "已接收一个新的连接 fd : " << ret << std::endl;
            } else if (ret == -1 && errno == EMFILE){
                // 我的架构中只有一个线程accept，所以此做法可以保证安全；
                fileopen_helper prevent(FileOpen);
                // 防止堆积在全连接队列中，直接告诉对端关闭连接；
                ret = ::accept4(fd(), nullptr, nullptr, SOCK_NONBLOCK);
                ::close(fd());
                break;
            } else if (ret == -1 && errno == EWOULDBLOCK){    // EWOULDBLOCK
                continue;
            } else if (ret == -1){   // ECONNABORTED 在等待队列的时候被关闭了，不是非阻塞套及字的话会一直阻塞；进入这个条件判断可能还是一些比较扯淡的条件，具体见man手册
                break;
            } else {
                // 这种情况理论是不会出现的；
                std::cerr << "ERROR : Server::Server_Accept, unexpected situation.\n";
            }
        }
        

/*         for(int _fd; (_fd = accept4(fd(), nullptr, nullptr, SOCK_NONBLOCK)) != -1; ){
            f(_fd);
            std::cout << "已接收一个新的连接 fd : " <<_fd << std::endl; 
        } */
    }

    void Server::Server_BindAndListen(){ 
        assert(Addr_ != nullptr); 
        int para1 = bind(fd(), Addr_->Return_Pointer(), Addr_->Return_length());
        if(para1 == -1) throw std::runtime_error("'Server_BindAndListen' : error in bind.");
        int para2 = listen(fd(), ::max(SOMAXCONN, 1024));
        if(para2 == -1) throw std::runtime_error("'Server_BindAndListen' : error in listen.");        
    }

    inline int Server::Set_Linger(){
        /*
        struct linger {
            int l_onoff;  0 = off, nozero = on 
            int l_linger;  linger time 
        }; 
        */
        struct linger buffer_ = {1,0};
        return setsockopt(fd(), SOL_SOCKET, SO_LINGER, &buffer_, sizeof(buffer_));
    }
}