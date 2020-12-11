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

#ifndef ADDRESS_H_
#define ADDRESS_H_

#include "../base/copyable.h"

#include <arpa/inet.h>

namespace ws{
    class Address : public Copyable{ 
        public:
            explicit Address(const struct sockaddr_in& para) : addr_(para){}
            Address(const char* IP, int port);
            explicit Address(int port);

            const sockaddr* Return_Pointer() & noexcept {return static_cast<const sockaddr*>(static_cast<void*>(&addr_));}
            size_t Return_length() const & noexcept {return sizeof(struct sockaddr_in);} 
        private:
            struct sockaddr_in addr_;
    };
}


#endif 