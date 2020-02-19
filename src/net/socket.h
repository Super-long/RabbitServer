#ifndef SOCKET__H_
#define SOCKET__H_


#include "../base/havefd.h"
#include "../base/copyable.h"
#include "../tool/userbuffer.h"

#include <sys/epoll.h> 
#include <sys/socket.h>
#include <memory>
#include <functional>
#include <string.h> 
#include <unistd.h>
#include <fcntl.h>

namespace ws{

    class Extrabuf : public Nocopy{
        enum isvaild {INVAILD = -1, VAILD};
        public:
            void init(){
                extrabuf = std::make_unique<char[]>(4048);
                ExtrabufPeek = static_cast<int>(VAILD);
            }
            char* Get_ptr() const noexcept{
                return extrabuf.get();
            }

            int Get_length() const noexcept{
                return ExtrabufPeek;
            }

            void Write(int spot) noexcept {
                ExtrabufPeek = spot;
            }

            int WriteAble() const noexcept{
                return BufferSize - ExtrabufPeek;
            }

            bool IsVaild() const noexcept{
                return ExtrabufPeek == INVAILD ? false : true;
                //return static_cast<bool>(ExtrabufPeek);
            }

            bool Reset(){
                std::unique_ptr<char[]> TempPtr = std::make_unique<char[]>(BufferSize);
                memcpy(TempPtr.get(), extrabuf.get(), BufferSize);
                extrabuf.reset(new char[BufferSize*2]);
                memcpy(extrabuf.get(), TempPtr.get(), BufferSize);
                BufferSize*=2;
            }

            void SetHighWaterMarkCallback_(std::function<void()> fun){
                highWaterMarkCallback_ = std::move(fun);
            }

            bool IsExecutehighWaterMark() const noexcept{
                return Get_length() >= highWaterMark_;
            }

            void Callback(){
                if(highWaterMarkCallback_) highWaterMarkCallback_();
            }
        private:
            static const int highWaterMark_ = 64*1024*1024;
            std::function<void()> highWaterMarkCallback_;
            std::unique_ptr<char[]> extrabuf = nullptr; 
            int ExtrabufPeek = static_cast<int>(isvaild::INVAILD);
            int BufferSize = 4048;
    };

    class Socket : public Havefd,Copyable{
        public:
            Socket() : Socket_fd_(socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0)){
                //SetNoblockingCLOEXEC();
                SetKeepAlive();
                SetNoDelay(); 
            }
            explicit Socket(int fd) : Socket_fd_(fd){}
            explicit Socket(const Havefd& Hf) : Socket_fd_(Hf.fd()){}
            explicit Socket(const Havefd&& Hf) : Socket_fd_(Hf.fd()){}
            
            virtual ~Socket() {if(Have_Close_) ::close(Socket_fd_);}
            
            int Close(); 
            int Shutdown() {return ::shutdown(Socket_fd_, SHUT_RDWR);}
            int ShutdownWrite() {return ::shutdown(Socket_fd_, SHUT_WR);}
            int ShutdownRead() {return ::shutdown(Socket_fd_, SHUT_RD);}
            
            int fd() const noexcept override {return Socket_fd_; }
            int SetNoblocking(int flag = 0);
            int SetNoblockingCLOEXEC(){
                return Socket::SetNoblocking(O_CLOEXEC); 
            }
            int SetNoDelay(); //Forbid Nagle. 
            int SetKeepAlive();

            int Read(char* Buffer, int Length, int flag = 0);
            int Read(std::shared_ptr<UserBuffer>, int length = -1, int flag = 0);

            int Write(char* Buffer, int length, int flag = 0);
            //int Read(...)

        private:
            Extrabuf ExtraBuffer_;
            bool Have_Close_ = true;
            int Socket_fd_;
    };
}

#endif