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

#include "config.h"

namespace Y_Dragon{
    // 1. Too many requests from the same IP.
    // 2. IP is blacklisted.
    // 3. Insufficient file permissions.
//    bool IsForbidden(std::shared_ptr<ws::HttpRequest>){
//    }

/*     constexpr int MyPort(){
        return 8888;
    } */

    constexpr int FastCgiPort(){
        return 9000;
    }

/*     constexpr int EventResult_Number(){
        return 8096;
    }
 */
    constexpr char* FastCgiIP(){
        return (char*)"127.0.0.1";
    }

/*     constexpr char* MyIP(){
        return (char*)"127.0.0.1";
    }
 */
}