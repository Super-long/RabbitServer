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

#ifndef USERBUFFER_H_
#define USERBUFFER_H_

#include "../base/nocopy.h"
#include "basebuffer.h"
#include <memory>
#include <string>
#include <stdexcept>
 
namespace ws{
    class UserBuffer : public Nocopy,public BaseBuffer{
        public:
            explicit UserBuffer(int Length = 4048) : Buffer_Size(Length),Write_Spot(0),
            Read_Spot(0),Buffer_(std::make_unique<char[]>(static_cast<size_t>(Length))) {}

            size_t Length() const noexcept final{return Buffer_Size;}
            size_t Readable() const noexcept final{return Write_Spot - Read_Spot;}
            size_t Writeable() const noexcept final{return Buffer_Size - Write_Spot;}
            
            const char* ReadPtr() const final{return Buffer_.get() + Read_Spot;}
            char* WritePtr() final{return Buffer_.get() + Write_Spot;}
            /*In most cases it is dangerous to operate the pointer directly.*/

            std::unique_ptr<char[]> Read(int Bytes) final;
            void Read(char* Start, int bytes) final;
            void read(int bytes) {
                if(bytes < 0) throw std::invalid_argument("'userbuffer::read' error paramater.");
                Read_Spot += bytes;
            }
            //void Read(std::string& str, int bytes);
            char Peek(int jump) const final;
            //The above function inherits from the base class.

            int Write(int bytes);
            int Write(char* Buf, int bytes);
            int Write(const char* Buf,int bytes);
            int Write(const std::string& str);
            int SWrite(const char* , va_list);

            void Clean(){Write_Spot = 0,Read_Spot = 0;}
            size_t WSpot()const noexcept {return Write_Spot;}
            size_t RSpot()const noexcept {return Read_Spot;}
            void ReWirte(int spot) noexcept{Write_Spot = static_cast<size_t>(spot);}
            void Move_Buffer(); 

        private:
            std::unique_ptr<char[]> Buffer_;
            size_t Write_Spot;
            size_t Read_Spot;
            size_t Buffer_Size;
    };
}

#endif 