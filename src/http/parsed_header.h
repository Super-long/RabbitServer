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

#ifndef PARSED_HEADER_H_
#define PARSED_HEADER_H_

#include "../base/copyable.h"
#include "../tool/basebuffer.h"
#include <memory>
#include <string>
#include <cstring>
#include <fstream> 
#include <functional>
#include <iostream>

namespace ws{

    class ParsedHeader : public Copyable, public BaseBuffer{
        public:
            explicit ParsedHeader(const char* ptr) : length(strlen(ptr)),Header(ptr){}        
            ParsedHeader(const char* ptr,int len) :
                length(static_cast<size_t>(len)),Header(ptr){}
            ParsedHeader() = default;
            size_t Readable() const override {return length;}
            const char* ReadPtr() const override{return Header;}
            void __attribute__ ((access (write_only, 1))) Read(char* Start, int bytes){
                memcpy(Start, Header, static_cast<size_t>(bytes));
            }
            
            [[deprecated]] virtual std::unique_ptr<char[]> Read(int bytes){
                //This is a design error. 
                return nullptr;
            }
            size_t Writeable() const {
                return length;
            }
            virtual __attribute__((returns_nonnull)) char* WritePtr(){
                return const_cast<char*>(Header);
            }

            size_t Length() const {return length;}
            char Peek(int jump) const {
                if(jump > length) throw std::out_of_range("'ParsedHeader Peek' Out of max length.");
                return Header[jump];
            }

            bool operator==(const ParsedHeader& ) const;
            bool ParsedHeaderIsEqual(const ParsedHeader& ) const;
            bool ParsedHeaderIsEqual(const char* ) const;
            bool ParsedHeaderIsEqual(const std::string& ) const;

            ~ParsedHeader(){}

        private:
            const char* Header;//获取到的指针本来就是智能指针
            size_t length;
    };

    std::ostream& operator<<(std::ostream&, const ParsedHeader&);
    
    struct ParseHeaderHash{
        size_t operator()(const ParsedHeader& ptr) const {
            return std::hash<const char*>()(ptr.ReadPtr()) + std::hash<size_t>()(ptr.Readable());
        }
    };
}



#endif