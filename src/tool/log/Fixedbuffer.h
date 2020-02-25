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

#ifndef FIXEDBUFFER_H_
#define FIXEDBUFFER_H_

#include <memory>
#include <string>
#include <functional>
#include <algorithm>
#include <cstring>

#include "../../base/nocopy.h"

namespace ws{

namespace detail{

constexpr int SmallBuffer = 4048;
constexpr int LargerBuffer = 4048*1000; //感觉有点太大了

template<int BufferSize>
class FixedBuffer : public Nocopy{
public:
    FixedBuffer() : Spot(0), Buffer_(std::make_unique<char[]>(BufferSize)){
        setcookie(CookieStart); 
    }
    
    void setcookie(std::function<void()> cookie) {cookie_ = std::move(cookie);}
    constexpr uint32_t Length() const noexcept {return Spot;}
    uint32_t avail() const {return BufferSize - Spot;}
    void add(uint32_t len) {Spot += len;}
    void setSpotBegin() {Spot = 0;}
    char* current() const {return Buffer_.get() + Spot;}
    const char* data() const {return Buffer_.get();}
    void setZero() {memset(Buffer_.get(), 0, BufferSize);}
    void reset() {Spot = 0;}
    std::string toString() const {return std::string(Buffer_.get(), Spot);}

    void append(const char* buf, size_t len){
        if(avail() > len){
            //TODO 拷贝有没有更优的
            memcpy(Buffer_.get() + Spot, buf, len);
            Spot += len;
        }
    }
private:
    std::unique_ptr<char[]> Buffer_;
    std::function<void()> cookie_;
    uint32_t Spot;

    const char* end() const {return std::advance(Buffer_, Length());}
    static void CookieStart(){};
    static void CookieEnd(){};
};

}

}

#endif 