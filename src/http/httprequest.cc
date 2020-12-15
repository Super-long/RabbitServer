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

#include "httprequest.h"

#include <algorithm>

namespace ws{
    void HttpRequest::Store_Header(const ParsedHeader& header, const ParsedHeader& value){
        Header_Value.emplace(header, value); 
    }

    ParsedHeader HttpRequest::Get_Value(const ParsedHeader& header) const{
/*         auto T = Header_Value.find(header); 
        if(T == Header_Value.end()){   
            throw std::invalid_argument("'HttpRequest::GrtValue' No such user.");
        }
        return T->second; */  
 
        //Pointers have different hash values.

        auto temp = std::find_if(Header_Value.begin(), Header_Value.end(), [&header](auto para){
            return para.first.ParsedHeaderIsEqual(header);
        });
        
        if(temp == Header_Value.end()) throw std::invalid_argument("'HttpRequest::GrtValue' No such user.");
        return temp->second;
    }
}