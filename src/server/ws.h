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

#ifndef WS_H_
#define WS_H_

#include "server.h"
#include "../net/epoll.h"
#include "manger.h"
#include "channel.h"
#include "../tool/timer.h"

namespace ws{
    class Web_Server{ 
        public:
            Web_Server();
            void Running();
        private: 
            Manger _Manger_;
            Epoll _Epoll_; 
            Server _Server_;
            Timer _Timer_;
    };
}

#endif