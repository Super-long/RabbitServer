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

#include "../base/config.h"
#include <signal.h>
#include <iostream>
#include <assert.h> 
#include <sys/time.h>
#include "ws.h"

namespace ws{

    int64_t Get_Current_Time(){
        timeval now;
        int ret = gettimeofday(&now, nullptr);
        assert(ret != 1);
        return now.tv_sec*1000 + now.tv_usec / 1000;
    }

    Web_Server::Web_Server() : _Epoll_(), _Manger_(_Epoll_), _Server_(Y_Dragon::MyPort()){}
 
    void Web_Server::Running(){ 
        try{
            signal(SIGPIPE, SIG_IGN);
            _Server_.Set_AddrRUseP();
            _Server_.Server_BindAndListen();
            _Epoll_.Add(_Server_, EpollCanRead());
            EpollEvent_Result Event_Reault(Y_Dragon::EventResult_Number());

            channel_helper Channel_;
            Channel_.loop();

            while(true){
                //constexpr int Second = 20;
                _Epoll_.Epoll_Wait(Event_Reault);
                for(int i = 0; i < Event_Reault.size(); ++i){
                    auto & item = Event_Reault[i];
                    int id = item.Return_fd();

                    if(id == _Server_.fd()){ //这里放入事件循环
                        _Server_.Server_Accept([&](int fd){Channel_.Distribution(fd);});
                        _Epoll_.Modify(_Server_, EpollCanRead());
                    }
                }
            }
        } catch (std::exception& err){
            std::cout << err.what() << std::endl;
        }
    }
}