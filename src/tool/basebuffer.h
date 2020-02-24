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

#ifndef BASEBUFFER_H_
#define BASEBUFFER_H_

#include <cstddef>
#include <memory>

namespace ws{
    class BaseBuffer{
        public:
            virtual size_t Readable() const = 0;
            virtual const char* ReadPtr() const = 0;
            virtual std::unique_ptr<char[]> Read(int bytes) = 0;
            virtual void Read(char* Start, int bytes) = 0;
            virtual size_t Writeable() const = 0;
            virtual char* WritePtr() = 0;
            virtual char Peek(int jump) const = 0;
            virtual size_t Length() const = 0;
    };
}




#endif