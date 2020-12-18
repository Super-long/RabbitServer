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

#include "userbuffer.h"
#include <cstring>

#ifndef __GNUC__

#define  __attribute__(x)  /*NOTHING*/
    
#endif

namespace ws{ 

    // 向Start这个缓冲区中写入bytes个字节，并把read_slot向后移动
    void __attribute__ ((access (write_only, 1))) UserBuffer::Read(char* Start, int bytes) {
        if(Read_Spot + bytes > Write_Spot){
            throw std::out_of_range("'UserBuffer::Read' Out ou range when reading.");
        }
        memcpy(Start,Buffer_.get() + Read_Spot, static_cast<size_t>(bytes));
        Read_Spot += static_cast<size_t>(bytes);
    }

/*     void UserBuffer::Read(std::string& str, int bytes){
        if(Read_Spot + bytes > Write_Spot){
            throw std::out_of_range("'UserBuffer::Read' Out ou range when reading.");
        }
        memcpy(const_cast<char*>(str.c_str()),Buffer_.get() + Read_Spot, static_cast<size_t>(bytes));
        Read_Spot += static_cast<size_t>(bytes);  
    } */

    std::unique_ptr<char[]> UserBuffer::Read(int bytes){
        if(Read_Spot + bytes > Write_Spot){
            throw std::out_of_range("'UserBuffer::Read' Out ou range when reading.");
        }
        std::unique_ptr<char[]> ptr(new char(Readable()));
        memcpy(ptr.get(), Buffer_.get() + Read_Spot, static_cast<size_t>(bytes));
        Read_Spot += static_cast<size_t>(bytes);
        return ptr;
    }

    char __attribute__((hot)) __attribute__((pure)) UserBuffer::Peek(int jump) const{
        if(Write_Spot < Read_Spot + jump && Read_Spot + jump < 0 ){
            throw std::out_of_range("'UserBuffer::Peek' The parameter is not in the vaild range.");
        }
        return Buffer_[Read_Spot + jump];
    }

    // 返回成功写入的字节数
    int __attribute__((hot)) UserBuffer::Write(int bytes){
        if(bytes > Writeable()){
            throw std::out_of_range("'UserBuffer::Write' The parameter is not in the valid range");
        }
        Write_Spot += bytes;
        Move_Buffer();
        return bytes;
    }

    int __attribute__ ((access (read_only, 1))) UserBuffer::Write(char* Buf, int bytes){
        if(bytes > Writeable()){
            Move_Buffer();
        }
        if(bytes > Writeable()){
            throw std::out_of_range("'UserBuffer::Write' The parameter is not in the valid range");
        }
        memcpy(Buffer_.get() + Write_Spot, Buf, static_cast<size_t>(bytes));
        Write_Spot += static_cast<size_t>(bytes);
        return bytes;
    } 

    int __attribute__ ((access (read_only, 1))) UserBuffer::Write(const char* Buf, int bytes){
        if(bytes > Writeable()){
            Move_Buffer();
        }
        if(bytes > Writeable()){
            throw std::out_of_range("'UserBuffer::Write' The parameter is not in the valid range");
        }
        memcpy(Buffer_.get() + Write_Spot, Buf, static_cast<size_t>(bytes));
        Write_Spot += static_cast<size_t>(bytes);
        return bytes;
    }

    int UserBuffer::Write(const std::string& str){
        size_t len = str.length();
        if(len > Writeable()){
            Move_Buffer();
        }
        if(len > Writeable()){
            throw std::out_of_range("'UserBuffer::Write' The parameter is not in the valid range");
        }
        memcpy(Buffer_.get() + Write_Spot, str.c_str(), len);
        Write_Spot += len;
        return static_cast<int>(len);
    }

    int __attribute__ ((access (read_only, 1))) UserBuffer::SWrite(const char* format, va_list para){
        return vsnprintf(Buffer_.get() + Write_Spot, static_cast<size_t>(Writeable()), format, para);
    }

    void __attribute__((hot)) UserBuffer::Move_Buffer(){
        // 并不是每一次都会拷贝，在读指针大于缓存的一半的时候再拷贝；
        if(Read_Spot < Buffer_Size / 2){
            return;
        }
        size_t inter = Write_Spot - Read_Spot;
        memcpy(Buffer_.get(), Buffer_.get() + Read_Spot, inter);
        Read_Spot = 0;
        Write_Spot = inter;
        return;
    }
}