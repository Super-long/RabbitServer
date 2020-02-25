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

namespace ws{
    class Server : public Socket{ 
            using fun = std::function<void(int)>;
        public: 
            Server(const Address& addr_) : Addr_(std::make_unique<Address>(addr_)),FileOpen(){}
            Server(const char* buffer, int port) : Addr_(std::make_unique<Address>(buffer,port)),FileOpen(){}
            explicit Server(int port) : Addr_(std::make_unique<Address>(port)),FileOpen(){}

            std::unique_ptr<Socket> Server_Accept(); 
            void Server_Accept(fun&& f);
            void Server_BindAndListen();  

            int Set_AddrRUseA() {return Set_Socket(SO_REUSEADDR);}
            int Set_AddrRUseP() {return Set_Socket(SO_REUSEPORT);}
            int Set_KeepAlive() {return Set_Socket(SO_KEEPALIVE);}
            int Set_Linger();

            //TODO Nagle 

        private:
            std::unique_ptr<Address> Addr_;
            fileopen FileOpen; 
            int Set_Socket(int event_type,void* ptr = nullptr){
                int buffer_ = 0;
                return setsockopt(fd(), SOL_SOCKET, event_type, &buffer_, sizeof(buffer_));
            }
    };
}

#endif 