#include "socket.h"
#include <errno.h>

#include <iostream>

namespace ws{
    int Socket::Close(){
        int rv = ::close(Socket_fd_);
        if(rv != -1) Have_Close_ = false;
        return rv;
    } 

    int Socket::SetNoblocking(int flag){
        int old_option = fcntl(Socket_fd_,F_GETFL);
        int new_option = old_option | O_NONBLOCK | flag;
        fcntl(Socket_fd_,F_SETFL,new_option);
        return old_option;
    } 

    int Socket::Read(std::shared_ptr<UserBuffer> ptr, int length, int flag){
 
        if(length == -1 || length > ptr->Writeable()){  
            length = ptr->Writeable();
        }
        //deepin 15.7 x86 long int
        ssize_t sum = 0;
        ssize_t ret = 0;
        while(true){
            std::cout << "errno : " << errno << std::endl;
            ret = recv(Socket_fd_,ptr->WritePtr(),static_cast<size_t>(length),flag);
            //ret = read(Socket_fd_,ptr->WritePtr(),static_cast<size_t>(length));
            if(ret != -1){ 
                sum += ret;
                ptr->Write(ret);
            }else if(ret < 0 ){ //errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR        
                if(errno == EWOULDBLOCK || errno == EAGAIN)
                    break;
                else if(errno == EINTR)
                    continue;
            }
        }
        std::cout << "recv : " << sum << std::endl; 
        return static_cast<int>(sum);
    }

    int Socket::Read(char* Buffer, int length, int flag){
        return static_cast<int>(recv(Socket_fd_,static_cast<void*>(Buffer),
        static_cast<ssize_t>(length),flag));
    }

    int Socket::Write(char* Buffer, int length, int flag){
        return static_cast<int>(send(Socket_fd_,static_cast<void*>(Buffer),
        static_cast<ssize_t>(length),flag));
    }
}