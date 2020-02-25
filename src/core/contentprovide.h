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

#ifndef CONTENTPROVIDER_H_
#define CONTENTPROVIDER_H_

#include "../base/nocopy.h"
#include "../net/writeloop.h"
#include "../http/httprequest.h"
#include "provider/provider.h"
#include "provider/reaprovider.h"

#include <memory>

namespace ws{

    class ContentProvider : public Nocopy{
        public:
            ContentProvider(std::shared_ptr<HttpRequest> Hr, std::shared_ptr<WriteLoop> Wl) : 
                Http_Request(Hr), Write_Loop(Wl){}
            void Provide();
        private: 
            std::unique_ptr<Provider> Pro;
            std::shared_ptr<HttpRequest> Http_Request;
            std::shared_ptr<WriteLoop> Write_Loop;
    };
}

#endif