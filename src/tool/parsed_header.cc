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

#include "parsed_header.h"

#include <iostream>

namespace ws{

    bool ParsedHeader::ParsedHeaderIsEqual(const ParsedHeader& para) const{
        if(length != para.Readable()) return false;
        if(memcmp(Header, para.ReadPtr(), length) == 0) return true;
        return false; 
    }

    bool ParsedHeader::ParsedHeaderIsEqual(const char* ptr) const{
        if(length != strlen(ptr)) return false;
        if(memcmp(Header, ptr, length) == 0) return true;
        return false;
    }

    bool ParsedHeader::ParsedHeaderIsEqual(const std::string& str) const{
        if(length != str.length()) return false;
        if(memcmp(Header, str.c_str(), length) == 0) return true;
        return false;
    }

    std::ostream& operator<<(std::ostream& os, const ParsedHeader& ptr){
        os.write(ptr.ReadPtr(), ptr.Readable());
        return os;
    }

    bool ParsedHeader::operator==(const ParsedHeader& para) const{
        bool Len_is_equal = (length == para.Readable());
        return (Len_is_equal && (strncmp(Header, para.ReadPtr(), length) == 0));
    }
}