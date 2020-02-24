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
        while(1){
            int ret = 0;
            ret = ::accept4(fd(), nullptr, nullptr, SOCK_NONBLOCK);
            if(ret != -1){
                f(ret);
                std::cout << "已接收一个新的连接 fd : " << ret << std::endl;
            }else if(ret == -1 && errno == EMFILE){
                fileopen_helper prevent(FileOpen);
                break;
            }else if(ret == -1){
                break;
            } 
        }
        

/*         for(int _fd; (_fd = accept4(fd(), nullptr, nullptr, SOCK_NONBLOCK)) != -1; ){
            f(_fd);
            std::cout << "已接收一个新的连接 fd : " <<_fd << std::endl; 
        } */
    }

    void Server::Server_BindAndListen(){ 
        assert(Addr_ != nullptr); 
        int para1 = bind(fd(),Addr_->Return_Pointer(),Addr_->Return_length());
        if(para1 == -1) throw std::runtime_error("'Server_BindAndListen' : error in bind.");
        int para2 = listen(fd(),::max(SOMAXCONN,1024));
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