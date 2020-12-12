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

#include "httpstatus.h"

namespace ws{
    
    //For debugging
    std::ostream& operator<<(std::ostream& os, const HttpParser_Content& para){
        using std::endl;
        os << "method : " << static_cast<int>(para.method) << endl;
        os << "Status : " << static_cast<int>(para.Status) << endl;
        os << "Fault  : " << static_cast<int>(para.Fault) << endl;
        os << "content_length : " << para.Content_length << endl;
        os << "V_major : " << para.V_major << endl;
        os << "V_minor : " << para.V_minor << endl;
        os << "Set_ka  : " << static_cast<int>(para.Set_Ka) << endl; 
        //os << "uri    : " << para.Uri << endl;
        os << "uri_length : " << para.Uri_length << endl;
        return os;
    }
}