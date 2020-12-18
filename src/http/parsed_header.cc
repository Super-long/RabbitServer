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
#include <assert.h>

#ifndef __GNUC__

#define  __attribute__(x)  /*NOTHING*/
    
#endif

namespace ws{

    constexpr inline int __attribute__((always_inline)) my_memcmp(const void *buffer1,const void *buffer2,int count) throw() {
        if (!count)
            return(0);
        while ( --count && *(char *)buffer1 == *(char *)buffer2)
        {
            buffer1 = (char *)buffer1 + 1;
                buffer2 = (char *)buffer2 + 1;
        }
        return( *((unsigned char *)buffer1) - *((unsigned char *)buffer2) );
    }

    constexpr inline int __attribute__((always_inline)) my_strlen(const char* str) throw() {
        const char *p = str;  
        while(*p)  
        {  
            p++;  
        }  
        return p-str;
    }

    // 这里其实可以引入一个优化，就是用数字比较代替字符串比较；
    // Fix:后期可大量替换strlen，memcmp这样的函数为自己的，写成constexper，最大幅度减少函数调用的开销
    bool ParsedHeader::ParsedHeaderIsEqual(const ParsedHeader& para) const{
        if(length != para.Readable()) return false;
        if(my_memcmp(Header, para.ReadPtr(), length) == 0) return true;
        return false; 
    }

    bool ParsedHeader::ParsedHeaderIsEqual(const char* ptr) const{
        if(length != my_strlen(ptr)) return false;
        if(my_memcmp(Header, ptr, length) == 0) return true;
        return false;
    }

    bool ParsedHeader::ParsedHeaderIsEqual(const std::string& str) const{
        if(length != str.length()) return false;
        if(my_memcmp(Header, str.c_str(), length) == 0) return true;
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