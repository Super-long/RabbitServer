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

#ifndef HTTPPARSER_H_
#define HTTPPARSER_H_

#include "../base/nocopy.h"
#include "../tool/userbuffer.h"
#include "httprequest.h"
#include "httpstatus.h"
#include "../net/socket.h"

#include <memory>

#ifndef __GNUC__

#define  __attribute__(x)  /*NOTHING*/

#endif

namespace ws{ 

    class HttpParser : public Nocopy{
        public: 
            HttpParser(std::shared_ptr<UserBuffer> ptr, std::shared_ptr<HttpRequest> request, Extrabuf* extra):
                User_Buffer_(std::move(ptr)),Parser_Result(std::make_unique<HttpParser_Content>()), Request_Result(request), Extrabuffer_(extra){}

            void __attribute__((hot)) Again_Parser(); 
            HttpParserFault __attribute__((hot)) Starting_Parser();
            bool __attribute__((hot)) Finished() const {
                // Faulth初始值是HPFOK
                return Parser_Result->Fault == HPFContent;
                //return Parser_Result->Fault != HPFOK;
            }
            
            bool SetRequesting();

            // 名字起的奇形怪状是因为clear是最后加上的，前面相同功能的函数是做其他事情的；
            void clear(){
                User_Buffer_->Clean();
                Parser_Result->init();
                Request_Result->clear();
                // Extrabuffer_在socket clear的时候已经初始化了
            }

        private: 
            std::shared_ptr<UserBuffer> User_Buffer_;
            std::unique_ptr<HttpParser_Content> Parser_Result;
            std::shared_ptr<HttpRequest> Request_Result; 
            Extrabuf* Extrabuffer_;
            bool Parsering();
            bool Parser_able() __attribute__((pure)) {return User_Buffer_->Readable() >= 16;}  // 请求行+空行，最小的HTTP方法为三字节
    };

}

#endif