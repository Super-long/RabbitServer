#ifndef SERVER_H_
#define SERVER_H_

#include"socket.h"
#include"../net/address.h"
#include<memory>
#include<functional>

namespace ws{
    class Server : public Socket{
            using fun = std::function<void(int)>;
        public:
            Server(const Address& addr_) : Addr_(std::make_unique<Address>(addr_)){}
            Server(const char* buffer, int port) : Addr_(std::make_unique<Address>(buffer,port)){}
            explicit Server(int port) : Addr_(std::make_unique<Address>(port)){}

            std::unique_ptr<Socket> Server_Accept();
            void Server_Accept(fun&& f);
            void Server_BindAndListen(); 

            int Set_AddrRUseA() {return Set_Socket(SO_REUSEADDR);}
            int Set_AddrRUseP() {return Set_Socket(SO_REUSEPORT);}
            int Set_KeepAlive() {return Set_Socket(SO_KEEPALIVE);}
            int Set_Linger();

        private:
            std::unique_ptr<Address> Addr_;
            int Set_Socket(int event_type,void* ptr = nullptr){
                int buffer_ = 0;
                return setsockopt(fd(), SOL_SOCKET, event_type, &buffer_, sizeof(buffer_));
            }
    };
}

#endif 