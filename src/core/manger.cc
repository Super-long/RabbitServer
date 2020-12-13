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

#include "manger.h"
#include "../net/epoll_event.h"

#include <functional>

#include <sys/socket.h>

namespace ws{
    int Manger::Opera_Member(std::unique_ptr<Member>& ptr, EpollEventType& EE){
        int id = ptr->fd();         //RTTI
        Fd_To_Member.emplace(id,ptr.release());
        _Epoll_.Add(*Fd_To_Member[id], std::move(EE)); 
        return id;      
    }

    int Manger::Opera_Member(std::unique_ptr<Member>&& ptr, EpollEventType&& EE){
        int id = ptr->fd();
        Fd_To_Member.emplace(id, std::move(ptr.release()));
        _Epoll_.Add(*Fd_To_Member[id],std::move(EE));
        return id;
    }

    int Manger::Opera_Member(std::unique_ptr<Member>&& ptr, EpollEventType& EE){
        int id = ptr->fd();         
        Fd_To_Member.emplace(id, std::move(ptr.release()));
        _Epoll_.Add(*Fd_To_Member[id],EE); 
        return id;      
    }

    int Manger::Opera_Member(std::unique_ptr<Member>& ptr, EpollEventType&& EE){
        int id = ptr->fd();         
        Fd_To_Member.emplace(id,ptr.release());
        _Epoll_.Add(*Fd_To_Member[id], std::move(EE)); 
        return id;      
    }

    int Manger::Remove(int fd){
        if(!Exist(fd)){
            throw std::invalid_argument("'Manger::Remove' Don't have this fd.");
        }
        _Epoll_.Remove(*Fd_To_Member[fd],EpollTypeBase());
        return Fd_To_Member.erase(fd);
    }

    int Manger::Update(int fd){
        if(!Exist(fd)){
            throw std::invalid_argument("'Manger::Update' Don't have this fd.");
        }
        return _Epoll_.Modify(*Fd_To_Member[fd],EpollCanRead());
    }

    int Manger::UpdateWrite(int fd){
        if(!Exist(fd)){
            throw std::invalid_argument("'Manger::Update' Don't have this fd.");
        }
        return _Epoll_.Modify(*Fd_To_Member[fd],EpollCanWite());  
    }

    /**
     * @notes: 可写事件进入这里的时候如果写完会从后两个判断出去，取决于连接状态，因为确定包是收全的；
    */
    int Manger::JudgeToClose(int fd){   // 函数没有返回值
        if(!Exist(fd)){
            throw std::invalid_argument("'Manger::JudgeToClose' Don't have this fd.");
        }
        auto& OneMember = Fd_To_Member[fd];
        if(!OneMember->IsWriteComplete()){  // 解析成功，缓冲区写入未完成
            // 注册可写事件，等待套接字可写
            UpdateWrite(fd);
            return 0;
        } else if (OneMember->CloseAble()){
            _Epoll_.Remove(static_cast<EpollEvent>(fd));
            auto temp = Fd_To_Member.find(fd);
            Fd_To_Member.erase(fd);   
        } else {    // 解析失败，包没收全；或者包收全，状态为keep-alive
            Update(fd);
        }
        return 0;   // 设计失误
    }
 
    void Manger::Reading(int fd, long _time_){ 
        if(!Exist(fd)){
            throw std::invalid_argument("'Manger::Reading' Don't have this fd.");
        }
        auto& user = Fd_To_Member[fd];
        user->DoRead();
    } 

    void Manger::InsertTimeWheel(int fd){
        using std::placeholders::_1; 
        if(Exist(fd)) Timer_Wheel_->TW_Update(fd);
        else Timer_Wheel_->TW_Add(fd, std::bind(&Manger::Remove, this, _1));
    }

    void Manger::Writing(int fd, long time){
        if(!Exist(fd)){
            throw std::invalid_argument("'Manger::Reading' Don't have this fd.");
        }
        auto& user = Fd_To_Member[fd];
        user->DoWrite();
    }
}