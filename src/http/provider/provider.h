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
#include "../../tool/writeloop.h"
#include "../../http/httpstatus.h"
#include <memory>
#include <iostream>

namespace ws{
    /*
     * @function: 根据解析的结果把响应报文写到writeloop中;
     * @notes: 这里使用一个纯虚函数是因为考虑到provider的provide可能会有多种不同的可能性，且单一个Provider没有什么意义；
     * 当然使得类的负载更重了，但是效率没有什么影响;
     * */
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
            std::string MIME(const char*, ptrdiff_t) const;
            std::string AutoAdapt() const;//用于指定响应数据的类型和编码
            bool Good() const {return _Request_->Request_good();}

            static std::string defaultMIME() {return "text/html\n";}
            static constexpr bool IsFilename(char);
 
            virtual void provide() = 0;

        protected:
            std::shared_ptr<HttpRequest> _Request_;
            std::shared_ptr<WriteLoop> _Write_Loop_;
    };
}

#endif 