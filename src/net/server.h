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

#ifndef SERVER_H_
#define SERVER_H_

#include "socket.h"
#include "../net/address.h"
#include "../tool/fileopen.h"

#include <memory>
#include <functional>
#include <netinet/tcp.h> 

namespace ws{
    class Server : public Socket{ 
            using fun = std::function<void(int)>;
        public: 
            Server(const Address& addr_) : Addr_(std::make_unique<Address>(addr_)), FileOpen(){}
            Server(const char* buffer, int port) : Addr_(std::make_unique<Address>(buffer,port)), FileOpen(){}
            explicit Server(int port) : Addr_(std::make_unique<Address>(port)), FileOpen(){}

            std::unique_ptr<Socket> Server_Accept(); 
            void Server_Accept(fun&& f);
            void Server_BindAndListen();

            int Set_AddrRUseA() {return Set_Socket(SO_REUSEADDR, SOL_SOCKET);}  // 防止服务器重启受阻
            // https://blog.csdn.net/chen_fly2011/article/details/56480925?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522160761747019725271062162%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=160761747019725271062162&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduend~default-1-56480925.first_rank_v2_pc_rank_v29&utm_term=SO_REUSEPORT&spm=1018.2118.3001.4449
            int Set_AddrRUseP() {return Set_Socket(SO_REUSEPORT, SOL_SOCKET);}  // 用于解决单线程accept的瓶颈与负载不均衡的问题，我的架构用不到这个，但它很有用；
            int Set_KeepAlive() {return Set_Socket(SO_KEEPALIVE, SOL_SOCKET);}  // TCP保活机制

            int Set_Nodelay() {return Set_Socket(TCP_NODELAY, SOL_TCP);}        // Nagle
            int Set_COPK() {return Set_Socket(TCP_CORK, SOL_TCP);}              // TCP_CORK可以提升吞吐量，在确认要传输传输大于MSS的数据时使用
            int Set_QuickAck() {return Set_Socket(TCP_QUICKACK, SOL_TCP);}      // 禁止延迟确认机制，减少时延
            int Base_Setting() {return Set_Socket(TCP_NODELAY | TCP_QUICKACK, SOL_TCP);}    // 减少一次系统调用
            int Set_Linger();   // close时直接发送RST报文

        private:
            std::unique_ptr<Address> Addr_;
            fileopen FileOpen; 
            int Set_Socket(int event_type, int level){
                int buffer_ = 0;
                return setsockopt(fd(), level, event_type, &buffer_, sizeof(buffer_));
            }

    };
}

#endif 