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

#ifndef Y_DRAGON_H_
#define Y_DRAGON_H_

#include "../http/httprequest.h"
#include "../net/address.h"
#include <memory>

namespace Y_Dragon{

    constexpr char* Version(){
        return (char*)"0.0";
        //return static_cast<char*>("0.0");
    }
    
    constexpr int MyPort(){
        return 8888;
    }

    constexpr char* MyIP(){
        return (char*)"127.0.0.1";
    }

    constexpr int FastCgiPort();

    constexpr char* FastCgiIP();

    constexpr int EventResult_Number(){
        return 8096;
    }

}

#endif