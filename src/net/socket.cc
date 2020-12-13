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

#include "socket.h"
#include <errno.h>
#include <sys/socket.h>
#include <netinet/tcp.h> //TCP_NODELAY

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

        char* strart = ptr->WritePtr();

        char* StartBuffer = ptr->WritePtr();
        while(true){
            ret = recv(Socket_fd_, StartBuffer, static_cast<size_t>(length), flag);
/*             std::cout << "errno : " << errno << std::endl;
            std::cout << "ret : " << ret << std::endl; */
            // ret = read(Socket_fd_,ptr->WritePtr(),static_cast<size_t>(length)); 

            // 显然每次length大于等于ret
            if(ret != -1 && !ExtraBuffer_.IsVaild()){ 
                sum += ret;
                length -= ret;  // 目前缓冲区中有效的buffer长度
                ptr->Write(ret);
                StartBuffer = ptr->WritePtr();

                if(!ptr->Writeable()){ //Buffer is full.
                    ExtraBuffer_.init();    // 初始化额外的缓冲区
                    StartBuffer = ExtraBuffer_.Get_ptr();
                    length = ExtraBuffer_.WriteAble();
                }
            } else if (ret != -1 && ExtraBuffer_.IsVaild()){
                sum += ret;
                length -= ret;
                ExtraBuffer_.Write(ret);
                StartBuffer = ExtraBuffer_.Get_ptr();
                if(!ExtraBuffer_.WriteAble()){ //Extrabuffer is full.
                    if(ExtraBuffer_.IsExecutehighWaterMark()){
                        ExtraBuffer_.Callback();
                        //return -1;
                        break;
                    }
                    ExtraBuffer_.Reset();
                    StartBuffer = ExtraBuffer_.Get_ptr();
                    length = ExtraBuffer_.WriteAble();
                }
            } else if (ret < 0){ // ret == -1
                // https://man7.org/linux/man-pages/man2/recv.2.html
                if(errno == EWOULDBLOCK || errno == EAGAIN)
                    break;
                else if(errno == EINTR)
                    continue;
                else {
                    // 走到这里就是各种各样奇怪的错误了，没必要在做细化的区分，直接退出就OK；
                    std::cerr << "ERROR : Socket::Read.\n";
                    break;
                }
            }
        }
        std::string str(strart, ptr->Readable());
/*         std::cout << "内容 : \n" << str << std::endl;
        std::cout << "readable : " << ptr->Readable() << std::endl;*/
        std::cout << "一次recv的完成 socket.cc : " << sum << std::endl;
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

    // 写重复了，算了，不管了，反正能跑；
    int Socket::SetNoDelay(){ //TCP_CORK
        int optval  = 1; 
        return ::setsockopt(fd(), SOL_SOCKET, TCP_NODELAY,
                    &optval, static_cast<socklen_t>(sizeof optval));
    }

    int Socket::SetKeepAlive(){
        int optval  = 1;
        return ::setsockopt(fd(), SOL_SOCKET, SO_KEEPALIVE,
                    &optval, static_cast<socklen_t>(sizeof optval));
    }
}