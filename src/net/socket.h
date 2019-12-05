#ifndef SOCKET__H_
#define SOCKET__H_


#include"../base/havefd.h"
#include"../base/copyable.h"
#include"../tool/userbuffer.h"

#include<sys/epoll.h>
#include<sys/socket.h>
#include<memory>
#include<unistd.h>
#include<fcntl.h>

namespace ws{ 
    class Socket : public Havefd,Copyable{
        public:
            Socket() : Socket_fd_(socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0)){
                //SetNoblockingCLOEXEC();
            }
            explicit Socket(int fd) : Socket_fd_(fd){}
            explicit Socket(const Havefd& Hf) : Socket_fd_(Hf.fd()){}
            explicit Socket(const Havefd&& Hf) : Socket_fd_(Hf.fd()){}
            
            virtual ~Socket() {if(Have_Close_) ::close(Socket_fd_);}
            
            int Close(); 
            
            int fd() const noexcept override {return Socket_fd_; }
            int SetNoblocking(int flag = 0);
            int SetNoblockingCLOEXEC(){
                return Socket::SetNoblocking(O_CLOEXEC);
            }

            int Read(char* Buffer, int Length, int flag = 0);
            int Read(std::shared_ptr<UserBuffer>, int length = -1, int flag = 0);

            int Write(char* Buffer, int length, int flag = 0);
            //int Read(...)

        private:
            bool Have_Close_ = true;
            int Socket_fd_;
    };
}

#endif