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

#ifndef PROVIDER_H_
#define PROVIDER_H_

#include "../../base/copyable.h"
#include "../../http/httprequest.h"
#include "../../net/writeloop.h"
#include "../../http/httpstatus.h"
#include <memory>
#include <iostream>

namespace ws{
    class Provider : public Copyable{
        public:
            Provider(std::shared_ptr<HttpRequest> Hr,
                     std::shared_ptr<WriteLoop> Wl) : _Request_(Hr), _Write_Loop_(Wl){}
            int WriteHead(int, int, const HttpStatusCode& );
            int WriteItem(const char*, const char*);
            int WriteConnection();
            int WriteDate();
            int WriteCRLF();
  
            int ProvideError();
            int RegularProvide(long Content_Length);
            int RegularProvide(long Content_Length, const char*); 
                                        //long int
            const char* MIME(const char*, ptrdiff_t) const;
            const char* AutoAdapt() const;//用于指定响应数据的类型和编码
            constexpr const char* defaultMIME() const{return "text/html\n";}
            bool Good() const {return _Request_->Request_good();} 
            bool IsFilename(char) const;
 
            virtual void provide() = 0;
        protected:

            std::shared_ptr<HttpRequest> _Request_;
            std::shared_ptr<WriteLoop> _Write_Loop_;
    };
}

#endif 