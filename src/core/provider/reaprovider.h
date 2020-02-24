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

#ifndef REAPROVIDER_H_
#define REAPROVIDER_H_

#include "provider.h"
#include "../../net/writeloop.h"
#include "../../tool/filereader.h"   
#include "../../http/httprequest.h"
#include "../../tool/parsed_header.h"
#include <memory> 

namespace ws{

    class REAProvider : public Provider{
        public:
            REAProvider(std::shared_ptr<HttpRequest> Hr,
                        std::shared_ptr<WriteLoop> Wl) : 
                        Provider(Hr, Wl){}
            void provide() final;
        private: 
            bool FileProvider(std::shared_ptr<FileReader>& file);
    };

}

#endif