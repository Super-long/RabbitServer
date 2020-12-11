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

#include "address.h"

#include <strings.h>
#include <cassert>

namespace ws{
    Address::Address(const char* IP, int port){
        assert(port > 0);

        bzero(&addr_,sizeof(addr_));

        addr_.sin_addr.s_addr = inet_addr(IP);
        addr_.sin_port = htons(static_cast<uint16_t >(port));
        addr_.sin_family = AF_INET;
    }

    Address::Address(int port){
        assert(port > 0);
    
        bzero(&addr_,sizeof(addr_));
    
        //addr_.sin_addr.s_addr = htonl(INADDR_ANY); 
        addr_.sin_addr.s_addr = inet_addr("127.0.0.1");        
        addr_.sin_port = htons(static_cast<uint16_t >(port));
        addr_.sin_family = AF_INET;
    } 
}